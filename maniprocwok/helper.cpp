#include "stdafx.h"
#include "global.h"

using namespace std;
using namespace Ambiesoft;

void ShowText(const wstring& text, bool bError)
{
	if (gbGui)
	{
		MessageBox(NULL, text.c_str(), APPNAME,
			bError ? MB_ICONERROR : MB_ICONINFORMATION);
	}
	else
	{
		if (bError)
			wcerr << text << endl;
		else
			wcout << text << endl;
	}
}
void ShowInfo(const wstring& text)
{
	ShowText(text, false);
}
void ShowError(const wstring& error)
{
	ShowText(error, true);
}

void ShowHelp(const wstring& help)
{
	ShowInfo(help);
}
void ShowVersion()
{
	ShowInfo(APPVERSION);
}
void ShowLastError(DWORD dw)
{
	ShowError(GetLastErrorString(dw));
}
void ShowNtStatusError(NTSTATUS status)
{
	ShowError(GetLastErrorString(status));
}
void ShowHResultError(HRESULT hr)
{
	ShowError(GetLastErrorString(hr));
}