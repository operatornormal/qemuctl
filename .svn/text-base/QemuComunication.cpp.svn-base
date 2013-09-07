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
