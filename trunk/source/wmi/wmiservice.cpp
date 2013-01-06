#include "wmiservice.hpp"

#include <stdexcept>

#include "qtcominterop.hpp"

WmiService::WmiService(IWbemServices* svc, bool take) :
	ComPointer<IWbemServices>(svc, take)
{}

WmiClassObject WmiService::getObject(const QString& objectPath) const
{
	if (!valid())
	{
		qWarning() << "Service invalid";
		return WmiClassObject();
	}
	
	const long flags = WBEM_FLAG_USE_AMENDED_QUALIFIERS | WBEM_FLAG_RETURN_WBEM_COMPLETE;
	IWbemClassObject* obj = 0;
	BStrScoped op(objectPath);
	HRESULT hr = get()->GetObject(op, flags, 0, &obj, 0);
	if (FAILED(hr))
	{
		qWarningFromHresult(hr, "Could not get object");
		return WmiClassObject();
	}
	return WmiClassObject(obj);
}

WmiClassObjectEnum WmiService::execQuery(const QString& query, bool notification) const
{
	if (!valid())
	{
		qWarning() << "Service invalid";
		return WmiClassObjectEnum();
	}
	
	IEnumWbemClassObject* enumer = 0;
	BStrScoped bsq(query);
	HRESULT hr;
	if (notification)
	{
		hr = get()->ExecNotificationQuery(
			BSTR(L"WQL"), bsq,
			WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
			0, &enumer);
	}
	else
	{
		hr = get()->ExecQuery(
			BSTR(L"WQL"), bsq,
			WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
			0, &enumer);
	}
	if (FAILED(hr))
	{
		qWarningFromHresult(hr, "Query execution failed");
		return WmiClassObjectEnum();
	}
	return WmiClassObjectEnum(enumer);
}
