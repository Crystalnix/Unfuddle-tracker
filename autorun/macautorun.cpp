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

#include "macautorun.h"
#include <QStringList>
#include <QDir>
#include <QList>

MacAutoRun::MacAutoRun()
    : loginwindow(QDir::homePath()
                  + QDir::separator()
                  + loginWindowPath
                  , QSettings::NativeFormat)
{
}

MacAutoRun::~MacAutoRun()
{
}

bool MacAutoRun::autoRunOn()
{
    if (isAutoRunning()) {
        return true;
    }

    const QString appPath = getAppPath();

    QVariantMap mlaunchItem;
    mlaunchItem.insert(pathSettingName, appPath);
    mlaunchItem.insert(hideSettingName, false);

    QVariantList launchItems = getLaunchItems();
    launchItems.append(mlaunchItem);

    setLaunchItems(launchItems);

    return true;
}

bool MacAutoRun::autoRunOff()
{
    const QString appPath = getAppPath();

    QVariantList launchItems = getLaunchItems();

    for (QVariantList::iterator i = launchItems.begin(); i != launchItems.end(); ++i) {
        QVariantMap mlaunchItem = i->toMap();
        if (mlaunchItem.value(pathSettingName) == appPath) {
            launchItems.erase(i);

            setLaunchItems(launchItems);

            return true;
        }
    }

    return true;
}

bool MacAutoRun::isAutoRunning() const
{
    const QString appPath = getAppPath();

    QVariantList launchItems = getLaunchItems();

    for (QVariantList::iterator i = launchItems.begin(); i != launchItems.end(); ++i) {
        QVariantMap mlaunchItem = i->toMap();
        if (mlaunchItem.value(pathSettingName) == appPath) {
            return true;
        }
    }

    return false;
}

QVariantList MacAutoRun::getLaunchItems() const
{
    return loginwindow.value(autoLaunchSettingName).toList();
}

void MacAutoRun::setLaunchItems(const QVariantList &launchItems)
{
    loginwindow.setValue(autoLaunchSettingName, launchItems);
}

QString MacAutoRun::getAppPath() const
{
    const QString appPath = qApp->applicationFilePath();
    const int index = appPath.lastIndexOf(appSuffix, -1, Qt::CaseInsensitive);
    return appPath.left(index + appSuffix.length());
}
