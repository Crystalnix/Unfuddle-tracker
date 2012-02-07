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

#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include <QGraphicsItem>
#include <QPen>
#include <QFont>

class GraphicsBrushItem : public QGraphicsItemGroup
{
public:
    explicit GraphicsBrushItem(QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);
    GraphicsBrushItem(const QPointF &point, QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);
    GraphicsBrushItem(const QList<QPointF> &points, QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);

    QPen pen() const;
    void setPen(const QPen &pen);

    void addPoint(const QPointF &point);
    void clearPoints();

    QRectF boundingRect() const;

    enum { Type = UserType + 1 };
    int type() const;

private:
    QList<QGraphicsLineItem *> m_lines;
    QPen m_pen;
    QRectF m_boundingRect;

    Q_DISABLE_COPY(GraphicsBrushItem)
};

class GraphicsArrowItem : public QGraphicsLineItem
{
public:
    explicit GraphicsArrowItem(QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);
    GraphicsArrowItem(const QLineF &line, qreal angle = 30, qreal altLength = 0.2,
                      QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);

    inline qreal altAngle() const { return m_altAngle; }
    inline void setAltAngle(qreal altAngle) { m_altAngle = altAngle; }

    inline qreal altLength() const { return m_altLength; }
    inline void setAltLength(qreal altLength) { m_altLength = altLength; }

    QLineF leftLine() const;
    QLineF rightLine() const;

    QRectF boundingRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    QPainterPath shape() const;

    enum { Type = UserType + 2 };
    int type() const;

private:
    qreal m_altAngle;
    qreal m_altLength;

    Q_DISABLE_COPY(GraphicsArrowItem)
};

#endif // PRIMITIVES_H
