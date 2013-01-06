#include "querymodel.hpp"

#include <QtGui>

static const int QUERY_MODEL_BATCH_SIZE = 100;
static const int NO_WAIT = 0;

QueryModel::QueryModel(const WmiLocator& l, QObject* p) :
	ObjectEnumModel(l, p),
	_query("SELECT * FROM meta_class"),
	_notification(false)
{}

QueryModel::~QueryModel()
{}

QString QueryModel::query() const { return _query; }
void QueryModel::setQuery(const QString& q) { _query = q; }

bool QueryModel::isNotification() const { return _notification; }
void QueryModel::setNotification(bool n) { _notification = n; }

WmiClassObjectEnum QueryModel::doExecute(WmiService service, QString* errorMsg)
{
	setUseBusyCursor(!_notification);

	if (_notification)
		return service.execNotificationQuery(_query, errorMsg);
	else
		return service.execQuery(_query, errorMsg);
}

