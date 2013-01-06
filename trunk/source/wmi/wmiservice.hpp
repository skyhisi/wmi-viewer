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
		
		enum ClassDepth {
			ClassDepthNormal,
			ClassDepthDeep,
			ClassDepthShallow
		};
		
		WmiClassObject getObject(const QString& objectPath) const;
		
		WmiClassObjectEnum execQuery(
			const QString& query, QString* errorMsg = 0) const;
		WmiClassObjectEnum execNotificationQuery(
			const QString& query, QString* errorMsg = 0) const;
		WmiClassObjectEnum createInstanceEnum(
			const QString& className, QString* errorMsg = 0) const;
		WmiClassObjectEnum createClassEnum(
			const QString& superClassName, ClassDepth depth = ClassDepthNormal, QString* errorMsg = 0) const;
		
		
		
};

#endif
