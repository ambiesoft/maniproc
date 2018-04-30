#include "stdafx.h"
#include "helper.h"
#include "CJob.h"


using namespace Ambiesoft;
using namespace std;


int SetIOPriority(HANDLE hProcess, IO_PRIORITY_HINT ioph)
{
	BOOLEAN success = TRUE;
	BOOLEAN cancelled = FALSE;
	NTSTATUS status;
	DWORD dwProcess = GetProcessId(hProcess);

	if ((POINTERSIZE)dwProcess != (POINTERSIZE)SYSTEM_PROCESS_ID)
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

bool SetPriority(HANDLE hProcess, DWORD basepriority)
{
	if (basepriority == PROCESS_MODE_BACKGROUND_BEGIN)
	{
		if (!SetPriorityClass(hProcess, IDLE_PRIORITY_CLASS))
		{
			ShowLastError(GetLastError());
			return false;
		}

		NTSTATUS status = SetIOPriority(hProcess, IoPriorityVeryLow);
		if (!NT_SUCCESS(status))
		{
			ShowNtStatusError(status);
			return false;
		}
		return true;
	}

	if (!SetPriorityClass(hProcess, basepriority))
	{
		ShowLastError(GetLastError());
		return false;
	}
	return true;
}

bool CPriorityJob::DoWork()
{
	int failcount = 0;
	for (HANDLE h : handles_)
	{
		if (!SetPriority(h, priority_))
			failcount++;
	}
	return failcount == 0;
}
