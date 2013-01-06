#include "window.hpp"

#include "queryviewer.hpp"
#include "classbrowser.hpp"

Window::Window(const WmiLocator& locator, QWidget* p) :
	QMainWindow(p)
{
	QueryViewer* viewer = new QueryViewer(locator, this);

	ClassBrowser* classBrowser = new ClassBrowser(locator, this);


	QTabWidget* tabs = new QTabWidget();
	setCentralWidget(tabs);
	
	tabs->addTab(viewer, tr("Query"));
	tabs->addTab(classBrowser, tr("Class Browser"));
	
	connect(
		classBrowser, SIGNAL(query(const QString&, const QString&)),
		viewer, SLOT(loadQuery(const QString&, const QString&)));
	
	QSignalMapper* switchTabMapper = new QSignalMapper(this);
	connect(switchTabMapper, SIGNAL(mapped(QWidget*)), tabs, SLOT(setCurrentWidget(QWidget*)));
	for (int i = 0; i < tabs->count(); ++i)
	{
		QWidget* w = tabs->widget(i);
		switchTabMapper->setMapping(w, w);
		connect(w, SIGNAL(switchTo()), switchTabMapper, SLOT(map()));
	}
	
	
	resize(700, 500);
}

Window::~Window()
{}
