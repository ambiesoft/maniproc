// maniprocwok.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

// #include <phnt_ntdef.h>

#include "../../lsMisc/CommandLineParser.h"
#include "../../lsMisc/CreateProcessCommon.h"
#include "../../lsMisc/GetLastErrorString.h"
#include "Macro.h"
#include "CBasePriority.h"

using namespace Ambiesoft;
using namespace std;

void ShowHelp(const wstring& help)
{
	wcout << help << endl;
}
void ShowVersion()
{
	wcout << APPVERSION << endl;
}
void ShowError(const wstring& error)
{
	wcout << error << endl;
}
void ShowLastError(DWORD dw)
{
	ShowError(GetLastErrorString(dw));
}
void ShowNtStatusError(NTSTATUS status)
{
	ShowError(GetLastErrorString(status));
}
int SetIOPriority(HANDLE hProcess, IO_PRIORITY_HINT ioph)
{
	BOOLEAN success = TRUE;
	BOOLEAN cancelled = FALSE;
	NTSTATUS status;
	DWORD dwProcess = GetProcessId(hProcess);

	if (dwProcess != (DWORD)SYSTEM_PROCESS_ID)
	{
		status = PhSetProcessIoPriority(hProcess, ioph);
	}
	else
	{
		// See comment in PhUiSetPriorityProcesses.
		status = STATUS_UNSUCCESSFUL;
	}

	// NtClose(hProcess);


	//if (!NT_SUCCESS(status))
	//{
	//	BOOLEAN connected;

	//	success = FALSE;

	//	// The operation may have failed due to the lack of SeIncreaseBasePriorityPrivilege.
	//	if (!cancelled && PhpShowErrorAndConnectToPhSvc(
	//		NULL, //hWnd,
	//		L"Unable to set the I/O priority",
	//		status,
	//		&connected
	//	))
	//	{
	//		if (connected)
	//		{
	//			if (NT_SUCCESS(status = PhSvcCallControlProcess(Processes[i]->ProcessId, PhSvcControlProcessIoPriority, IoPriority)))
	//				success = TRUE;
	//			else
	//				PhpShowErrorProcess(hWnd, L"set the I/O priority of", Processes[i], status, 0);

	//			PhUiDisconnectFromPhSvc();
	//		}
	//		else
	//		{
	//			cancelled = TRUE;
	//		}
	//	}
	//	else
	//	{
	//		if (!PhpShowErrorProcess(hWnd, L"set the I/O priority of", Processes[i], status, 0))
	//			break;
	//	}
	//}

	return status;
}

int OnCmdStart(
	const wstring& exe,
	DWORD basepriority)
{
	DWORD dwLastError = 0;
	HANDLE hProcess = NULL;
	if (!CreateProcessCommon(exe.c_str(),
		nullptr,
		false,
		&dwLastError,
		WaitProcess_None,
		0,
		&hProcess))
	{
		ShowLastError(dwLastError);
		return 1;
	}
	
	if (basepriority == PROCESS_MODE_BACKGROUND_BEGIN)
	{
		if (!SetPriorityClass(hProcess, IDLE_PRIORITY_CLASS))
		{
			ShowLastError(GetLastError());
			return 1;
		}

		NTSTATUS status = SetIOPriority(hProcess, IoPriorityVeryLow);
		if(!NT_SUCCESS(status))
		{
			ShowNtStatusError(status);
			return 1;
		}

		return 0;
	}

	
	if (!SetPriorityClass(hProcess, basepriority))
	{
		ShowLastError(GetLastError());
		return 1;
	}

	return 0;
}

int workmain()
{
	// -cmd start -basepriority background -exe notepad.exe
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
		DWORD dwPriority = CBasePriority::getPriority(basepriority, error);
		if (!error.empty())
		{
			ShowError(error);
			return 1;
		}
		return OnCmdStart(exe, dwPriority);
	}
	if (!opExe.hadValue())
	{
		wcout << I18NS(L"No target specified") << endl;
		return 1;
	}
	return 0;


}