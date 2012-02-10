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

#ifndef ONEINSTANCE_H
#define ONEINSTANCE_H

#include <QObject>
#include "stdlib.h"
#if defined(Q_OS_WIN32)
#include "windows.h"
#endif

#if defined(Q_OS_UNIX)
#include <unistd.h>
#include <fcntl.h>
#endif

class OneInstance
{
public:
    OneInstance();
    bool isRunned();
#if defined(Q_OS_WIN32)

    HANDLE mutex;
    ~OneInstance();
    bool createMutex(const WCHAR * lockMutex);

#endif

#if defined(Q_OS_UNIX)

    int fdlock;
    bool openLockFile(const char * lockFile);

#endif

};

#endif // ONEINSTANCE_H
