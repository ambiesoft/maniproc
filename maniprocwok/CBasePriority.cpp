#include "stdafx.h"
#include <map>
#include "CBasePriority.h"

using namespace std;

struct SSS {
	wstring s_;
	DWORD d_;
	SSS(wstring s, DWORD d) {
		s_ = s;
		d_ = d;
	}
	SSS(const SSS& sss)
	{
		s_ = sss.s_;
		d_ = sss.d_;
	}
};
static const SSS sBasePriorities[] = {
	SSS(L"realtime", REALTIME_PRIORITY_CLASS),
	SSS(L"high", HIGH_PRIORITY_CLASS),
	SSS(L"above", ABOVE_NORMAL_PRIORITY_CLASS),
	SSS(L"normal", NORMAL_PRIORITY_CLASS),
	SSS(L"below", BELOW_NORMAL_PRIORITY_CLASS),
	SSS(L"idle", IDLE_PRIORITY_CLASS),
	SSS(L"background", PROCESS_MODE_BACKGROUND_BEGIN),
};

CBasePriority::CBasePriority()
{
}


CBasePriority::~CBasePriority()
{
}

std::wstring CBasePriority::getHelpString()
{
	wstring ret;
	for (int i = 0; i < _countof(sBasePriorities); ++i)
	{
		if (i == 0)
		{
			// first
		}
		else
		{
			// not first
			if ((i + 1) == _countof(sBasePriorities))
			{
				// last
				ret += L", or ";
			}
			else
			{
				// not first and last
				ret += L", ";
			}
		}
		ret += L'\'';
		ret += sBasePriorities[i].s_;
		ret += L'\'';
	}
	return ret;
}

DWORD CBasePriority::getPriority(const wstring& basepriority, wstring& error)
{
	for (int i = 0; i < _countof(sBasePriorities); ++i)
	{
		if (sBasePriorities[i].s_ == basepriority)
			return sBasePriorities[i].d_;
	}
	error = I18NS(L"BasePriorty not found");
	return 0;
}