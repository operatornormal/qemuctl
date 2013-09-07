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
