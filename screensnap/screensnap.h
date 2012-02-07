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

#ifndef SCREENSNAP_H
#define SCREENSNAP_H

#include <QObject>
#include <QScopedPointer>

#include "globalshortcut.h"
#include "dialog/screensnapdialog.h"
#include "editor/screensnapeditor.h"

class QMenu;
class MainWindow;

class Screensnap : public QObject
{
    Q_OBJECT

public:
    explicit Screensnap(QObject *parent = 0);

    QMenu * createScreensnapMenu();
    void setActiveDisplayShortcut(const QKeySequence &key);
    void enableShortcuts();
    void disableShortcuts();

signals:

private slots:
    void snapWindowByAction();
    void snapActiveWindow();

    void on_dialog_accepted();
    void on_dialog_rejected();
    void on_dialog_draw();
    void on_editor_screenshotChanged(const QPixmap &pixmap);

    void addedTicketComment(int project_id, int ticket_id, int comment_id);
    void addedAttachmentToTicketComment(int project_id, int ticket_id, int comment_id);

private:
    QScopedPointer<ScreensnapDialog> p_dialog;
    QScopedPointer<ScreensnapEditor> p_editor;
    QPixmap m_screenshot;
    GlobalShortcut m_activeDisplayShortcut;

    void showDialog(const QPixmap &pixmap);
    void finishScreensnap();
};

#endif // SCREENSNAP_H
