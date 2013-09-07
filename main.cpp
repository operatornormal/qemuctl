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
    Q_INIT_RESOURCE(qemuctl);
    qemuctl w(arguments);
    w.show();
    return a.exec();
}

