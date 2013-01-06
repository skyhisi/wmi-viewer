#include "querymodel.hpp"

static const int QUERY_MODEL_BATCH_SIZE = 100;
static const int NO_WAIT = 0;

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
	_objects(),
	_more(false),
	_loading(false),
	_notification(false),
	_semiSyncTimer(new QTimer(this))
{
	_semiSyncTimer->setSingleShot(true);
	connect(_semiSyncTimer, SIGNAL(timeout()), this, SLOT(loadMore()));
}

QueryModel::~QueryModel()
{}

QString QueryModel::query() const { return _query; }
void QueryModel::setQuery(const QString& q) { _query = q; }

QString QueryModel::serviceNamespace() const { return _serviceNamespace; }
void QueryModel::setServiceNamespace(const QString& s) { _serviceNamespace = s; }

WmiClassObject::NameSource QueryModel::nameSource() const { return _nameSource; }
void QueryModel::setNameSource(WmiClassObject::NameSource n) { _nameSource = n; }

bool QueryModel::isNotification() const { return _notification; }
void QueryModel::setNotification(bool n) { _notification = n; }

QString QueryModel::lastError() const { return _errorMsg; }

bool QueryModel::execute()
{
	_errorMsg.clear();
	
	cancel();
	
	beginResetModel();
	_columnNames.clear();
	_objects.clear();
	_more = true;
	_enum.reset(0);
	endResetModel();
	
	_service = _locator.connectServer(_serviceNamespace);
	if (!_service.valid())
	{
		_errorMsg = "Can not connect to server";
		return false;
	}
	
	_enum = _service.execQuery(_query, _notification);
	if (!_enum.valid())
	{
		_errorMsg = "Can not execute query";
		return false;
	}

	emit loadStatus(tr("Loading ..."), 0);
	loadMore();
	
	return true;
}

void QueryModel::cancel()
{
	_semiSyncTimer->stop();
	setLoading(false);
	emit loadStatus(tr("Load cancled"), 1000);
}

bool QueryModel::isLoading() const
{
	return _loading;
}

void QueryModel::setLoading(bool l)
{
	const bool prev = _loading;
	_loading = l;
	if (_loading != prev)
		emit loadingChanged(_loading);
}

void QueryModel::loadMore()
{
	if (!_enum.valid())
		return;
	
	setLoading(true);
	
	QList<WmiClassObject> tempList;
	_more = _enum.nextBatch(QUERY_MODEL_BATCH_SIZE, NO_WAIT, tempList);
	
	if (_more)
	{
		
		if (tempList.empty())
		{
			_semiSyncTimer->start(50);
		}
		else
		{
			_semiSyncTimer->start(0); // Callback once event loop clear
			emit loadStatus(tr("Loaded %n rows ...", 0, rowCount() + tempList.count()), 0);
		}
	}
	else
	{
		setLoading(false);
		emit loadStatus(tr("Load finished"), 1000);
	}
	
	if (tempList.empty())
		return;
	
	if (_objects.isEmpty())
	{
		const QStringList colNames = tempList.at(0).propertyNames(_nameSource);
		beginInsertColumns(QModelIndex(), 0, colNames.count() - 1);
		_columnNames = colNames;
		endInsertColumns();
	}
	
	qDebug() << "QueryModel::loadMore Rows " << rowCount() << "to" << rowCount() + tempList.count() - 1;
	
	beginInsertRows(QModelIndex(), rowCount(), rowCount() + tempList.count() - 1);
	_objects.append(tempList);
	endInsertRows();
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
	
	_semiSyncTimer->stop();
	loadMore();
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


