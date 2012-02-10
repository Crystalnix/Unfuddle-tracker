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

#ifndef MACAUTORUN_H
#define MACAUTORUN_H

#include "autorunimp.h"
#include <QSettings>
#include <QCoreApplication>

class MacAutoRun : public AutoRunImp
{
public:
    MacAutoRun();
    virtual ~MacAutoRun();

    virtual bool autoRunOn();
    virtual bool autoRunOff();
    virtual bool isAutoRunning() const;

private:
    QSettings loginwindow;

    QVariantList getLaunchItems() const;
    void setLaunchItems(const QVariantList &launchItems);
    QString getAppPath() const;
};

const QString loginWindowPath = "Library/Preferences/loginwindow.plist";
const QString autoLaunchSettingName = "AutoLaunchedApplicationDictionary";
const QString hideSettingName = "Hide";
const QString pathSettingName = "Path";
const QString aliasDataSettingName = "AliasData";

const QString appSuffix = ".app";

#endif // MACAUTORUN_H
