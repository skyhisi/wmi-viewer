#ifndef _WMIVIEWER_WMISERVICE_HPP_
#define _WMIVIEWER_WMISERVICE_HPP_

#include "wmibase.hpp"
#include <QtCore>

#include "wmiclassobject.hpp"
#include "wmiclassobjectenum.hpp"


class EXPORT WmiService : public ComPointer<IWbemServices>
{
	public:
		WmiService(IWbemServices* svc = 0, bool take = true);
		
		WmiClassObject getObject(const QString& objectPath) const;
		
		WmiClassObjectEnum execQuery(const QString& query, bool notification = false) const;
		
};

#endif
