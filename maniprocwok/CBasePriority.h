#pragma once
class CBasePriority
{
private:
	CBasePriority();
	~CBasePriority();
public:
	static std::wstring getHelpString();
	static DWORD getPriority(const std::wstring& basepriority, std::wstring& error);
};

