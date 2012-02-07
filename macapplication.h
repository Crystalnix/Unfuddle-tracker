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

#ifndef MACAPPLICATION_H
#define MACAPPLICATION_H

#include "application.h"

class NSEvent;
class objc_object;

class MacApplication : public Application
{
    Q_OBJECT
public:
    MacApplication( int& argc, char* argv[] );
    ~MacApplication();
    // This method to be public due to lack of friend classes in Objective-C and
    // the lack inheritance of Objective-C classes from C++ ones.
    void dockIconClickEvent();

private slots:
    void afterStartup() const;

private:
    NSEvent* cocoaEventFilter( NSEvent* incomingEvent );
    void setupCocoaEventHandler() const;

    objc_object* m_pool;
    objc_object* m_dockIconClickEventHandler;
};

#endif // MACAPPLICATION_H
