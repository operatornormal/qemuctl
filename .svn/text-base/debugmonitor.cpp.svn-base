#include "debugmonitor.h"

DebugMonitor::DebugMonitor(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);
	ui.plainTextEdit->setReadOnly(true);
	ui.plainTextEdit->setLineWrapMode(QPlainTextEdit::NoWrap);
	connect( ui.pushButton, SIGNAL(clicked()), this, SLOT(button()) );
	connect( ui.lineEdit, SIGNAL(returnPressed()), this, SLOT(button()) );
}

DebugMonitor::~DebugMonitor()
{

}

void DebugMonitor::button(){
	QString ret = ui.lineEdit->text();
	ui.lineEdit->clear();
	emit send( ret );
}

void DebugMonitor::newAnswer( QStringList answer, bool ok )
{
	this->list << answer;
	ui.plainTextEdit->appendPlainText(answer.join("\n"));
}

