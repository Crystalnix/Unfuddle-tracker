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

#ifndef GRAPHICSITEMCREATOR_H
#define GRAPHICSITEMCREATOR_H

#include <QObject>
#include <QPointF>
#include <QGraphicsScene>
#include <QScopedPointer>
#include <QGraphicsProxyWidget>
#include <screensnap/primitives.h>

#include "graphicsitemeditor.h"
#include "graphicstextedit.h"

class GraphicsBrushItem;
class GraphicsArrowItem;
class QTextEdit;

class GraphicsItemCreator : public QObject
{
    Q_OBJECT

public:
    explicit GraphicsItemCreator(QGraphicsScene *scene) : p_scene(scene) {}
    virtual ~GraphicsItemCreator() {};

    inline QGraphicsScene * scene() { return p_scene; }

signals:
    void itemCreated(QGraphicsItem *item);

private:
    QGraphicsScene *p_scene;
};

class GraphicsLineItemCreator : public GraphicsItemCreator
{
    Q_OBJECT

public:
    explicit GraphicsLineItemCreator(QGraphicsScene *scene);

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    QScopedPointer<QGraphicsLineItem> p_line;

    void updatePoints(const QPointF &firstPoint, const QPointF &point);
};

class GraphicsRectItemCreator : public GraphicsItemCreator
{
    Q_OBJECT

public:
    explicit GraphicsRectItemCreator(QGraphicsScene *scene);

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    QScopedPointer<QGraphicsRectItem> p_rect;

    void updatePoints(const QPointF &firstPoint, const QPointF &point);
};

class GraphicsBrushItemCreator : public GraphicsItemCreator
{
    Q_OBJECT

public:
    explicit GraphicsBrushItemCreator(QGraphicsScene *scene);

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    QScopedPointer<GraphicsBrushItem> p_brushItem;

    void updatePoints(const QPointF &point);
};

class GraphicsArrowItemCreator : public GraphicsItemCreator
{
    Q_OBJECT

public:
    explicit GraphicsArrowItemCreator(QGraphicsScene *scene);

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    QScopedPointer<GraphicsArrowItem> p_arrow;

    void updatePoints(const QPointF &firstPoint, const QPointF &point);
};

class GraphicsTextItemCreator : public GraphicsItemCreator
{
    Q_OBJECT

public:
    explicit GraphicsTextItemCreator(const QFont &font, QGraphicsScene *scene);
    ~GraphicsTextItemCreator();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    QScopedPointer<GraphicsTextEdit> p_proxy;
    bool resizing;

    void updatePoints(const QPointF &firstPoint, const QPointF &point);
    void updateBoundingRect();
    void showElems();
    void hideElems();
    void createTextItem();
};

#endif // GRAPHICSITEMCREATOR_H
