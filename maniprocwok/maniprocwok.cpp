// maniprocwok.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"


// #include <phnt_ntdef.h>


#include "Macro.h"
#include "CBasePriority.h"
#include "global.h"
#include "helper.h"
#include "CJob.h"
#include "CWmi.h"

using namespace Ambiesoft;
using namespace std;

bool OnCmdStart(
	const wstring& exe,
	const wstring& arg,
	HANDLE* phHandle,
	wstring& error)
{
	DWORD dwLastError = 0;
	HANDLE hProcess = NULL;
	if (!CreateProcessCommon(exe.c_str(),
		arg.c_str(),
		false,
		&dwLastError,
		WaitProcess_None,
		0,
		&hProcess))
	{
		error = GetLastErrorString(dwLastError);
		return false;
	}
	// STLSOFT_SCOPEDFREE_HANDLE(hProcess);
	*phHandle = hProcess;
	return true;
}

int workmain(bool bGui)
{
	gbGui = bGui;
	CWmi wmi;
	if (!wmi)
	{
		ShowError((LPWSTR)wmi.lastError());
		return 1;
	}
	// -cmd start -basepriority background -exe notepad.exe
	// -cmd start -basepriority background -exe "C:\LegacyPrograms\Auslogics Disk Defrag\DiskDefrag.exe" -arg C:\Linkout\ 
	// -wql "Name='notepaD.exe'" -basepriority background -limit 1
	// X -wql "Name='notepaD.exe' Limit 1"

	CCommandLineParser parser;
	wstring cmd;
	parser.AddOption(L"-cmd",
		1,
		&cmd,
		ArgEncodingFlags_Default,
		I18NS(L"Command: find or start"));

	wstring exe;
	parser.AddOption(L"-exe",
		1,
		&exe,
		ArgEncodingFlags_Default,
		I18NS(L"Executable to start"));

	wstring arg;
	parser.AddOption(L"-arg",
		1,
		&arg,
		ArgEncodingFlags_Default,
		I18NS(L"Command line argument for starting executable"));

	//wstring name;
	//parser.AddOption(L"-name",
	//	1,
	//	&name,
	//	ArgEncodingFlags_Default,
	//	I18NS(L"Filename to find processes"));

	wstring wql;
	parser.AddOption(L"-wql",
		1,
		&wql,
		ArgEncodingFlags_Default,
		I18NS(L"'Where' clause of WMI sql for quering processes"));

	int limit = -1;
	parser.AddOption(L"-limit",
		1,
		&limit,
		ArgEncodingFlags_Default,
		I18NS(L"Set limit of wql results"));

	wstring basepriority;
	parser.AddOption(L"-basepriority",
		1,
		&basepriority,
		ArgEncodingFlags_Default,
		I18NS(L"Base Priority:") + wstring(L" ") + CBasePriority::getHelpString());

	wstring iopriority;
	parser.AddOption(L"-iopriority",
		1,
		&iopriority,
		ArgEncodingFlags_Default,
		I18NS(L"IO Priority:") + wstring(L" ") + CBasePriority::getHelpString());

	bool uac = false;
	parser.AddOption(L"-uac",
		0,
		&uac,
		ArgEncodingFlags_Default,
		I18NS(L"Elevate to administrator if neccessary"));

	//bool bShow = false;
	//parser.AddOption(L"-s",
	//	0,
	//	&bShow,
	//	ArgEncodingFlags_Default,
	//	I18NS(L"Show process information"));

	bool bHelp = false;
	parser.AddOption(
		L"-h", L"/?",
		0,
		&bHelp,
		ArgEncodingFlags_Default,
		L"Show Help");

	bool bVersion = false;
	parser.AddOption(L"-version",
		0,
		&bVersion,
		ArgEncodingFlags_Default,
		I18NS(L"Show application version"));

	COption opExe(L"");
	parser.AddOption(&opExe);

	parser.Parse();

	if (bHelp)
	{
		ShowHelp(parser.getHelpMessage());
		return 0;
	}
	if (bVersion)
	{
		ShowVersion();
		return 0;
	}

	vector<HANDLE> handles;
	struct HandleCloser {
		vector<HANDLE>* hs;
		HandleCloser(vector<HANDLE>* p)
		{
			hs = p;
		}
		~HandleCloser() {
			for (HANDLE h : *hs)
			{
				CloseHandle(h);
			}
		}
	} handleCloser(&handles);

	if (cmd == L"start")
	{
		if (exe.empty() && arg.empty())
		{
			ShowError(I18NS(L"exe or arg must be specified to start process"));
			return 1;
		}
		HANDLE h = NULL;
		wstring error;
		if (!OnCmdStart(exe, arg, &h, error))
		{
			ShowError(error);
			return 1;
		}
		handles.push_back(h);
	}
	else if (cmd.empty() || cmd == L"find")
	{
		if (wql.empty())
		{
			ShowError(I18NS(L"No wql specified"));
			return 1;
		}
		bool needUac = false;
		if (!wmi.GetProcesses(handles, wql, limit, needUac))
		{
			if (needUac && uac)
			{
				wstring me, mearg;
				CCommandLineString::ExplodeExeAndArg(GetCommandLine(), me, mearg);

				if (!OpenCommon(NULL,
					me.c_str(),
					mearg.c_str(),
					NULL,
					NULL,
					L"runas"))
				{
					ShowError(L"Failed to launch myself as administrator.");
					return 1;
				}
				return 0;
			}
			wstring error = I18NS(L"Failed to get processes.");
			error += L"\r\n\r\n";
			error += (LPCWSTR)wmi.lastError();
			ShowError(error);
			return 1;
		}
	}

	// handles found, now do action
	if (handles.empty())
	{
		ShowInfo(I18NS(L"No processes found"));
		return 0;
	}

	bool actionTaken = false;
	wstring error;
	bool bSetPriority = false;
	DWORD dwPriority = 0;
	if (!basepriority.empty())
	{
		dwPriority = CBasePriority::getPriority(basepriority, error);
		if (!error.empty())
		{
			ShowError(error);
			return 1;
		}
		bSetPriority = true;
	}

	if (bSetPriority)
	{
		CPriorityJob job(handles, dwPriority);
		if (!job.DoWork())
			return 1;

		actionTaken = true;
	}

	if (!actionTaken)
	{
		ShowInfo(I18NS(L"No action specified"));
		return 0;
	}
	//if (!opExe.hadValue())
	//{
	//	ShowError(I18NS(L"No target specified"));
	//	return 1;
	//}
	return 0;


}