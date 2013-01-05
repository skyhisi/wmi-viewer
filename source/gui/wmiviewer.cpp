#include <QtCore>
#include <QtGui>

#include "../wmi/comscope.hpp"
#include "../wmi/wmilocator.hpp"
#include "../wmi/wmiservice.hpp"
#include "../wmi/wmiclassobjectenum.hpp"
#include "../wmi/wmiclassobject.hpp"
#include "../wmi/qtcominterop.hpp"

#include "window.hpp"

void test(WmiLocator& loc)
{
	WmiService svc = loc.connectServer("ROOT\\CIMV2");
	if (!svc.valid())
	{
		qWarning() << "INVALID SERVICE";
		return;
	}
	
	WmiClassObject obj = svc.getObject("Win32_Process");
	if (!obj.valid())
	{
		qWarning() << "INVALID OBJECT";
		return;
	}
	
	WmiQualifierSet qset = obj.classQualifierSet();
	if (!qset.valid())
	{
		qWarning() << "INVALID QSET";
		return;
	}
	
	QStringList qsetNames = qset.qualifierNames();
	qDebug() << "QUALIFIER NAMES:" << qsetNames;
	qDebug() << "QUALIFIERS:";
	foreach(QString name, qsetNames)
	{
		qDebug() << name << ":" << qset.getProperty(name);
	}
	
	QStringList propNames = obj.propertyNames();
	qDebug() << "PROPERTY NAMES:" << propNames;
	
	qDebug() << "CAPTION PROP SET";
	WmiQualifierSet propqset = obj.propertyQualifierSet("Caption");
	if (!propqset.valid())
	{
		qWarning() << "INVALID QSET";
		return;
	}
	
	QStringList propqsetNames = propqset.qualifierNames();
	qDebug() << propqsetNames;
	foreach(QString name, propqsetNames)
	{
		qDebug() << name << ":" << propqset.getProperty(name);
	}
	
	
}

int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	qDebug() << "WmiViewer";
	
	ComScope com;
	if (!com.initialise())
	{
		qWarning() << "Error initialising COM";
		return 1;
	}
	
	WmiLocator wmiLocator;
	if (!wmiLocator.initialise())
	{
		qWarning() << "Error initialising locator";
		return 1;
	}
	
	//test(wmiLocator);


	Window win(wmiLocator);
	win.show();
		
	app.exec();
	return 0;
}


