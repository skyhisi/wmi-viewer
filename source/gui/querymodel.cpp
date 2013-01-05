#include "querymodel.hpp"

static const int QUERY_MODEL_BATCH_SIZE = 100;
static const int QUERY_MODEL_TIMEOUT = 0;

QueryModel::QueryModel(const WmiLocator& l, QObject* p) :
	QAbstractTableModel(p),
	_locator(l),
	_service(),
	_enum(),
	_query("SELECT * FROM meta_class"),
	_serviceNamespace("ROOT\\CIMV2"),
	_nameSource(WmiClassObject::NameSourceAll),
	_errorMsg(),
	_columnNames(),
	_objects()
{}

QueryModel::~QueryModel()
{}

QString QueryModel::query() const { return _query; }
void QueryModel::setQuery(const QString& q) { _query = q; }

QString QueryModel::serviceNamespace() const { return _serviceNamespace; }
void QueryModel::setServiceNamespace(const QString& s) { _serviceNamespace = s; }

WmiClassObject::NameSource QueryModel::nameSource() const { return _nameSource; }
void QueryModel::setNameSource(WmiClassObject::NameSource n) { _nameSource = n; }

QString QueryModel::lastError() const { return _errorMsg; }

bool QueryModel::execute()
{
	_errorMsg.clear();
	
	beginResetModel();
	
	_columnNames.clear();
	_objects.clear();
	_more = true;
	_enum.reset(0);
	
	_service = _locator.connectServer(_serviceNamespace);
	if (!_service.valid())
	{
		_errorMsg = "Can not connect to server";
		endResetModel();
		return false;
	}
	
	_enum = _service.execQuery(_query);
	if (!_enum.valid())
	{
		_errorMsg = "Can not execute query";
		endResetModel();
		return false;
	}
	
	WmiClassObject obj = _enum.next();
	if (obj.valid())
	{
		_columnNames = obj.propertyNames(_nameSource);
		_objects.append(obj);
		_more = _enum.nextBatch(QUERY_MODEL_BATCH_SIZE, QUERY_MODEL_TIMEOUT, _objects);
	}
	else
	{
		_more = false;
	}
	
	endResetModel();
	return true;
}

bool QueryModel::canFetchMore(const QModelIndex& p) const
{
	if (p.isValid() || !_enum.valid())
		return false;
	return _more;
}

void QueryModel::fetchMore(const QModelIndex& p)
{
	if (p.isValid() || !_enum.valid())
		return;
	
	QList<WmiClassObject> tempList;
	_more = _enum.nextBatch(QUERY_MODEL_BATCH_SIZE, QUERY_MODEL_TIMEOUT, tempList);
	if (tempList.empty())
		return;
	
	beginInsertRows(p, rowCount(), rowCount() + tempList.count() - 1);
	_objects.append(tempList);
	endInsertRows();
}

int QueryModel::columnCount(const QModelIndex& p) const
{
	if (p.isValid())
		return 0;
	
	return _columnNames.count();
}

int QueryModel::rowCount(const QModelIndex& p) const
{
	if (p.isValid())
		return 0;
	
	return _objects.count();
}

Qt::ItemFlags QueryModel::flags(const QModelIndex& idx) const
{
	if (!idx.isValid() || idx.column() >= columnCount() || idx.row() >= rowCount())
		return Qt::NoItemFlags;
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant QueryModel::data(const QModelIndex& idx, int role) const
{
	if (!idx.isValid() || idx.column() >= columnCount() || idx.row() >= rowCount())
		return QVariant();
	
	if (role == Qt::DisplayRole)
	{
		const QString& colName = _columnNames.at(idx.column());
		const WmiClassObject& obj = _objects.at(idx.row());
		return obj.getProperty(colName);
	}
	return QVariant();
}

QVariant QueryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();
	
	if (orientation == Qt::Vertical)
		return section + 1;
	
	return _columnNames.at(section);
}


