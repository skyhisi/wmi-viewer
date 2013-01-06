#ifndef _WMIVIEWER_QUERYMODEL_HPP_
#define _WMIVIEWER_QUERYMODEL_HPP_

#include "objectenummodel.hpp"

class QueryModel : public ObjectEnumModel
{
	Q_OBJECT
	public:
		QueryModel(const WmiLocator& locator, QObject* parent = 0);
		virtual ~QueryModel();
		
		QString query() const;
		void setQuery(const QString& query);
		
		bool isNotification() const;
		void setNotification(bool notify);
		
	protected:
		virtual WmiClassObjectEnum doExecute(WmiService service, QString* errorMsg);

	private:	
		QString _query;
		bool _notification;
		
		Q_DISABLE_COPY(QueryModel)
};

#endif
