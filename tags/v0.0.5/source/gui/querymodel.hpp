#ifndef _WMIVIEWER_QUERYMODEL_HPP_
#define _WMIVIEWER_QUERYMODEL_HPP_

#include <QtCore>

#include "../wmi/wmilocator.hpp"
#include "../wmi/wmiservice.hpp"
#include "../wmi/wmiclassobjectenum.hpp"
#include "../wmi/wmiclassobject.hpp"


class QueryModel : public QAbstractTableModel
{
	Q_OBJECT
	public:
		QueryModel(const WmiLocator& locator, QObject* parent = 0);
		virtual ~QueryModel();
		
		QString query() const;
		void setQuery(const QString& query);
		
		QString serviceNamespace() const;
		void setServiceNamespace(const QString& serviceNamespace);
		
		WmiClassObject::NameSource nameSource() const;
		void setNameSource(WmiClassObject::NameSource nameSource);
		
		QString lastError() const;
		
		bool execute();
		
		virtual bool canFetchMore(const QModelIndex& parent = QModelIndex()) const;
		virtual void fetchMore(const QModelIndex& parent = QModelIndex());
		
		virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
		virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
		virtual Qt::ItemFlags flags(const QModelIndex& index) const;
		virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
		virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
		
		
	private:
		const WmiLocator& _locator;
		WmiService _service;
		WmiClassObjectEnum _enum;
		QString _query;
		QString _serviceNamespace;
		WmiClassObject::NameSource _nameSource;
		QString _errorMsg;
		QStringList _columnNames;
		QList<WmiClassObject> _objects;
		bool _more;
		
		Q_DISABLE_COPY(QueryModel)
};

#endif
