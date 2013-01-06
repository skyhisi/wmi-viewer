#include "classbrowser.hpp"

ClassBrowser::ClassBrowser(const WmiLocator& l, QWidget* p) :
	QWidget(p),
	_locator(l),
	_listModel(new QueryModel(_locator, this)),
	_listFilterModel(new QSortFilterProxyModel(this)),
	_classModel(new ClassModel(_locator, this)),
	_namespaceLineEdit(new QLineEdit(tr("ROOT\\CIMV2"))),
	_classFilterLineEdit(new QLineEdit()),
	_classListView(new QListView()),
	_classNameLabel(new QLabel()),
	_classDescLabel(new QLabel()),
	_propertyTableView(new QTableView())
{
	_listFilterModel->setSourceModel(_listModel);
	_listFilterModel->setFilterKeyColumn(1);
	_listFilterModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
	
	
	_classListView->setModel(_listFilterModel);
	_classListView->setModelColumn(1);
	_classListView->setSelectionMode(QAbstractItemView::SingleSelection);
	
	_propertyTableView->setModel(_classModel);
	_propertyTableView->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
	_propertyTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
	_propertyTableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
	
	
	_classNameLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
	_classDescLabel->setWordWrap(true);
	
	QPushButton* buildQueryButton = new QPushButton(tr("Build Query"));
	
	QGroupBox* namespaceGroup = new QGroupBox(tr("Namespace"));
	QVBoxLayout* namespaceLayout = new QVBoxLayout();
	namespaceGroup->setLayout(namespaceLayout);
	namespaceLayout->addWidget(_namespaceLineEdit);
	QPushButton* namespaceLoadButton = new QPushButton(tr("Load Classes"));
	namespaceLayout->addWidget(namespaceLoadButton);
	
	QGroupBox* classListGroup = new QGroupBox(tr("Class List"));
	QVBoxLayout* classListLayout = new QVBoxLayout();
	classListGroup->setLayout(classListLayout);
	classListLayout->addWidget(new QLabel(tr("Filter")));
	classListLayout->addWidget(_classFilterLineEdit);
	classListLayout->addWidget(_classListView);
	
	
	QWidget* leftWidget = new QWidget();
	QVBoxLayout* leftLayout = new QVBoxLayout();
	leftWidget->setLayout(leftLayout);
	leftLayout->addWidget(namespaceGroup, 0);
	leftLayout->addWidget(classListGroup, 1);
	
	QGroupBox* propTableGroup = new QGroupBox(tr("Class Properties"));
	propTableGroup->setLayout(new QVBoxLayout());
	propTableGroup->layout()->addWidget(_propertyTableView);
	
	QWidget* rightWidget = new QWidget();
	QVBoxLayout* rightLayout = new QVBoxLayout();
	rightWidget->setLayout(rightLayout);
	rightLayout->addWidget(_classNameLabel, 0);
	rightLayout->addWidget(_classDescLabel, 0);
	rightLayout->addWidget(propTableGroup, 1);
	rightLayout->addWidget(buildQueryButton, 0);

	
	QSplitter* splitter = new QSplitter();
	splitter->addWidget(leftWidget);
	splitter->addWidget(rightWidget);
	splitter->setStretchFactor(0, 1);
	splitter->setStretchFactor(1, 3);
	
	
	QVBoxLayout* mainLayout = new QVBoxLayout();
	mainLayout->setContentsMargins(0, 0, 0, 0);
	setLayout(mainLayout);
	mainLayout->addWidget(splitter);
	
	
	connect(
		namespaceLoadButton, SIGNAL(clicked()),
		this, SLOT(loadClasses()));
	
	connect(
		_listModel, SIGNAL(modelReset()),
		this, SLOT(loadMoreClasses()));
	connect(
		_listModel, SIGNAL(modelReset()),
		_classModel, SLOT(clear()));
	connect(
		_listModel, SIGNAL(rowsInserted(const QModelIndex&,int,int)),
		this, SLOT(loadMoreClasses()));
	
	connect(
		_classFilterLineEdit, SIGNAL(textChanged(const QString&)),
		_listFilterModel, SLOT(setFilterWildcard(const QString&)));
	
	connect(
		_classListView->selectionModel(),
		SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
		this,
		SLOT(classSelected(const QItemSelection&, const QItemSelection&)));
	
	connect(_classModel, SIGNAL(modelReset()), this, SLOT(classLoaded()));
	
	connect(buildQueryButton, SIGNAL(clicked()), this, SLOT(buildQuery()));
}

ClassBrowser::~ClassBrowser()
{}

void ClassBrowser::loadClasses()
{
	_classFilterLineEdit->clear();
	_listModel->setServiceNamespace(_namespaceLineEdit->text());
	_listModel->setQuery("SELECT * FROM meta_class");
	if (!_listModel->execute())
	{
		QMessageBox::warning(this, tr("Load Classes"), tr("Invalid namespace"));
		return;
	}
	_classListView->setModelColumn(1);
}

void ClassBrowser::loadMoreClasses()
{
	while (_listModel->canFetchMore())
		_listModel->fetchMore();
}

void ClassBrowser::classSelected(const QItemSelection& selected,
	const QItemSelection& deselected)
{
	const QModelIndexList selectedList = selected.indexes();
	qDebug() << "SELECTED:" << selectedList.size();
	if (selectedList.isEmpty())
	{
		_classModel->clear();
		return;
	}
	
	const QString className = selectedList.at(0).data().toString();
		
	_classModel->setServiceNamespace(_listModel->serviceNamespace());
	_classModel->setClassName(className);
	
	QString err;
	if (!_classModel->load(&err))
	{
		_classModel->clear();
		QMessageBox::warning(this, tr("Load Class"), err);
		return;
	}
	
}

void ClassBrowser::classLoaded()
{
	_classNameLabel->setText(QString("<b>%1</b>").arg(_classModel->className()));
	_classDescLabel->setText(_classModel->description());
}

void ClassBrowser::buildQuery()
{
	QStringList params;
	foreach(QModelIndex idx, _propertyTableView->selectionModel()->selectedRows())
	{
		params << _classModel->headerData(idx.row(), Qt::Vertical).toString();
	}
	const QString paramStr = params.isEmpty() ? QString("*") : params.join(", ");

	const QString q = QString("SELECT %1 FROM %2").arg(
		paramStr, _classModel->className());
	
	emit query(_classModel->serviceNamespace(), q);
}


