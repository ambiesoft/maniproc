#pragma once

#define MYDEFINECOMPTR(I) _COM_SMARTPTR_TYPEDEF(I,__uuidof(I))

MYDEFINECOMPTR(IWbemLocator);
MYDEFINECOMPTR(IWbemServices);
MYDEFINECOMPTR(IEnumWbemClassObject);
MYDEFINECOMPTR(IWbemClassObject);

class CWmi
{
	bool ok_ = false;
	// HRESULT hr_ = 0;
	bstr_t error_;
	IWbemLocatorPtr pLoc_;
	IWbemServicesPtr pSvc_;

	void SetError(HRESULT hr);
public:
	CWmi();
	~CWmi();

	bstr_t lastError() const {
		return error_;
	}
	operator bool() const {
		return ok_;
	}

	
	bool GetProcesses(
		std::vector<HANDLE>& handles, 
		const std::wstring& query,
		int limit,
		bool& needUac);
};

