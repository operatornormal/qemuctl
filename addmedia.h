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
