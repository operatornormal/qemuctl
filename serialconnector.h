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

#ifndef SERIALCONNECTOR_H
#define SERIALCONNECTOR_H

#include <QWidget>
#include <QFileDialog>
#include <QNetworkInterface>
#include <QList>
#include <QNetworkAddressEntry>
#include <QDir>
#include <QStringList>
#include <QFileInfoList>
#include <QHostAddress>
#include <QAbstractSocket>
#include <QVBoxLayout>
#include "ui_serialconnector.h"

class SerialConnector : public QWidget
{
    Q_OBJECT

public:
    SerialConnector(QWidget *parent = 0);
    ~SerialConnector();
    const QString getCommand();
    QVBoxLayout * getMainLayout();
public slots:
	void typeChanged( int type );
	void tcpServerCheckBox( int state );
	void done();
	void cancel();
	void fileChoose();
	void pipeChoose();
	void unixChoose();
signals:
	void newCommand( const QString & cmd );
private:
    Ui::SerialConnectorClass ui;
    QString command;
    void fillHostAdresses();
    void fillParport();
    void fillTTY();
};

#endif // SERIALCONNECTOR_H
