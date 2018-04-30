#include "stdafx.h"
#include "global.h"
#include <sstream>
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
	_com_error err(hr);
	wstring message = err.ErrorMessage();

	// wstring message = GetLastErrorString(hr);
	if (message.empty())
	{
		wstringstream ws;
		ws << I18NS(L"Unknown error") << " 0x" << std::hex << hr;
		message = ws.str();
	}
	ShowError(message);
}