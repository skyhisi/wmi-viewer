#include "queryviewer.hpp"

#include "wqlhighlighter.hpp"

QueryViewer::QueryViewer(const WmiLocator& locator, QWidget* p):
	QWidget(p),
	_model(new QueryModel(locator, this)),
	_view(new QTableView()),
	_nsEdit(new QLineEdit()),
	_editor(new QTextEdit()),
	_nameSource(new QComboBox()),
	_notifyCheckBox(new QCheckBox(tr("Notification Query")))
{
	QGroupBox* queryGroupBox = new QGroupBox();
	QVBoxLayout* queryGroupBoxLayout = new QVBoxLayout();
	queryGroupBox->setLayout(queryGroupBoxLayout);
	
	queryGroupBoxLayout->addWidget(new QLabel(tr("Service Namespace")));
	queryGroupBoxLayout->addWidget(_nsEdit);
	queryGroupBoxLayout->addWidget(new QLabel(tr("WMI Query")));
	queryGroupBoxLayout->addWidget(_editor);
	
	QHBoxLayout* nameSourceLayout = new QHBoxLayout();
	nameSourceLayout->addWidget(new QLabel(tr("Name Source")));
	nameSourceLayout->addWidget(_nameSource);
	nameSourceLayout->addStretch();
	queryGroupBoxLayout->addLayout(nameSourceLayout);
	
	queryGroupBoxLayout->addWidget(_notifyCheckBox);
	
	
	QDialogButtonBox* buttons = new QDialogButtonBox();
	queryGroupBoxLayout->addWidget(buttons);
	
	QPushButton* execBtn = buttons->addButton(tr("Execute"), QDialogButtonBox::AcceptRole);
	connect(execBtn, SIGNAL(clicked(bool)), this, SLOT(execute()));
	
	QPushButton* cancelBtn = buttons->addButton(tr("Cancel"), QDialogButtonBox::ActionRole);
	cancelBtn->setDisabled(true);
	connect(cancelBtn, SIGNAL(clicked(bool)), _model, SLOT(cancel()));
	
	
	
	QVBoxLayout* mainLayout = new QVBoxLayout();
	setLayout(mainLayout);
	
	mainLayout->addWidget(queryGroupBox, 1);
	mainLayout->addWidget(_view, 5);
	
	_view->setModel(_model);
	_view->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
	
	_nsEdit->setText(_model->serviceNamespace());
	_editor->setPlainText(_model->query());
	_editor->setAcceptRichText(false);
	_editor->setTabChangesFocus(true);
	_editor->setUndoRedoEnabled(true);
	
	WqlHighlighter* highlighter = new WqlHighlighter(this);
	highlighter->setDocument(_editor->document());
	

	_nameSource->addItem(tr("All"), WmiClassObject::NameSourceAll);
	_nameSource->addItem(tr("Local"), WmiClassObject::NameSourceLocal);
	_nameSource->addItem(tr("Propagated"), WmiClassObject::NameSourcePropagated);
	_nameSource->addItem(tr("System"), WmiClassObject::NameSourceSystem);
	_nameSource->addItem(tr("Non-System"), WmiClassObject::NameSourceNonSystem);
	
	
	connect(
		_model, SIGNAL(loadStatus(const QString&,int)),
		this, SIGNAL(status(const QString&, int)));
	
	connect(
		_model, SIGNAL(loadingChanged(bool)),
		execBtn, SLOT(setDisabled(bool)));
	connect(
		_model, SIGNAL(loadingChanged(bool)),
		cancelBtn, SLOT(setEnabled(bool)));
	
}

QueryViewer::~QueryViewer()
{}

void QueryViewer::loadQuery(const QString& ns, const QString& query)
{
	if (!isVisible())
		emit switchTo();
		
	_nsEdit->setText(ns);
	_editor->setPlainText(query);
	_nameSource->setCurrentIndex(4);
	execute();
}

void QueryViewer::execute()
{
	_model->setServiceNamespace(_nsEdit->text().trimmed());
	_model->setQuery(_editor->toPlainText().trimmed());
	_model->setNameSource((WmiClassObject::NameSource)(_nameSource->itemData(_nameSource->currentIndex())).toInt());
	_model->setNotification(_notifyCheckBox->isChecked());
	if (!_model->execute())
	{
		QMessageBox::warning(this, tr("Query Error"), _model->lastError());
	}
}
