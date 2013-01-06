#ifndef _WMIVIEWER_WMIQUALIFIERSET_HPP_
#define _WMIVIEWER_WMIQUALIFIERSET_HPP_

#include "wmibase.hpp"
#include <QtCore>

class WmiClassObject;

class EXPORT WmiQualifierSet : public ComPointer<IWbemQualifierSet>
{
	public:
		WmiQualifierSet(IWbemQualifierSet* qualset = 0, bool take = 0);
	
		enum NameSource {
			NameSourceAll,
			NameSourceLocal,
			NameSourcePropagated
		};
	
		QStringList qualifierNames(NameSource source = NameSourceAll) const;
		QVariant getProperty(const QString& name) const;
};

#endif
