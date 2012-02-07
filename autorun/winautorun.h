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

#ifndef WINAUTORUN_H
#define WINAUTORUN_H

#include "autorunimp.h"
#include <QSettings>
#include <QCoreApplication>

class WinAutoRun : public AutoRunImp
{
public:
    WinAutoRun();
    virtual ~WinAutoRun();

    virtual bool autoRunOn();
    virtual bool autoRunOff();
    virtual bool isAutoRunning() const;

private:
    QSettings loginsettings;

    QString getRunString() const;
};

const QString loginSettingsPath = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run";

#endif // WINAUTORUN_H
