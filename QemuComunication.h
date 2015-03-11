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
