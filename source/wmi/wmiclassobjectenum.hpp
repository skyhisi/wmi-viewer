#ifndef _WMIVIEWER_WMICLASSOBJECTENUM_HPP_
#define _WMIVIEWER_WMICLASSOBJECTENUM_HPP_

#include "wmibase.hpp"
#include <QtCore>
#include "wmiclassobject.hpp"

class EXPORT WmiClassObjectEnum : public ComPointer<IEnumWbemClassObject>
{
	public:
		WmiClassObjectEnum(IEnumWbemClassObject* enumClassObject = 0, bool take = true);
		
		WmiClassObject next(long timeout = WBEM_INFINITE);
		bool nextBatch(int max, long timeout, QList<WmiClassObject>& appendTo);
};

#endif
