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

#include "x11autorun.h"

#include <QFile>
#include <QApplication>

X11AutoRun::X11AutoRun()
{
}

X11AutoRun::~X11AutoRun()
{
}

bool X11AutoRun::autoRunOn()
{
    QDir dir = autoRunDir();
    if (dir == QDir()) {
        dir.mkdir("autostart");
        dir.cd("autostart");
    }

    QString content = QString("[Desktop Entry]\n"
                    "Encoding=UTF-8\n"
                    "Name=%1\n"
                    "GenericName=\n"
                    "Type=Application\n"
                    "Exec=%2\n"
                    "Terminal=false\n"
                    "X-GNOME-Autostart-Delay=10\n")
            .arg(QApplication::applicationName(), QApplication::applicationFilePath());

    QFile desktop(dir.path() + QDir::separator() + QApplication::applicationName() + ".desktop");
    desktop.open(QFile::WriteOnly);
    desktop.write(content.toUtf8());
    desktop.close();
    return true;
}

bool X11AutoRun::autoRunOff()
{
    QDir dir = autoRunDir();
    if (dir == QDir()) {
        return true;
    }
    dir.remove(QApplication::applicationName() + ".desktop");
    return true;
}

bool X11AutoRun::isAutoRunning() const
{
    QDir dir = autoRunDir();
    if (dir == QDir()) {
        return false;
    }
    return dir.exists(QApplication::applicationName() + ".desktop");
}

QDir X11AutoRun::autoRunDir() const
{
    QDir dir = QDir::home();
    dir.cd(".config");
    if (dir.exists("autostart")) {
        dir.cd("autostart");
        return dir;
    } else {
        return QDir();
    }
}
