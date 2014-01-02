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
 * QHexSpinbox.cpp
 *
 *  Created on: Dec 14, 2010
 *      Author: peter
 */

#include "QHexSpinbox.h"

QHexSpinbox::QHexSpinbox( QWidget * parent )
: QSpinBox(parent)
{
	this->setMaximum(0xffff);
}

QHexSpinbox::~QHexSpinbox() {
}

int QHexSpinbox::valueFromText( const QString & text ) const{
	bool ok;
	return text.toInt(&ok, 16);
}

QString QHexSpinbox::textFromValue( int value ) const{
	return QString().sprintf( "%4.4x",value );
}
