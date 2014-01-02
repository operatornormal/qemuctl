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
 * QemuComunication.cpp
 *
 *  Created on: Nov 5, 2010
 *      Author: peter
 */

#include "QemuComunication.h"

QemuComunication::QemuComunication(){
	stop = false;
	timeout = 5000;
	socket = NULL;
}

QemuComunication::~QemuComunication() {
	this->quit();
}

void QemuComunication::setServer( QString host, quint16 port ){
    mutex.lock();
	this->host = host;
	this->port = port;
    mutex.unlock();
}

void QemuComunication::send( QString comand ){
	//this->command.lock();
	qDebug("QemuComunication::send: %s", comand.toAscii().data() );
	if( socket->isWritable() && this->isRunning() ){
		this->command.lock();
		comand.append( '\n' );
		const QByteArray array = comand.toAscii();
		socket->write(array);
	}

}

void QemuComunication::readSocket(){
	int len;
	int pos;
	int timeout = 100;
	char * input = new char[10000];
	QByteArray array;
	while( ( pos = array.indexOf("\n(qemu)") ) < 0 && timeout > 0 ){
		if( ( len = socket->read(input,9999) ) > 0 ){
			input[len] = 0;
			array.append(input);
		}
		usleep(10);
		timeout--;
	}
	QStringList ret = QString(array).split( "\r\n", QString::KeepEmptyParts );
	int parse;
	for( int i = 0; i < ret.size(); i++ ){
		parse = ret[i].lastIndexOf("\033[D");
		if( parse >= 0 ){
			ret[i].remove(0,parse+3);
		}
		parse = ret[i].indexOf("\033[K");
		if( parse >= 0 ){
			ret[i].remove(parse,3);
		}
	}
	if( pos > 0 ){
		if( ret.size() > 1 ){
			qDebug( "found %d %d", pos, ret.size() );
			ret.pop_back();
			emit newAnswer( ret, true );
			this->command.unlock();
		}
	} else {
		if( ret.size() > 0 ){
			if( ! ( ret.size() == 1 && ret[0].isEmpty() == true ) ){
				qDebug( "found %d %d", pos, ret.size() );
				emit newAnswer( ret, false );
				this->command.unlock();
			}
		}
	}
}

void QemuComunication::run(){
	sleep( 1 );

	this->array = new QByteArray();

	this->socket = new QTcpSocket();
	connect( socket, SIGNAL(readyRead()), this, SLOT(readSocket()) );

    mutex.lock();
    QString serverName = host;
    quint16 serverPort = port;
    mutex.unlock();

    socket->connectToHost( serverName, serverPort );

    if (!socket->waitForConnected(timeout)) {
        emit error(socket->error(), socket->errorString());
        qDebug( "QemuComunication: %s", socket->errorString().toAscii().data() );
        return;
    }
    exec();
}
