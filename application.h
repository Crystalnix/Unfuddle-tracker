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

#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>

#include "screensnap/screensnap.h"
#include "mainwindow.h"
#include "preferencesdialog.h"

class Application : public QApplication
{
    Q_OBJECT
public:
    explicit Application(int& argc, char* argv[]);
    ~Application();

    static Application& instance();
    static bool hasInstance();

    QSystemTrayIcon& trayIcon();
    MainWindow& mainWindow() { return m_mainWindow; }

    void applyDefaultSettings();
    void enableScreensnap();
    void disableScreensnap();

signals:

public slots:
    void preferencesChanged();

private slots:
    void loggedIn();
    void loggedOut();
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void showAboutDialog();

protected:
    void openAWindow(bool raise = false);

#ifdef Q_WS_MAC
    QMenu m_dockMenu;
    QScopedPointer<QMenu> m_mac_screensnapMenu;
#endif

private:
    Screensnap m_screensnap;
    QSystemTrayIcon m_trayIcon;
    QMenu m_mainMenu;
    QMenu m_trayIconMenu;
    QAction m_quitAction;
    QAction m_preferencesAction;
    QAction m_logoutAction;
    QAction m_aboutAction;
    MainWindow m_mainWindow;
    PreferencesDialog m_preferencesDlg;
    QScopedPointer<QMenu> m_screensnapMenu;

    static Application* m_instance;
};

#endif // APPLICATION_H
