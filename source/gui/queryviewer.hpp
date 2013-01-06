#ifndef _WMIVIEWER_QUERYVIEWER_HPP_
#define _WMIVIEWER_QUERYVIEWER_HPP_

#include <QtGui>
#include "querymodel.hpp"

class QueryViewer : public QWidget
{
	Q_OBJECT
	public:
		QueryViewer(const WmiLocator& locator, QWidget* parent = 0);
		virtual ~QueryViewer();
		
	public slots:
		void loadQuery(const QString& ns, const QString& query);
		
	signals:
		void switchTo();
		void status(const QString& message, int timeout);
		
	private slots:
		void execute();
		// void fetchAll();
	
	private:
		QueryModel* _model;
		QTableView* _view;
		QLineEdit* _nsEdit;
		QTextEdit* _editor;
		QComboBox* _nameSource;
		Q_DISABLE_COPY(QueryViewer)
};

#endif
