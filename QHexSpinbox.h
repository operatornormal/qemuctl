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
