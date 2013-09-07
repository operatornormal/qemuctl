#ifndef SERIALCONNECTOR_H
#define SERIALCONNECTOR_H

#include <QWidget>
#include <QFileDialog>
#include <QNetworkInterface>
#include <QList>
#include <QNetworkAddressEntry>
#include <QDir>
#include <QStringList>
#include <QFileInfoList>
#include <QHostAddress>
#include <QAbstractSocket>
#include <QVBoxLayout>
#include "ui_serialconnector.h"

class SerialConnector : public QWidget
{
    Q_OBJECT

public:
    SerialConnector(QWidget *parent = 0);
    ~SerialConnector();
    const QString getCommand();
    QVBoxLayout * getMainLayout();
public slots:
	void typeChanged( int type );
	void tcpServerCheckBox( int state );
	void done();
	void cancel();
	void fileChoose();
	void pipeChoose();
	void unixChoose();
signals:
	void newCommand( const QString & cmd );
private:
    Ui::SerialConnectorClass ui;
    QString command;
    void fillHostAdresses();
    void fillParport();
    void fillTTY();
};

#endif // SERIALCONNECTOR_H
