#ifndef DEBUGMONITOR_H
#define DEBUGMONITOR_H

#include <QtGui/QWidget>
#include <QStringList>
#include "ui_debugmonitor.h"

class DebugMonitor : public QWidget
{
    Q_OBJECT

public:
    DebugMonitor(QWidget *parent = 0);
    ~DebugMonitor();
public slots:
    void newAnswer( QStringList answer, bool ok );
    void button();
signals:
	void send( QString cmd );
private:
    Ui::DebugMonitorClass ui;
    QStringList list;
};

#endif // DEBUGMONITOR_H
