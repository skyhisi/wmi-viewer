#ifndef _WMIVIEWER_COMSCOPE_HPP_
#define _WMIVIEWER_COMSCOPE_HPP_

#include "wmibase.hpp"

class EXPORT ComScope
{
	public:
		ComScope();
		~ComScope() throw();
		
		bool initialise();
		
	private:
		bool _initialised;
		ComScope(const ComScope&);
		ComScope& operator=(const ComScope&);
};

#endif
