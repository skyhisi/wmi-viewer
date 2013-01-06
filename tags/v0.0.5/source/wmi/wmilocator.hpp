#ifndef _WMIVIEWER_WMILOCATOR_HPP_
#define _WMIVIEWER_WMILOCATOR_HPP_

#include <QtCore>
#include "wmibase.hpp"
#include "wmiservice.hpp"

class EXPORT WmiLocator : public ComPointer<IWbemLocator>
{
	public:
		WmiLocator(IWbemLocator* locator = 0, bool take = true);

		bool initialise();
		
		WmiService connectServer(const QString& serviceNamespace) const;
};

#endif
