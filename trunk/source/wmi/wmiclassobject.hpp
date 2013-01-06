#ifndef _WMIVIEWER_WMICLASSOBJECT_HPP_
#define _WMIVIEWER_WMICLASSOBJECT_HPP_

#include "wmibase.hpp"
#include <QtCore>
#include "wmiqualifierset.hpp"

class EXPORT WmiClassObject : public WmiCloneable<IWbemClassObject>, public WmiComparable<IWbemClassObject>
{
	public:
		WmiClassObject(IWbemClassObject* object = 0, bool take = true);
		
		enum NameSource {
			NameSourceAll,
			NameSourceLocal,
			NameSourcePropagated,
			NameSourceSystem,
			NameSourceNonSystem
		};
		
		QStringList propertyNames(NameSource source = NameSourceAll) const;
		
		QVariant getProperty(const QString& name) const;
		bool setProperty(const QString& name, const QVariant& variant);
		
		
		WmiQualifierSet classQualifierSet() const;
		WmiQualifierSet propertyQualifierSet(const QString& propName) const;
};


#endif
