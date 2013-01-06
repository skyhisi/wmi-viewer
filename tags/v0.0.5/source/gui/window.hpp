#ifndef _WMIVIEWER_WINDOW_HPP_
#define _WMIVIEWER_WINDOW_HPP_

#include <QtGui>
#include "../wmi/wmilocator.hpp"

class Window : public QMainWindow
{
	Q_OBJECT
	public:
		Window(const WmiLocator& locator, QWidget* parent = 0);
		virtual ~Window();
	
	private:
		Q_DISABLE_COPY(Window)
};

#endif
