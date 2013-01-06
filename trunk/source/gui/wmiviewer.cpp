#include <QtCore>
#include <QtGui>

#include "../wmi/comscope.hpp"
#include "../wmi/wmilocator.hpp"

#include "window.hpp"

int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	app.setApplicationName("WMI Viewer");
	app.setApplicationVersion(PROJ_VERSION);
	app.setOrganizationName("WMI Viewer");
	app.setOrganizationDomain("wmi-viewer.googlecode.com");
	
	qDebug() << "WmiViewer";
	
	ComScope com;
	if (!com.initialise())
	{
		qWarning() << "Error initialising COM";
		QMessageBox::warning(0, app.applicationName(), QObject::tr("COM Initialisation Error"));
		return 1;
	}
	
	WmiLocator wmiLocator;
	if (!wmiLocator.initialise())
	{
		qWarning() << "Error initialising locator";
		QMessageBox::warning(0, app.applicationName(), QObject::tr("WMI Locator Initialisation Error"));
		return 1;
	}

	Window win(wmiLocator);
	win.show();
	
	return app.exec();
}


