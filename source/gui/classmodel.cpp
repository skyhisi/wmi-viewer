#include "classmodel.hpp"

ClassModel::ClassModel(const WmiLocator& loc, QObject* par) :
	_locator(loc),
	_service(),
	_classObject(),
	_name(),
	_serviceNamespace(),
	_description(),
	_displayName(),
	_propValueNames(),
	_propList()
{}

ClassModel::~ClassModel()
{}

QString ClassModel::className() const
{ return _name; }

void ClassModel::setClassName(const QString& n)
{ _name = n; }

QString ClassModel::serviceNamespace() const
{ return _serviceNamespace; }
void ClassModel::setServiceNamespace(const QString& sn)
{ _serviceNamespace = sn; }

bool ClassModel::load(QString* error)
{
	if (error) error->clear();
	
	beginResetModel();
	_description.clear();
	_displayName.clear();
	_propValueNames.clear();
	_propList.clear();
	
	_service = _locator.connectServer(_serviceNamespace);
	if (!_service.valid())
	{
		if (error) *error = "Can not connect to server";
		endResetModel();
		return false;
	}
	
	_classObject = _service.getObject(_name);
	if (!_classObject.valid())
	{
		if (error) *error = "Can not load object";
		endResetModel();
		return false;
	}
	
	WmiQualifierSet classQS = _classObject.classQualifierSet();
	Q_ASSERT(classQS.valid());
	
	const QStringList avaliableClsAttrs = classQS.qualifierNames();
	qDebug() << avaliableClsAttrs;
	
	if (avaliableClsAttrs.contains("Description"))
		_description = classQS.getProperty("Description").toString();
	
	if (avaliableClsAttrs.contains("DisplayName"))
		_displayName = classQS.getProperty("DisplayName").toString();
	
	QStringList propNames = _classObject.propertyNames(WmiClassObject::NameSourceNonSystem);
	foreach(QString propName, propNames)
	{
		qDebug() << propName;
		WmiQualifierSet propQS = _classObject.propertyQualifierSet(propName);
		Q_ASSERT(propQS.valid());
		
		PropertyInfo info;
		info.name = propName;
		
		const QStringList avaliablePropValueNames = propQS.qualifierNames();
		qDebug() << avaliablePropValueNames;
		
		foreach(QString propValueName, avaliablePropValueNames)
		{
			int propValueIndex = _propValueNames.indexOf(propValueName);
			if (propValueIndex == -1)
			{
				_propValueNames.append(propValueName);
				propValueIndex = _propValueNames.size() - 1;
			}
			while (info.values.size() < _propValueNames.size())
				info.values.append(QVariant());
			
			info.values[propValueIndex] = propQS.getProperty(propValueName);
		}
		
		_propList.append(info);
	}
	
	endResetModel();
	
	return true;
}

void ClassModel::clear()
{
	beginResetModel();
	_classObject.release();
	_service.release();
	_name.clear();
	_description.clear();
	_displayName.clear();
	_propValueNames.clear();
	_propList.clear();
	endResetModel();
}

QString ClassModel::description() const
{ return _description; }
QString ClassModel::displayName() const
{ return _displayName; }

int ClassModel::columnCount(const QModelIndex& par) const
{
	return par.isValid() ? 0 : _propValueNames.size();
}

int ClassModel::rowCount(const QModelIndex& par) const
{
	return par.isValid() ? 0 : _propList.size();
}

//Qt::ItemFlags ClassModel::flags(const QModelIndex& index) const;

QVariant ClassModel::data(const QModelIndex& idx, int role) const
{
	if (!idx.isValid() || idx.parent().isValid())
		return QVariant();
	
	if (idx.row() < 0 || idx.row() >= _propList.size())
		return QVariant();
	
	if (role == Qt::DisplayRole)
	{
		const PropertyInfo& info = _propList.at(idx.row());
		if (idx.column() < info.values.size())
			return info.values.at(idx.column());
	}
	return QVariant();
}

QVariant ClassModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole)
	{
		if (orientation == Qt::Vertical)
		{
			if (section < _propList.size())
				return _propList.at(section).name;
		}
		else
		{
			if (section < _propValueNames.size())
				return _propValueNames.at(section);
		}
	}
	return QVariant();
}




