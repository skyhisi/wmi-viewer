#ifndef _WMIVIEWER_CLASSBROWSER_HPP_
#define _WMIVIEWER_CLASSBROWSER_HPP_

#include <QtGui>
#include "querymodel.hpp"
#include "classmodel.hpp"
#include "../wmi/wmilocator.hpp"
#include "../wmi/wmiservice.hpp"
#include "../wmi/wmiclassobjectenum.hpp"
#include "../wmi/wmiclassobject.hpp"

class ClassBrowser : public QWidget
{
	Q_OBJECT
	public:
		ClassBrowser(const WmiLocator& locator, QWidget* parent = 0);
		virtual ~ClassBrowser();
		
	signals:
		void query(const QString& ns, const QString& query);
		void switchTo();
		void status(const QString& message, int timeout);
		
	private slots:
		void loadClasses();
		void loadStarted();
		void classSelected(const QItemSelection& selected,
			const QItemSelection& deselected);
		void classLoaded();
		void buildQuery();
		
	private:
		const WmiLocator& _locator;
		
		QueryModel* _listModel;
		QSortFilterProxyModel* _listFilterModel;
		ClassModel* _classModel;
		QLineEdit* _namespaceLineEdit;
		QLineEdit* _classFilterLineEdit;
		QListView* _classListView;
		QLabel* _classNameLabel;
		QLabel* _classDescLabel;
		QTableView* _propertyTableView;
		
		
		Q_DISABLE_COPY(ClassBrowser)
};

#endif
