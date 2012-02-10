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

#include "application.h"

#include <QSettings>
#include <QMenuBar>

#include "settingskeys.h"
#include "autorun/autorun.h"
#include "ui_aboutdialog.h"

Application* Application::m_instance = 0;

Application::Application(int& argc, char* argv[]) :
    QApplication(argc, argv),
    m_quitAction(this),
    m_preferencesAction(this),
    m_logoutAction(this),
    m_aboutAction(this)
{
    setQuitOnLastWindowClosed(false);

    QCoreApplication::setOrganizationName("Crystalnix");
    QCoreApplication::setOrganizationDomain("crystalnix.com");
    QCoreApplication::setApplicationName("utracker");
    QCoreApplication::setApplicationVersion("1.0");
    setWindowIcon(QIcon(":/images/transparent-logo"));

    Q_ASSERT_X(m_instance == 0, "Application ctor",
                   "Application is a singleton and cannot be created more than once");
    m_instance = this;

    QSettings settings;
    if (!settings.value("preferences/didApplyDefault", false).toBool())
        applyDefaultSettings();

    m_screensnapMenu.reset(m_screensnap.createScreensnapMenu());
    m_screensnapMenu->setEnabled(false);

    m_trayIconMenu.addMenu(m_screensnapMenu.data());

    m_preferencesAction.setText("Settings");
    m_preferencesAction.setMenuRole(QAction::PreferencesRole);

    m_logoutAction.setText(tr("Log out"));
    m_logoutAction.setEnabled(false);

    m_aboutAction.setText(tr("&About"));
    m_aboutAction.setMenuRole(QAction::AboutRole);

    m_quitAction.setText("&Quit");

    m_mainMenu.setTitle("&Unfuddle");
    m_mainMenu.addMenu(m_screensnapMenu.data());
    m_mainMenu.addAction(&m_preferencesAction);
    m_mainMenu.addAction(&m_logoutAction);
    m_mainMenu.addSeparator();
    m_mainMenu.addAction(&m_quitAction);
    m_mainWindow.menuBar()->addMenu(&m_mainMenu);

    QMenu *helpMenu = m_mainWindow.menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(&m_aboutAction);

#ifdef Q_WS_MAC
    m_mac_screensnapMenu.reset(m_screensnap.createScreensnapMenu());
    m_mac_screensnapMenu->setEnabled(false);

    m_dockMenu.addMenu(m_mac_screensnapMenu.data());
    m_dockMenu.addAction(&m_logoutAction);
#endif

    m_trayIconMenu.addAction(&m_logoutAction);

    m_trayIconMenu.addSeparator();

    m_trayIconMenu.addAction(&m_quitAction);

    m_trayIcon.setIcon(QIcon(":/images/favicon.ico"));
    m_trayIcon.setContextMenu(&m_trayIconMenu);

    preferencesChanged();

    connect(&m_preferencesAction, SIGNAL(triggered()), &m_preferencesDlg, SLOT(exec()));
    connect(&m_logoutAction, SIGNAL(triggered()), &m_mainWindow, SLOT(needsLogin()));
    connect(&m_preferencesDlg, SIGNAL(accepted()), SLOT(preferencesChanged()));
    connect(&m_aboutAction, SIGNAL(triggered()), SLOT(showAboutDialog()));
    connect(&m_quitAction, SIGNAL(triggered()), SLOT(quit()));
    connect(&m_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));
    connect(&m_mainWindow, SIGNAL(loggedIn()), SLOT(loggedIn()));
    connect(&m_mainWindow, SIGNAL(loggedOut()), SLOT(loggedOut()));

    m_mainWindow.show();

    settings.beginGroup(PREFERENCES);
    if (settings.value(SIGNIN_ON_LAUNCH, false).toBool()) {
        m_mainWindow.login();
    }
    settings.endGroup();
}

Application::~Application()
{
}

Application& Application::instance()
{
    Q_ASSERT_X(m_instance, "Application::instance",
               "Singleton not constructed yet");
    return *m_instance;
}

bool Application::hasInstance()
{
    return m_instance != 0;
}

QSystemTrayIcon& Application::trayIcon()
{
    return m_trayIcon;
}

void Application::applyDefaultSettings()
{
    QSettings settings;
    settings.setValue("preferences/didApplyDefault", true);
    settings.setValue("preferences/showTrayIcon", true);
}

void Application::preferencesChanged()
{
    QSettings settings;
    settings.beginGroup(PREFERENCES);
    m_trayIcon.setVisible(settings.value(SHOW_TRAY_ICON).toBool());
    const QString &keyStr = settings.value(SCREENSNAP_HOTKEY).toString();
    const QKeySequence &key = keyStr.isEmpty() ? QKeySequence("Ctrl+Alt+A") : QKeySequence(keyStr);
    m_screensnap.setActiveDisplayShortcut(key);
    AutoRun ar;
    ar.setAutoRun(settings.value(SYSTEM_STARTUP).toBool());
    settings.endGroup();
}

void Application::openAWindow(bool raise)
{
    m_mainWindow.showNormal();
    if (raise)
        m_mainWindow.raise();
}

void Application::loggedIn()
{
    m_logoutAction.setEnabled(true);
}

void Application::loggedOut()
{
    m_logoutAction.setEnabled(false);
}

void Application::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::DoubleClick || reason == QSystemTrayIcon::Trigger) {
        openAWindow();
    }
}

void Application::enableScreensnap()
{
    m_screensnap.enableShortcuts();
    m_screensnapMenu->setEnabled(true);
#ifdef Q_WS_MAC
    m_mac_screensnapMenu->setEnabled(true);
#endif
}

void Application::disableScreensnap()
{
    m_screensnap.disableShortcuts();
    m_screensnapMenu->setEnabled(false);
#ifdef Q_WS_MAC
    m_mac_screensnapMenu->setEnabled(false);
#endif
}

void Application::showAboutDialog()
{
    QDialog aboutDialog;
    Ui::AboutDialog ui;
    ui.setupUi(&aboutDialog);
    const QString &app = ui.applicationNameLabel->text();
    ui.applicationNameLabel->setText(app.arg(applicationVersion()));
    aboutDialog.exec();
}
