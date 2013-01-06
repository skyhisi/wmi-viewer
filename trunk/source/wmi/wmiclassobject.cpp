#include "wmiclassobject.hpp"

#include <stdexcept>

#include "qtcominterop.hpp"

WmiClassObject::WmiClassObject(IWbemClassObject* obj, bool take) :
	WmiCloneable<IWbemClassObject>(obj, take),
	WmiComparable<IWbemClassObject>(obj, take),
	ComPointer<IWbemClassObject>(obj,take)
{}

QStringList WmiClassObject::propertyNames(NameSource source) const
{
	if (!valid())
		return QStringList();
		
	long flags = WBEM_FLAG_ALWAYS;
	switch (source)
	{
		case NameSourceAll: break;
		case NameSourceLocal: flags |= WBEM_FLAG_LOCAL_ONLY; break;
		case NameSourcePropagated: flags |= WBEM_FLAG_PROPAGATED_ONLY; break;
		case NameSourceSystem: flags |= WBEM_FLAG_SYSTEM_ONLY; break;
		case NameSourceNonSystem: flags |= WBEM_FLAG_NONSYSTEM_ONLY; break;
	}
		
	SafeArrayScoped nameArray(0);
	HRESULT hr = get()->GetNames(0, flags, 0, nameArray.ptr());
	if (FAILED(hr))
	{
		qWarningFromHresult(hr, "Can not get property names");
		return QStringList();
	}
	return nameArray.toStringList();
}

QVariant WmiClassObject::getProperty(const QString& name) const
{
	if (!valid())
		return QVariant();
	VARIANT v;
	const BStrScoped bsname(name);
	const HRESULT hr = get()->Get(bsname, 0, &v, 0, 0);
	if (FAILED(hr))
	{
		qWarningFromHresult(hr, "Can not get property");
		return QVariant();
	}
	
	const QVariant ret(fromComVariant(v));
	VariantClear(&v);
	return ret;
}

bool WmiClassObject::setProperty(const QString& name, const QVariant& variant)
{
	if (!valid())
		return false;
	
	VARIANT v;
	if (!toComVariant(variant, v))
		return false;
	
	BStrScoped bsname(name);
	const HRESULT hr = get()->Put(bsname, 0, &v, 0);
	VariantClear(&v);
	if (FAILED(hr))
	{
		qWarningFromHresult(hr, "Can not set property");
		return false;
	}
	return true;
}

WmiQualifierSet WmiClassObject::classQualifierSet() const
{
	if (!valid())
	{
		qWarning() << "Class object not valid";
		return WmiQualifierSet();
	}
	
	IWbemQualifierSet* qset = 0;
	HRESULT hr = get()->GetQualifierSet(&qset);
	if (FAILED(hr))
	{
		qWarningFromHresult(hr, "Can not get qualifier set");
		return WmiQualifierSet();
	}
	return WmiQualifierSet(qset);
}

WmiQualifierSet WmiClassObject::propertyQualifierSet(const QString& propName) const
{
	if (!valid())
	{
		qWarning() << "Class object not valid";
		return WmiQualifierSet();
	}
	IWbemQualifierSet* qset = 0;
	HRESULT hr = get()->GetPropertyQualifierSet(reinterpret_cast<LPCWSTR>(propName.utf16()), &qset);
	if (FAILED(hr))
	{
		qWarningFromHresult(hr, "Can not get property qualifier set");
		return WmiQualifierSet();
	}
	return WmiQualifierSet(qset);
}

