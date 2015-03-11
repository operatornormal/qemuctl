/*
    Copyright (C) 2010  Peter Rustler

    This file is part of qemuctl.

    qemuctl is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    qemuctl is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with qemuctl.  If not, see <http://www.gnu.org/licenses/>.
*/

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

