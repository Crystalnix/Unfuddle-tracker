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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

#include "mystatusbar.h"

class LoginDialog;
class MainUIWidget;
class QStackedWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    const MainUIWidget *getMainUI() const { return mainUI; }

    void setStatus(const QString &message, MyStatusBar::Status s = MyStatusBar::STATUS_DEFAULT)
    {
        myStatusBar->setStatus(message, s);
    }

    //MyStatusBar * getMyStatusBar() const { return myStatusBar; }
    void login();

signals:
    void loggedIn();
    void loggedOut();

public slots:
    void needsLogin();

private slots:
    void slotLoggedIn();
    void wentOffline();
    void wentOnline();

private:
    QStackedWidget *parentWidget;
    LoginDialog *loginDialog;
    MainUIWidget *mainUI;
    MyStatusBar *myStatusBar;
};

#endif // MAINWINDOW_H
