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
