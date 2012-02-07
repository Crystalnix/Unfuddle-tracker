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

#ifndef GLOBALSHORTCUT_H
#define GLOBALSHORTCUT_H

#include <QAbstractEventDispatcher>
#include <QHash>
#include <QObject>

QT_BEGIN_NAMESPACE
class QKeySequence;
QT_END_NAMESPACE

class GlobalShortcut : public QObject
{
    Q_OBJECT
public:
    explicit GlobalShortcut(QObject *parent = 0);
    explicit GlobalShortcut(const QKeySequence& shortcut, QObject* parent = 0);

    Qt::Key key;
    Qt::KeyboardModifiers mods;
    quint32 prevNativeKey, prevNativeMods;

    static bool error;

    bool setShortcut(const QKeySequence& shortcut);
    static void activateShortcut(quint32 nativeKey, quint32 nativeMods);

#ifndef Q_WS_MAC
    static int ref;
    static QAbstractEventDispatcher::EventFilter prevEventFilter;
    static bool eventFilter(void* message);
#endif // Q_WS_MAC

private:
    static quint32 nativeKeycode(Qt::Key keycode);
    static quint32 nativeModifiers(Qt::KeyboardModifiers modifiers);

    static bool registerShortcut(quint32 nativeKey, quint32 nativeMods);
    static bool unregisterShortcut(quint32 nativeKey, quint32 nativeMods);

    static QHash<QPair<quint32, quint32>, GlobalShortcut*> shortcuts;

signals:
    void activated();

public slots:

};

#endif // GLOBALSHORTCUT_H
