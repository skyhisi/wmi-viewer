#ifndef _WMIVIEWER_CLASSMODEL_HPP_
#define _WMIVIEWER_CLASSMODEL_HPP_

#include <QtCore>

#include "../wmi/wmilocator.hpp"
#include "../wmi/wmiservice.hpp"
#include "../wmi/wmiclassobjectenum.hpp"
#include "../wmi/wmiclassobject.hpp"

class ClassModel : public QAbstractTableModel
{
	Q_OBJECT
	public:
		ClassModel(const WmiLocator& locator, QObject* parent = 0);
		virtual ~ClassModel();
		
		QString className() const;
		void setClassName(const QString& name);
		
		QString serviceNamespace() const;
		void setServiceNamespace(const QString& serviceNamespace);
		
		bool load(QString* error = 0);
		
		QString description() const;
		QString displayName() const;
		
		virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
		virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
		//virtual Qt::ItemFlags flags(const QModelIndex& index) const;
		virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
		virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
		
	public slots:
		void clear();
		
	private:
		const WmiLocator& _locator;
		WmiService _service;
		WmiClassObject _classObject;
		
		QString _name;
		QString _serviceNamespace;
		
		QString _description;
		QString _displayName;
		
		QStringList _propValueNames;
		
		struct PropertyInfo
		{
			QString name;
			QVariantList values;
			//QString type;
			//QString description;
		};
		
		QList<PropertyInfo> _propList;
};

#endif
