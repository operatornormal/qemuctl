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
 * QHexSpinbox.h
 *
 *  Created on: Dec 14, 2010
 *      Author: peter
 */

#ifndef QHEXSPINBOX_H_
#define QHEXSPINBOX_H_

#include <qspinbox.h>

class QHexSpinbox: public QSpinBox {
public:
	QHexSpinbox( QWidget * parent = 0 );
	virtual ~QHexSpinbox();
	int valueFromText(const QString &text) const;
	QString textFromValue(int value) const;
};

#endif /* QHEXSPINBOX_H_ */
