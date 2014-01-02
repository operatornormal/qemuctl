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

#ifndef QDISCFILEDIALOG_H
#define QDISCFILEDIALOG_H

#include <QtGui/QWidget>
#include "ui_QDiscFileDialog.h"

class QDiscFileDialog : public QWidget
{
    Q_OBJECT

public:
    QDiscFileDialog(QWidget *parent = 0);
    ~QDiscFileDialog();

private:
    Ui::QDiscFileDialogClass ui;
};

#endif // QDISCFILEDIALOG_H
