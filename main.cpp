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

#include <QApplication>
#include <QProcess>
#include <QString>
#include <QStringList>
/*
#include <QX11EmbedContainer>

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);

	QX11EmbedContainer frame;

	QProcess mplayer;
	mplayer.start("/usr/bin/qemu", QStringList("-wid")
	              << QString::number(frame.winId())
	              << "http://blip.tv/file/get/Lc-BdaleGarbee394.OGG");



	frame.resize(800,600);
	frame.show();

	return app.exec();
}*/


#include <QtGui>
#include <QApplication>
#include <QStringList>

#include "qemuctl.h"

int main(int argc, char *argv[])
{
	QStringList * arguments = new QStringList();
	for( int i = 0; i < argc; i++ ){
		arguments->append(QString::fromLocal8Bit(argv[i]));
	}
    QApplication a(argc, argv);
    QApplication::setOrganizationName("Qemuctl");
    QCoreApplication::setOrganizationDomain("qemuctl.sourceforge.net");
    QCoreApplication::setApplicationName("Qemuctl");
    Q_INIT_RESOURCE(qemuctl);
    qemuctl w(arguments);
    w.show();
    return a.exec();
}

