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

#include <Cocoa/Cocoa.h>

#include "MacApplication.h"

#include <QTimer>

extern void qt_mac_set_dock_menu(QMenu*);

@interface DockIconClickEventHandler : NSObject
{
@public
    MacApplication* macApplication;
}
- (void)handleDockClickEvent:(NSAppleEventDescriptor*)event withReplyEvent:(NSAppleEventDescriptor*)replyEvent;
@end

@implementation DockIconClickEventHandler
- (void)handleDockClickEvent:(NSAppleEventDescriptor*)event withReplyEvent:(NSAppleEventDescriptor*)replyEvent {
    Q_UNUSED(event)
    Q_UNUSED(replyEvent)

    if (macApplication)
        macApplication->dockIconClickEvent();
}
@end

MacApplication::MacApplication( int& argc, char* argv[] )
    : Application( argc, argv ), m_pool([[NSAutoreleasePool alloc] init]),
    m_dockIconClickEventHandler([[DockIconClickEventHandler alloc] init])
{
    DockIconClickEventHandler* dockIconClickEventHandler =
            static_cast<DockIconClickEventHandler*>(m_dockIconClickEventHandler);
    dockIconClickEventHandler->macApplication = this;

    QTimer::singleShot(100, this, SLOT(afterStartup()));    // wait until application goes to exec cycle

    qt_mac_set_dock_menu( &m_dockMenu );

    QCoreApplication::setAttribute( Qt::AA_DontShowIconsInMenus );
}

MacApplication::~MacApplication()
{
    [m_pool drain];
}

void MacApplication::afterStartup() const
{
    setupCocoaEventHandler();
}

void MacApplication::setupCocoaEventHandler() const
{
    // TODO: This apparently uses a legacy API and we should be using the
    // applicationShouldHandleReopen:hasVisibleWindows: method on
    // NSApplicationDelegate but this isn't possible without nasty runtime
    // reflection hacks until Qt is fixed. If this breaks, shout at them :)
    [[NSAppleEventManager sharedAppleEventManager]
     setEventHandler:m_dockIconClickEventHandler
     andSelector:@selector(handleDockClickEvent:withReplyEvent:)
     forEventClass:kCoreEventClass
     andEventID:kAEReopenApplication];
}

void MacApplication::dockIconClickEvent()
{
    openAWindow();
}
