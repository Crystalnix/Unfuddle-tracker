/*
*    Unfuddle Tracker is a time tracking tool for Unfuddle service.
*    Copyright (C) 2012	Vadim Zakondyrin <thekondr@crystalnix.com>
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

#include "winautorun.h"
#include <QStringList>
#include <QList>
#include <QDir>

WinAutoRun::WinAutoRun()
    : loginsettings(loginSettingsPath, QSettings::NativeFormat)
{
}

WinAutoRun::~WinAutoRun()
{
}

bool WinAutoRun::autoRunOn()
{
    if (isAutoRunning()) {
        return true;
    }

    const QString runStr = getRunString();
    loginsettings.setValue(qApp->applicationName(), runStr);
    return true;
}

bool WinAutoRun::autoRunOff()
{
    const QString runStr = getRunString();
    QStringList items = loginsettings.allKeys();
    for (QStringList::iterator i = items.begin(); i != items.end(); ++i) {
        if (loginsettings.value(*i).toString() == runStr) {
            loginsettings.remove(*i);
            return true;
        }
    }
    return true;
}

bool WinAutoRun::isAutoRunning() const
{
    const QString runStr = getRunString();
    QStringList items = loginsettings.allKeys();
    for (QStringList::iterator i = items.begin(); i != items.end(); ++i) {
        if (loginsettings.value(*i).toString() == runStr) {
            return true;
        }
    }
    return false;
}

QString WinAutoRun::getRunString() const
{
    return QDir::toNativeSeparators(qApp->applicationFilePath());
}
