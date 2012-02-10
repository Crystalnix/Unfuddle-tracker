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

#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"

#include <QSettings>
#include <QKeyEvent>
#include <QKeySequence>

#include "settingskeys.h"

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);
    connect(this, SIGNAL(accepted()), SLOT(accepted()));

    ui->screensnapHotkeyLineEdit->installEventFilter(this);
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

void PreferencesDialog::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)

    QSettings settings;
    settings.beginGroup(PREFERENCES);

    ui->trayIconCheck->setChecked(
                settings.value(SHOW_TRAY_ICON, true).toBool());
    ui->signinOnLaunchCheck->setChecked(
                settings.value(SIGNIN_ON_LAUNCH, false).toBool());
    ui->systemStartupCheck->setChecked(
                settings.value(SYSTEM_STARTUP, false).toBool());
    ui->screensnapHotkeyLineEdit->setText(
                settings.value(SCREENSNAP_HOTKEY, QString()).toString());

    settings.endGroup();
}

bool PreferencesDialog::eventFilter(QObject *watched, QEvent *event)
{
    Q_UNUSED(watched);

    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        QKeySequence key(keyEvent->modifiers() + keyEvent->key());

        ui->screensnapHotkeyLineEdit->setText(key.toString(QKeySequence::NativeText));

        event->accept();
        return true;
    } else {
        return false;
    }
}

void PreferencesDialog::accepted()
{
    QSettings settings;
    settings.beginGroup(PREFERENCES);

    settings.setValue(SHOW_TRAY_ICON,
                      ui->trayIconCheck->isChecked());
    settings.setValue(SIGNIN_ON_LAUNCH,
                      ui->signinOnLaunchCheck->isChecked());
    settings.setValue(SYSTEM_STARTUP,
                      ui->systemStartupCheck->isChecked());
    settings.setValue(SCREENSNAP_HOTKEY,
                      ui->screensnapHotkeyLineEdit->text());

    settings.endGroup();
}
