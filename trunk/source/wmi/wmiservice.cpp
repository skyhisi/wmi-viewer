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

WmiClassObjectEnum WmiService::execQuery(const QString& query, QString* errorMsg) const
{
	if (errorMsg) errorMsg->clear();
	if (!valid())
	{
		qWarning() << "Service invalid";
		if (errorMsg) *errorMsg = QObject::tr("Internal Error");
		return WmiClassObjectEnum();
	}
	
	IEnumWbemClassObject* enumer = 0;
	BStrScoped bsq(query);
	const HRESULT hr = get()->ExecQuery(BSTR(L"WQL"), bsq,
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, 0, &enumer);
	if (FAILED(hr))
	{
		qWarningFromHresult(hr, "ExecQuery");
		if (errorMsg) *errorMsg = QString::fromLatin1(HResultToName(hr));
		return WmiClassObjectEnum();
	}
	return WmiClassObjectEnum(enumer);
}

WmiClassObjectEnum WmiService::execNotificationQuery(const QString& query, QString* errorMsg) const
{
	if (errorMsg) errorMsg->clear();
	if (!valid())
	{
		qWarning() << "Service invalid";
		if (errorMsg) *errorMsg = QObject::tr("Internal Error");
		return WmiClassObjectEnum();
	}
	
	IEnumWbemClassObject* enumer = 0;
	BStrScoped bsq(query);
	const HRESULT hr = get()->ExecNotificationQuery(BSTR(L"WQL"), bsq,
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, 0, &enumer);
	if (FAILED(hr))
	{
		qWarningFromHresult(hr, "ExecNotificationQuery");
		if (errorMsg) *errorMsg = QString::fromLatin1(HResultToName(hr));
		return WmiClassObjectEnum();
	}
	return WmiClassObjectEnum(enumer);
}

WmiClassObjectEnum WmiService::createInstanceEnum(const QString& className, QString* errorMsg) const
{
	if (errorMsg) errorMsg->clear();
	if (!valid())
	{
		qWarning() << "Service invalid";
		if (errorMsg) *errorMsg = QObject::tr("Internal Error");
		return WmiClassObjectEnum();
	}
	
	IEnumWbemClassObject* enumer = 0;
	BStrScoped bsq(className);
	const HRESULT hr = get()->CreateInstanceEnum(bsq,
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, 0, &enumer);
	if (FAILED(hr))
	{
		qWarningFromHresult(hr, "CreateInstanceEnum");
		if (errorMsg) *errorMsg = QString::fromLatin1(HResultToName(hr));
		return WmiClassObjectEnum();
	}
	return WmiClassObjectEnum(enumer);
}

WmiClassObjectEnum WmiService::createClassEnum(const QString& superClassName, ClassDepth depth, QString* errorMsg) const
{
	if (errorMsg) errorMsg->clear();
	if (!valid())
	{
		qWarning() << "Service invalid";
		if (errorMsg) *errorMsg = QObject::tr("Internal Error");
		return WmiClassObjectEnum();
	}
	
	IEnumWbemClassObject* enumer = 0;
	BStrScoped bsq(superClassName);
	long flags = WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY;
	if (depth == ClassDepthDeep)
		flags |= WBEM_FLAG_DEEP;
	else if (depth == ClassDepthShallow)
		flags |= WBEM_FLAG_SHALLOW;
	
	const HRESULT hr = get()->CreateClassEnum(
		(superClassName.isEmpty() ? (BSTR)(0) : bsq), flags, 0, &enumer);
	if (FAILED(hr))
	{
		qWarningFromHresult(hr, "CreateClassEnum");
		if (errorMsg) *errorMsg = QString::fromLatin1(HResultToName(hr));
		return WmiClassObjectEnum();
	}
	return WmiClassObjectEnum(enumer);
}


