#include "wmiclassobjectenum.hpp"

#include <stdexcept>

#include "wmiservice.hpp"
#include "qtcominterop.hpp"

WmiClassObjectEnum::WmiClassObjectEnum(IEnumWbemClassObject* obj, bool take) :
	ComPointer<IEnumWbemClassObject>(obj, take)
{}

WmiClassObject WmiClassObjectEnum::next(long timeout)
{
	IWbemClassObject* obj = 0;
	ULONG ret = 0;
	get()->Next(timeout, 1, &obj, &ret);
	return WmiClassObject((ret == 0) ? 0 : obj);
}

bool WmiClassObjectEnum::nextBatch(int max, long timeout, QList<WmiClassObject>& appendTo)
{
	if (max < 0 || timeout < 0)
		return false;
	
	QVector<IWbemClassObject*> objs(max);
	ULONG ret = 0;
	HRESULT hr = get()->Next(timeout, max, objs.data(), &ret);
	for (ULONG i = 0; i < ret; ++i)
		appendTo.append(WmiClassObject(objs[i]));
	return (hr == WBEM_S_NO_ERROR || hr == WBEM_S_TIMEDOUT);
}
