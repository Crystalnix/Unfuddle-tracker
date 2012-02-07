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

#include "mainwindow.h"
#include "logindialog.h"
#include "mainuiwidget.h"
#include "unfuddleapiwrapper.h"
#include "screensnap/screensnap.h"

#include <QStackedWidget>
#include <QSettings>
#include <QMessageBox>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QApplication>
#include <QStyle>

MainWindow::MainWindow(QWidget *parent)
    :
    QMainWindow(parent),
    parentWidget(new QStackedWidget()),
    mainUI(0),
    myStatusBar(new MyStatusBar(this))
{
    this->setCentralWidget(parentWidget);
    this->setStatusBar(myStatusBar);
    setWindowTitle("Unfuddle Tracker");

    loginDialog = new LoginDialog(parentWidget);
    parentWidget->addWidget(loginDialog);

    this->resize(loginDialog->width(), loginDialog->height());

    connect(loginDialog, SIGNAL(loggedIn()), SLOT(slotLoggedIn()));
}

MainWindow::~MainWindow()
{
    delete parentWidget;
}

void MainWindow::login()
{
    loginDialog->doLogin();
}

void MainWindow::slotLoggedIn()
{
    if (!mainUI) {
        mainUI = new MainUIWidget(parentWidget);
        parentWidget->addWidget(mainUI);
        connect(mainUI, SIGNAL(unauthorizedAccess()), SLOT(needsLogin()));
        //connect(mainUI, SIGNAL(wentOffline()), SLOT(wentOffline()));
        //connect(mainUI, SIGNAL(wentOnline()), SLOT(wentOnline()));
    } else
        mainUI->refreshData(true);

    parentWidget->setCurrentWidget(mainUI);
    this->adjustSize();

    myStatusBar->setStatus(
            QString("User %1 successfully logged in to %2").arg(
                    UnfuddleAPIWrapper::instance()->getLogin(),
                    UnfuddleAPIWrapper::instance()->getSubdomain())
            );

    emit loggedIn();
}


void MainWindow::needsLogin()
{
    this->resize(loginDialog->width(), loginDialog->height());
    parentWidget->setCurrentWidget(loginDialog);

    mainUI->close();
    mainUI->deleteLater();
    mainUI = 0;

    emit loggedOut();

    myStatusBar->setStatus(tr("User successfully logged out"));
}

void MainWindow::wentOffline()
{
//    QMessageBox msg;
//    msg.setText("Went <span style='color:red'>offline</span>");
//    msg.exec();
}

void MainWindow::wentOnline()
{
//    QMessageBox msg;
//    msg.setText("Went <span style='color:green'>online</span>");
//    msg.exec();
}
