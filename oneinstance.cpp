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

#include "oneinstance.h"

#if defined(Q_OS_WIN32)

OneInstance::~OneInstance()
{
    if (mutex != NULL) {
        CloseHandle(mutex);
    }
}

bool OneInstance::createMutex(const WCHAR * lockMutex)
{
    mutex = CreateMutex(NULL, FALSE, lockMutex);
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        mutex = NULL;
        return false;
    } else {
        return true;
    }
}
#endif

#if defined(Q_OS_UNIX)

bool OneInstance::openLockFile(const char * lockFile)
{
    struct flock fl;

    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 1;

    if((fdlock = open(lockFile, O_WRONLY|O_CREAT, 0666)) == -1)
        return false;

    if(fcntl(fdlock, F_SETLK, &fl) == -1)
        return false;

    return true;
}
#endif

OneInstance::OneInstance()
{
}

bool OneInstance::isRunned()
{
#if defined(Q_OS_WIN32)
    return !createMutex(TEXT("UnfTrMutex"));
#endif
#if defined(Q_OS_UNIX)
    return !openLockFile("/tmp/UnfTr.lock");
#endif
}
