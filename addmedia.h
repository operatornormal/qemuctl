#ifndef ADDMEDIA_H
#define ADDMEDIA_H

#include <QWidget>
#include <QFileDialog>
#include "ui_addmedia.h"

class AddMedia : public QWidget
{
    Q_OBJECT

public:
    AddMedia(QWidget *parent = 0);
    ~AddMedia();
    QString getCommand() const;
public slots:
    void mediaChanged(int index);
    void interfaceChanged(int index);
    void serialChanged(int index);
    void geometryChanged(int index);
    void selectFile();
    void doneButton();
    void cancelButton();
    void startNew();
signals:
	void newCommand( const QString & cmd );
private:
    Ui::AddMediaClass ui;
    QString command;
};

#endif // ADDMEDIA_H
