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

#include "globalshortcut.h"
#include <QKeySequence>

QHash<QPair<quint32, quint32>, GlobalShortcut*> GlobalShortcut::shortcuts;
bool GlobalShortcut::error = false;

#ifndef Q_WS_MAC
int GlobalShortcut::ref = 0;
QAbstractEventDispatcher::EventFilter GlobalShortcut::prevEventFilter = 0;
#endif // Q_WS_MAC

GlobalShortcut::GlobalShortcut(QObject *parent) :
    QObject(parent), key(Qt::Key(0)), mods(Qt::NoModifier)
{
#ifndef Q_WS_MAC
    if (!ref++)
        prevEventFilter = QAbstractEventDispatcher::instance()->setEventFilter(eventFilter);
#endif // Q_WS_MAC
}

GlobalShortcut::GlobalShortcut(const QKeySequence &shortcut, QObject *parent)
{
    Q_UNUSED(parent);
    setShortcut(shortcut);
}

bool GlobalShortcut::setShortcut(const QKeySequence& shortcut)
{
    if((prevNativeKey != 0) || (prevNativeMods != 0))
    {
        unregisterShortcut(prevNativeKey, prevNativeMods);
    }
    Qt::KeyboardModifiers allMods = Qt::ShiftModifier | Qt::ControlModifier | Qt::AltModifier | Qt::MetaModifier;
    key = shortcut.isEmpty() ? Qt::Key(0) : Qt::Key((shortcut[0] ^ allMods) & shortcut[0]);
    mods = shortcut.isEmpty() ? Qt::KeyboardModifiers(0) : Qt::KeyboardModifiers(shortcut[0] & allMods);
    const quint32 nativeKey = nativeKeycode(key);
    const quint32 nativeMods = nativeModifiers(mods);
    const bool res = registerShortcut(nativeKey, nativeMods);
    shortcuts.insert(qMakePair(nativeKey, nativeMods), this);

    prevNativeKey = nativeKey; // remember key to have an opportunity to delete it
    prevNativeMods = nativeMods;

    //if (!res)
        //qWarning() << "GlobalShortcut failed to register:" << QKeySequence(key + mods).toString();
    return res;
}

void GlobalShortcut::activateShortcut(quint32 nativeKey, quint32 nativeMods)
{
    GlobalShortcut* shortcut = shortcuts.value(qMakePair(nativeKey, nativeMods));
    if (shortcut) emit shortcut->activated();
}
