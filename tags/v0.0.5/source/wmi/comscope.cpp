#include "comscope.hpp"

#include <stdexcept>

ComScope::ComScope() :
	_initialised(false)
{
}

ComScope::~ComScope() throw()
{
	if (_initialised)
	{
		CoUninitialize();
	}
}

bool ComScope::initialise()
{
	if (_initialised)
		return true;
	
	HRESULT hr = CoInitializeEx(0, COINIT_APARTMENTTHREADED); 
	if (FAILED(hr))
	{
		qWarningFromHresult(hr, "COM initialisation failed");
		return false;
	}
	
	hr =  CoInitializeSecurity(
		NULL,                      // Security descriptor    
		-1,                        // COM negotiates authentication service
		NULL,                      // Authentication services
		NULL,                      // Reserved
		RPC_C_AUTHN_LEVEL_DEFAULT, // Default authentication level for proxies
		RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation level for proxies
		NULL,                        // Authentication info
		EOAC_NONE,                   // Additional capabilities of the client or server
		NULL);                       // Reserved

	if (FAILED(hr))
	{
		CoUninitialize();
		qWarningFromHresult(hr, "COM security initialisation failed");
		return false;
	}
	_initialised = true;
	return true;
}
