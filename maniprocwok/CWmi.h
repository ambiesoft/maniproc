#pragma once

#define MYDEFINECOMPTR(I) _COM_SMARTPTR_TYPEDEF(I,__uuidof(I))

MYDEFINECOMPTR(IWbemLocator);
MYDEFINECOMPTR(IWbemServices);

class CWmi
{
	bool ok_ = false;
	HRESULT hr_ = 0;

	IWbemLocatorPtr pLoc_;
	IWbemServicesPtr pSvc_;

public:
	CWmi();
	~CWmi();

	HRESULT lastError() const {
		return hr_;
	}
	operator bool() const {
		return ok_;
	}
};

