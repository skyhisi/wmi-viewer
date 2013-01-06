#include "querymodel.hpp"

#include <QtGui>

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
	//_columnNames(),
	_columnInfo(),
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
{
	cancel();
}

QString QueryModel::query() const { return _query; }
void QueryModel::setQuery(const QString& q) { _query = q; }

QString QueryModel::serviceNamespace() const { return _serviceNamespace; }
void QueryModel::setServiceNamespace(const QString& s) { _serviceNamespace = s; }

WmiClassObject::NameSource QueryModel::nameSource() const { return _nameSource; }
void QueryModel::setNameSource(WmiClassObject::NameSource n) { _nameSource = n; }

bool QueryModel::isNotification() const { return _notification; }
void QueryModel::setNotification(bool n) { _notification = n; }

bool QueryModel::execute(QString* errorMsg)
{
	if (errorMsg) errorMsg->clear();
	
	cancel();
	
	beginResetModel();
	//_columnNames.clear();
	_columnInfo.clear();
	_objects.clear();
	_more = true;
	_enum.reset(0);
	endResetModel();
	
	_service = _locator.connectServer(_serviceNamespace);
	if (!_service.valid())
	{
		qWarning() << "QueryModel::execute Invalid Service";
		if (errorMsg) *errorMsg = tr("Can not connect to server");
		return false;
	}
	
	QString queryErr;
	if (_notification)
		_enum = _service.execNotificationQuery(_query, &queryErr);
	else
		_enum = _service.execQuery(_query, &queryErr);
	if (!_enum.valid())
	{
		qWarning() << "QueryModel::execute Query Error:" << queryErr;
		if (errorMsg) *errorMsg = tr("Can not execute query: %1").arg(queryErr);
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
	{
		emit loadingChanged(_loading);
		
		if (! _notification)
		{
			if (_loading)
				QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
			else
				QApplication::restoreOverrideCursor();
		}
	}
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
	
	if (_columnInfo.isEmpty())
	{
		const WmiClassObject& obj = tempList.at(0);
		const QStringList colNames = obj.propertyNames(_nameSource);
		const QSet<QString> colNamesNS = obj.propertyNames(WmiClassObject::NameSourceNonSystem).toSet();
		
		beginInsertColumns(QModelIndex(), 0, colNames.count() - 1);
		foreach(QString name, colNames)
		{
			ColInfo info;
			info.name = name;
			info.canWrite = false;
			
			if (colNamesNS.contains(name))
			{
				const WmiQualifierSet propQS = obj.propertyQualifierSet(name);
				if (propQS.valid())
				{
					const QStringList propQNs = propQS.qualifierNames();
					qDebug() << name << propQNs;
					if (propQNs.contains("CIMTYPE"))
						info.type = propQS.getProperty("CIMTYPE").toString();
					if (propQNs.contains("write"))
						info.canWrite = propQS.getProperty("write").toBool();
					qDebug() << info.name << info.type << info.canWrite;
				}
			}
			_columnInfo.append(info);
		}
		endInsertColumns();
	}
	
	//qDebug() << "QueryModel::loadMore Rows " << rowCount() << "to" << rowCount() + tempList.count() - 1;
	
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
	
	return _columnInfo.count();
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
	
	Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	
	if (idx.column() < _columnInfo.size())
	{
		const ColInfo& info = _columnInfo.at(idx.column());
		if (info.canWrite && (info.type == "string" || info.type.contains("int") || info.type == "boolean"))
			flags |= Qt::ItemIsEditable;
			
		if (info.type == "boolean" && info.canWrite)
			flags |= Qt::ItemIsUserCheckable;
	}
	return flags;
}

QVariant QueryModel::data(const QModelIndex& idx, int role) const
{
	if (!idx.isValid() || idx.column() >= columnCount() || idx.row() >= rowCount())
		return QVariant();
	
	const ColInfo& colInfo = _columnInfo.at(idx.column());
	const WmiClassObject& obj = _objects.at(idx.row());
	
	if (role == Qt::DisplayRole)
	{
		const QVariant v = obj.getProperty(colInfo.name);
		if (v.type() == QVariant::Bool)
		{
			return v.toBool() ? tr("Yes") : tr("No");
		}
		return v;
	}
	else if (role == Qt::CheckStateRole && colInfo.canWrite)
	{
		const QVariant v = obj.getProperty(colInfo.name);
		if (v.type() == QVariant::Bool)
			return v.toBool() ? Qt::Checked : Qt::Unchecked;
	}
	return QVariant();
}

QVariant QueryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole)
	{
		if (orientation == Qt::Vertical)
			return section + 1;
		else
			if (section < _columnInfo.size())
				return _columnInfo.at(section).name;
	}
	else if (role == Qt::BackgroundRole && orientation == Qt::Horizontal && section < _columnInfo.size())
	{
		
		return _columnInfo.at(section).canWrite ?
			QBrush(QColor(Qt::red)) :
			QBrush();
	}
	
	return QVariant();
}


