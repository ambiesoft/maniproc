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

int OnCmdStart(
	const wstring& exe,
	const wstring& arg,
	bool bSetPriority,
	DWORD basepriority)
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
		ShowLastError(dwLastError);
		return 1;
	}
	STLSOFT_SCOPEDFREE_HANDLE(hProcess);

	if (bSetPriority)
	{
		CPriorityJob job({ hProcess }, basepriority);
		if (job.DoWork())
			return 1;
	}

	return 0;
}
int OnCmdFind(const wstring& name)
{

	return 0;
}
int workmain(bool bGui)
{
	gbGui = bGui;
	CWmi wmi;
	if (!wmi)
	{
		ShowHResultError(wmi.lastError());
		return 1;
	}
	// -cmd start -basepriority background -exe notepad.exe
	// -cmd start -basepriority background -exe "C:\LegacyPrograms\Auslogics Disk Defrag\DiskDefrag.exe" -arg C:\Linkout\ 

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

	wstring name;
	parser.AddOption(L"-name",
		1,
		&name,
		ArgEncodingFlags_Default,
		I18NS(L"Filename to find processes"));

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

	bool bShow = false;
	parser.AddOption(L"-s",
		0,
		&bShow,
		ArgEncodingFlags_Default,
		I18NS(L"Show process information"));

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

	if (cmd == L"start")
	{
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
		return OnCmdStart(exe, arg, bSetPriority, dwPriority);
	}
	else if (cmd.empty() || cmd == L"find")
	{
		return OnCmdFind(name);
	}
	if (!opExe.hadValue())
	{
		ShowError(I18NS(L"No target specified"));
		return 1;
	}
	return 0;


}