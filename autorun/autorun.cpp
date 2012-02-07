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

#include "autorun.h"

#ifdef Q_OS_MAC
#include "macautorun.h"
#endif
#ifdef Q_OS_WIN
#include "winautorun.h"
#endif
#ifdef Q_OS_LINUX
#include "x11autorun.h"
#endif

AutoRun::AutoRun() :
        _imp(new
#ifdef Q_OS_MAC
                MacAutoRun
#endif
#ifdef Q_OS_WIN
                WinAutoRun
#endif
#ifdef Q_OS_LINUX
                X11AutoRun
#endif
             )
{
}

bool AutoRun::setAutoRun(bool set)
{
    return set ? autoRunOn() : autoRunOff();
}

bool AutoRun::autoRunOn()
{
    return _imp->autoRunOn();
}

bool AutoRun::autoRunOff()
{
    return _imp->autoRunOff();
}

bool AutoRun::isAutoRunning() const
{
    return _imp->isAutoRunning();
}
