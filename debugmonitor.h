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

#ifndef DEBUGMONITOR_H
#define DEBUGMONITOR_H

#include <QtGui/QWidget>
#include <QStringList>
#include "ui_debugmonitor.h"

class DebugMonitor : public QWidget
{
    Q_OBJECT

public:
    DebugMonitor(QWidget *parent = 0);
    ~DebugMonitor();
public slots:
    void newAnswer( QStringList answer, bool ok );
    void button();
signals:
	void send( QString cmd );
private:
    Ui::DebugMonitorClass ui;
    QStringList list;
};

#endif // DEBUGMONITOR_H
