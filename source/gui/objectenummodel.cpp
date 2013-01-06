#include "objectenummodel.hpp"

#include <QtGui>

static const int QUERY_MODEL_BATCH_SIZE = 100;
static const int NO_WAIT = 0;

ObjectEnumModel::ObjectEnumModel(const WmiLocator& l, QObject* p) :
	QAbstractTableModel(p),
	_locator(l),
	_service(),
	_enum(),
	_serviceNamespace("ROOT\\CIMV2"),
	_nameSource(WmiClassObject::NameSourceAll),
	_columnNames(),
	_objects(),
	_more(false),
	_loading(false),
	_semiSyncTimer(new QTimer(this)),
	_useBusyCursor(true)
{
	_semiSyncTimer->setSingleShot(true);
	connect(_semiSyncTimer, SIGNAL(timeout()), this, SLOT(loadMore()));
}

ObjectEnumModel::~ObjectEnumModel()
{
	cancel();
}


QString ObjectEnumModel::serviceNamespace() const { return _serviceNamespace; }
void ObjectEnumModel::setServiceNamespace(const QString& s) { _serviceNamespace = s; }

WmiClassObject::NameSource ObjectEnumModel::nameSource() const { return _nameSource; }
void ObjectEnumModel::setNameSource(WmiClassObject::NameSource n) { _nameSource = n; }


bool ObjectEnumModel::execute(QString* errorMsg)
{
	if (errorMsg) errorMsg->clear();
		
	cancel();
	
	beginResetModel();
	_columnNames.clear();
	_objects.clear();
	_more = true;
	_enum.release();
	endResetModel();
	
	_service = _locator.connectServer(_serviceNamespace);
	if (!_service.valid())
	{
		qWarning() << "ObjectEnumModel Invalid Service";
		if(errorMsg) *errorMsg = tr("Internal Error");
		return false;
	}
	
	_enum = doExecute(_service, errorMsg);
	
	if (!_enum.valid())
	{
		qWarning() << "ObjectEnumModel::execute Invalid Enum";
		return false;
	}

	emit loadStatus(tr("Loading ..."), 0);
	loadMore();
	
	return true;
}

void ObjectEnumModel::cancel()
{
	_semiSyncTimer->stop();
	setLoading(false);
	emit loadStatus(tr("Load cancled"), 1000);
}

bool ObjectEnumModel::isLoading() const
{
	return _loading;
}

void ObjectEnumModel::setLoading(bool l)
{
	const bool prev = _loading;
	_loading = l;
	if (_loading != prev)
	{
		emit loadingChanged(_loading);
		
		if (_useBusyCursor)
		{
			if (_loading)
				QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
			else
				QApplication::restoreOverrideCursor();
		}
	}
}

void ObjectEnumModel::setUseBusyCursor(bool use)
{
	if (isLoading())
		qWarning() << "Can not change UseBusyCursor while loading";
	else
		_useBusyCursor = use;
}

WmiClassObject ObjectEnumModel::objectAt(int row) const
{
	if (0 <= row && row < _objects.size())
		return _objects.at(row);
	qWarning() << "ObjectEnumModel::objectAt Invalid row" << row;
	return WmiClassObject();
}

void ObjectEnumModel::loadMore()
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
	
	if (_columnNames.isEmpty())
	{
		const WmiClassObject& obj = tempList.at(0);
		const QStringList colNames = obj.propertyNames(_nameSource);
		
		beginInsertColumns(QModelIndex(), 0, colNames.count() - 1);
		_columnNames = colNames;
		endInsertColumns();
	}
	
	//qDebug() << "QueryModel::loadMore Rows " << rowCount() << "to" << rowCount() + tempList.count() - 1;
	
	beginInsertRows(QModelIndex(), rowCount(), rowCount() + tempList.count() - 1);
	_objects.append(tempList);
	endInsertRows();
}

bool ObjectEnumModel::canFetchMore(const QModelIndex& p) const
{
	if (p.isValid() || !_enum.valid())
		return false;
	return _more;
}

void ObjectEnumModel::fetchMore(const QModelIndex& p)
{
	if (p.isValid() || !_enum.valid())
		return;
	
	_semiSyncTimer->stop();
	loadMore();
}

int ObjectEnumModel::columnCount(const QModelIndex& p) const
{
	if (p.isValid())
		return 0;
	
	return _columnNames.count();
}

int ObjectEnumModel::rowCount(const QModelIndex& p) const
{
	if (p.isValid())
		return 0;
	
	return _objects.count();
}

Qt::ItemFlags ObjectEnumModel::flags(const QModelIndex& idx) const
{
	if (!idx.isValid() || idx.column() >= columnCount() || idx.row() >= rowCount())
		return Qt::NoItemFlags;

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable; /* | Qt::ItemIsEditable; */
}

QVariant ObjectEnumModel::data(const QModelIndex& idx, int role) const
{
	if (!idx.isValid() || idx.column() >= columnCount() || idx.row() >= rowCount())
		return QVariant();
	
	const QString& colName = _columnNames.at(idx.column());
	const WmiClassObject& obj = _objects.at(idx.row());
	
	if (role == Qt::DisplayRole)
	{
		const QVariant v = obj.getProperty(colName);
		if (v.type() == QVariant::Bool)
		{
			return v.toBool() ? tr("Yes") : tr("No");
		}
		return v;
	}
	else if (role == Qt::EditRole)
	{
		return obj.getProperty(colName);
	}
	else if (role == Qt::CheckStateRole)
	{
		const QVariant v = obj.getProperty(colName);
		if (v.type() == QVariant::Bool)
			return v.toBool() ? Qt::Checked : Qt::Unchecked;
	}
	else if (role == Qt::TextAlignmentRole)
	{
		const QVariant v = obj.getProperty(colName);
		//qDebug() << v.typeName() << v.type() << v.userType();
		// Align numbers to right
		if ((2 <= v.userType() && v.userType() <= 6) || (129 <= v.userType() && v.userType() <= 135))
			return QVariant(Qt::AlignRight | Qt::AlignTop);
		else
			return QVariant(Qt::AlignLeft | Qt::AlignTop);
	}
	return QVariant();
}

QVariant ObjectEnumModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole)
	{
		if (orientation == Qt::Vertical)
			return section + 1;
		else
			if (section < _columnNames.size())
				return _columnNames.at(section);
	}
	return QVariant();
}

bool ObjectEnumModel::setData(const QModelIndex& idx, const QVariant& value, int role)
{
	if (
		!idx.isValid() ||
		idx.parent().isValid() ||
		role != Qt::EditRole ||
		idx.column() >= columnCount() ||
		idx.row() >= rowCount()
	)
		return false;
	
	const QString& colName = _columnNames.at(idx.column());
	WmiClassObject& obj = _objects[idx.row()];
	const bool ok = obj.setProperty(colName, value);
	if (ok)
		emit dataChanged(idx, idx);
	return ok;
}


