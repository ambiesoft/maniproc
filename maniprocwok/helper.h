#pragma once

void ShowText(const std::wstring& text, bool bError);
void ShowInfo(const std::wstring& text);
void ShowError(const std::wstring& error);
void ShowHelp(const std::wstring& help);
void ShowVersion();
void ShowLastError(DWORD dw);
void ShowNtStatusError(NTSTATUS status);
void ShowHResultError(HRESULT hr);