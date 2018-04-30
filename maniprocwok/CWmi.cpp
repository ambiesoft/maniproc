#include "stdafx.h"



#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "Ole32.lib")
#include "CWmi.h"




using namespace std;
CWmi::CWmi()
{
	hr_ = CoInitialize(NULL);
	if(FAILED(hr_))
		return;

	hr_ = CoInitializeSecurity(
		NULL,                       // security descriptor
		-1,                          // use this simple setting
		NULL,                        // use this simple setting
		NULL,                        // reserved
		RPC_C_AUTHN_LEVEL_DEFAULT,   // authentication level  
		RPC_C_IMP_LEVEL_IMPERSONATE, // impersonation level
		NULL,                        // use this simple setting
		EOAC_NONE,                   // no special capabilities
		NULL);                          // reserved

	if (FAILED(hr_))
		return;


	hr_ = CoCreateInstance(
		CLSID_WbemLocator, 
		0,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, 
		(void**)&pLoc_);

	if (FAILED(hr_))
		return;


	// Connect to the root\default namespace with the current user.
	hr_ = pLoc_->ConnectServer(
		bstr_t(L"ROOT\\CIMV2"),  //namespace
		NULL,       // User name 
		NULL,       // User password
		0,         // Locale 
		NULL,     // Security flags
		0,         // Authority 
		0,        // Context object 
		&pSvc_);   // IWbemServices proxy

	if (FAILED(hr_))
	{
		return;
	}


	hr_ = CoSetProxyBlanket(pSvc_,
		RPC_C_AUTHN_WINNT,
		RPC_C_AUTHZ_NONE,
		NULL,
		RPC_C_AUTHN_LEVEL_CALL,
		RPC_C_IMP_LEVEL_IMPERSONATE,
		NULL,
		EOAC_NONE
	);

	if (FAILED(hr_))
		return;

	ok_ = true;
}


CWmi::~CWmi()
{
	CoUninitialize();
}
