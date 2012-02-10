/*
*    Unfuddle Tracker is a time tracking tool for Unfuddle service.
*    Copyright (C) 2012	Viatcheslav Gachkaylo <vgachkaylo@crystalnix.com>
*
*    This file is part of Unfuddle Tracker.
*
*    Unfuddle Tracker is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    Unfuddle Tracker is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with Unfuddle Tracker.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CHOOSEMULTIPLEFILESENTRYWIDGET_H
#define CHOOSEMULTIPLEFILESENTRYWIDGET_H

#include <QWidget>

class QPushButton;
class LabeledLineEdit;
class QToolButton;

class ChooseMultipleFilesEntryWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ChooseMultipleFilesEntryWidget(QWidget *parent = 0);

    enum Mode {
        MODE_ADD,
        MODE_REMOVE
    };

    Mode getMode() const { return mode; }
    void setMode(Mode _mode);

    QString getFilePath() const;
signals:
    void addRemoveButtonClicked(ChooseMultipleFilesEntryWidget *eventSource);
public slots:
private slots:
    void onBrowseButtonClicked();
    void onAddRemoveButtonClicked();
private:
    Mode mode;
    QPushButton *addRemoveButton;
    LabeledLineEdit *filePath;
    QToolButton *browseButton;

    static QIcon addIcon;
    static QIcon removeIcon;
    static bool initialized;
};

#endif // CHOOSEMULTIPLEFILESENTRYWIDGET_H
