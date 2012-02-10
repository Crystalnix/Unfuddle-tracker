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

#ifndef GRAPHICSUTILITY_H
#define GRAPHICSUTILITY_H

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsProxyWidget>

#include <screensnap/objectscontrol/graphicstextedit.h>

class QUndoCommand;
class GraphicsItemEditor;
class QTextEdit;

class GraphicsUtility : public QObject
{
    Q_OBJECT

public:
    explicit GraphicsUtility(QGraphicsScene *scene) : p_scene(scene) {}
    virtual ~GraphicsUtility() {};

    inline QGraphicsScene * scene() { return p_scene; }

signals:
    void utilityFinished(QUndoCommand *command);

private:
    QGraphicsScene *p_scene;
};

class CropUtility : public GraphicsUtility
{
    Q_OBJECT

public:
    explicit CropUtility(QGraphicsScene *scene, QGraphicsView *view);

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    QGraphicsView *p_view;
    QGraphicsRectItem m_rect;

    void updatePoints(const QPointF &firstPoint, const QPointF &point);
};

class InteractiveTextEditUtility : public GraphicsUtility
{
    Q_OBJECT

public:
    InteractiveTextEditUtility(QGraphicsTextItem *textItem, QGraphicsScene *scene, GraphicsItemEditor *editor);
    ~InteractiveTextEditUtility();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    QGraphicsTextItem *p_textItem;
    GraphicsItemEditor *p_editor;
    QScopedPointer<GraphicsTextEdit> p_proxy;
};

#endif // GRAPHICSUTILITY_H
