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

#ifndef AUTORUN_H
#define AUTORUN_H

#include <QScopedPointer>
#include "autorunimp.h"

class AutoRun
{
public:
    AutoRun();

    bool setAutoRun(bool set = true);
    bool autoRunOn();
    bool autoRunOff();
    bool isAutoRunning() const;

private:
    QScopedPointer<AutoRunImp> _imp;
};

#endif // AUTORUN_H
