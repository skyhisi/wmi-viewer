#ifndef _WMIVIEWER_OBJECTENUMMODEL_HPP_
#define _WMIVIEWER_OBJECTENUMMODEL_HPP_

#include <QtCore>

#include "../wmi/wmilocator.hpp"
#include "../wmi/wmiservice.hpp"
#include "../wmi/wmiclassobjectenum.hpp"
#include "../wmi/wmiclassobject.hpp"


class ObjectEnumModel : public QAbstractTableModel
{
	Q_OBJECT
	public:
		ObjectEnumModel(const WmiLocator& locator, QObject* parent = 0);
		virtual ~ObjectEnumModel();
		
		QString serviceNamespace() const;
		void setServiceNamespace(const QString& serviceNamespace);
		
		WmiClassObject::NameSource nameSource() const;
		void setNameSource(WmiClassObject::NameSource nameSource);
		
		bool execute(QString* errorMsg = 0);
		
		bool isLoading() const;
		
		virtual bool canFetchMore(const QModelIndex& parent = QModelIndex()) const;
		virtual void fetchMore(const QModelIndex& parent = QModelIndex());
		
		virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
		virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
		virtual Qt::ItemFlags flags(const QModelIndex& index) const;
		virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
		virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
		virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
		
	public slots:
		void cancel();
		
	signals:
		void loadingChanged(bool loading);
		void loadStatus(const QString& message, int timeout);
		
	protected:
		virtual WmiClassObjectEnum doExecute(WmiService service, QString* errorMsg) = 0;
		void setUseBusyCursor(bool use);
		
		WmiClassObject objectAt(int row) const;
		
	private slots:
		void loadMore();
		
	private:
		void setLoading(bool loading);
	
		const WmiLocator& _locator;
		WmiService _service;
		WmiClassObjectEnum _enum;
		QString _serviceNamespace;
		WmiClassObject::NameSource _nameSource;
		
		QStringList _columnNames;
		QList<WmiClassObject> _objects;
		
		bool _more;
		bool _loading;
		bool _useBusyCursor;
		QTimer* _semiSyncTimer;
		
		Q_DISABLE_COPY(ObjectEnumModel)
};

#endif
