/*
 * ConsoleInput.h
 *
 *  Created on: Nov 8, 2010
 *      Author: peter
 */

#ifndef CONSOLEINPUT_H_
#define CONSOLEINPUT_H_

#include <QFile>
#include <QByteArray>
#include <QIODevice>
#include <QThread>

class ConsoleInput: public QThread {
	Q_OBJECT
public:
	ConsoleInput();
	virtual ~ConsoleInput();
	void run();
private:
	bool running;
signals:
	void gotCin( QByteArray array );
};

#endif /* CONSOLEINPUT_H_ */
