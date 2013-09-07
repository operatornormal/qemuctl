/*
 * ConsoleInput.cpp
 *
 *  Created on: Nov 8, 2010
 *      Author: peter
 */

#include "ConsoleInput.h"

ConsoleInput::ConsoleInput() {
	running = true;
}

ConsoleInput::~ConsoleInput() {
	running = false;
	this->wait();
}

void ConsoleInput::run(){
	running = true;
	qDebug("ConsoleInput::run() start");
	QFile in;
	in.open( stdin, ( QIODevice::ReadOnly | QIODevice::Unbuffered ) );
	QByteArray input;
	int len;
	char * charinput = new char[1000];
	while( running == true ){

		if( ( len = in.read(charinput,999) ) > 0 ){
			charinput[len] = 0;
			input = charinput;
			emit gotCin( input );
		}
		usleep(100);
	}
	in.close();
	qDebug("ConsoleInput::run() stop");
}
