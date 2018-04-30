#include "stdafx.h"



#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "Ole32.lib")
#include "CWmi.h"




using namespace std;
using namespace Ambiesoft;
using namespace stdwin32;

#define SetErrorReturn(hr) do { if(FAILED(hr)) {SetError(hr); return;} } while(false)
#define SetErrorReturnVal(hr) do { if(FAILED(hr)) {SetError(hr); return false;} } while(false)
CWmi::CWmi()
{
	HRESULT hr = CoInitialize(NULL);
	SetErrorReturn(hr);

	hr = CoInitializeSecurity(
		NULL,                       // security descriptor
		-1,                          // use this simple setting
		NULL,                        // use this simple setting
		NULL,                        // reserved
		RPC_C_AUTHN_LEVEL_DEFAULT,   // authentication level  
		RPC_C_IMP_LEVEL_IMPERSONATE, // impersonation level
		NULL,                        // use this simple setting
		EOAC_NONE,                   // no special capabilities
		NULL);                          // reserved
	SetErrorReturn(hr);


	hr = CoCreateInstance(
		CLSID_WbemLocator, 
		0,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, 
		(void**)&pLoc_);

	SetErrorReturn(hr);

	// Connect to the root\default namespace with the current user.
	hr = pLoc_->ConnectServer(
		bstr_t(L"ROOT\\CIMV2"),  //namespace
		NULL,       // User name 
		NULL,       // User password
		0,         // Locale 
		NULL,     // Security flags
		0,         // Authority 
		0,        // Context object 
		&pSvc_);   // IWbemServices proxy

	SetErrorReturn(hr);
	
	
	hr = CoSetProxyBlanket(pSvc_,
		RPC_C_AUTHN_WINNT,
		RPC_C_AUTHZ_NONE,
		NULL,
		RPC_C_AUTHN_LEVEL_CALL,
		RPC_C_IMP_LEVEL_IMPERSONATE,
		NULL,
		EOAC_NONE
	);

	SetErrorReturn(hr);
	ok_ = true;
}

void CWmi::SetError(HRESULT hr)
{
	IErrorInfoPtr p = NULL;
	error_ = L"";
	//pLoc_->QueryInterface(IID_IErrorInfo, (void**)&p);
	if (SUCCEEDED(GetErrorInfo(0, &p)))
	{
		_com_error err(hr, p);
		error_ += L"Description: " + err.Description();
		error_ += L"\r\n";
		error_ += bstr_t(L"ErrorMessage: ") + err.ErrorMessage();
		error_ += L"\r\n";
		error_ += L"HelpFile: " + err.HelpFile();
		error_ += L"\r\n";
		error_ += L"Source: " + err.Source();
	}
}
bool CWmi::GetProcesses(
	std::vector<HANDLE>& handles,
	const wstring& where,
	int limit)
{
	if (!pSvc_)
		return false;

	HRESULT hr;
	wstring query = L"Select Handle From Win32_Process";
	if (!where.empty())
	{
		query += L" Where ";
		query += where;
	}

	IEnumWbemClassObjectPtr pEnumerator;
	hr = pSvc_->ExecQuery(
		bstr_t(L"WQL"),
		bstr_t(query.c_str()),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL,
		&pEnumerator);
	SetErrorReturnVal(hr);
	
	IWbemClassObjectPtr pclsObj;
	ULONG uReturn = 0;
	vector<DWORD> ids;
	int found = 0;
	while (pEnumerator)
	{
		if (limit >= 0)
		{
			if (limit <= found)
				break;
		}

		hr = pEnumerator->Next(
			WBEM_INFINITE,
			1,
			&pclsObj, 
			&uReturn);
		SetErrorReturnVal(hr);

		if (0 == uReturn)
			break;

		variant_t vtProp;

		// Get the value of the Name property
		hr = pclsObj->Get(
			L"Handle",
			0, 
			&vtProp,
			0,
			0);
		SetErrorReturnVal(hr);
		
		DASSERT(vtProp.vt == VT_BSTR);
		
		found++;
		ids.push_back(std::stol(vtProp.bstrVal));
	}

	for (DWORD id : ids)
	{
		HANDLE h = OpenProcess(
			PROCESS_SET_INFORMATION | PROCESS_SUSPEND_RESUME,
			TRUE,
			id);
		if (!h)
		{
			continue;
		}
		handles.push_back(h);
	}
	return true;
}

CWmi::~CWmi()
{
	// CoUninitialize();
}
