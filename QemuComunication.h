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

/*
 * QemuComunication.h
 *
 *  Created on: Nov 5, 2010
 *      Author: peter
 */

#ifndef QEMUCOMUNICATION_H_
#define QEMUCOMUNICATION_H_

#include <QThread>
#include <QMutex>
#include <QString>
#include <QTcpSocket>
#include <QStringList>
#include <QMutex>
#include <QWaitCondition>
#include <QTextStream>

class QemuComunication: public QThread {
	Q_OBJECT
public:
	QemuComunication();
	virtual ~QemuComunication();
	void run();
	void setServer( QString host, quint16 port );
public slots:
	void send( QString comand );
	void readSocket();
protected:
	QString host;
	quint16 port;
	bool stop;
	int timeout;
    QTcpSocket * socket;
    QMutex mutex;
    QMutex command;
    QTextStream * in;
    QByteArray * array;
signals:
    void error(int socketError, const QString &message);
    void newAnswer( QStringList answer, bool ok );
};

#endif /* QEMUCOMUNICATION_H_ */
