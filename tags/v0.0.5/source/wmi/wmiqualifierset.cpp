#include "wmiqualifierset.hpp"

#include "qtcominterop.hpp"
#include "wmiclassobject.hpp"

WmiQualifierSet::WmiQualifierSet(IWbemQualifierSet* qs, bool take) :
	ComPointer<IWbemQualifierSet>(qs, take)
{}

QStringList WmiQualifierSet::qualifierNames(NameSource source) const
{
	long flags = 0;
	switch (source)
	{
		case NameSourceAll: break;
		case NameSourceLocal: flags = WBEM_FLAG_LOCAL_ONLY; break;
		case NameSourcePropagated: flags = WBEM_FLAG_PROPAGATED_ONLY; break;
	}
	
	SafeArrayScoped nameArray(0);
	HRESULT hr = get()->GetNames(flags, nameArray.ptr());
	if (FAILED(hr))
	{
		qWarningFromHresult(hr, "Can not get qualifier names");
		return QStringList();
	}
	return nameArray.toStringList();
}

QVariant WmiQualifierSet::getProperty(const QString& name) const
{
	VARIANT v;
	BStrScoped bsname(name);
	HRESULT hr = get()->Get(bsname, 0, &v, 0);
	if (FAILED(hr))
	{
		qWarningFromHresult(hr, "Can not get qualifier names");
		return QVariant();
	}
	
	QVariant ret(fromComVariant(v));
	VariantClear(&v);
	return ret;
}

