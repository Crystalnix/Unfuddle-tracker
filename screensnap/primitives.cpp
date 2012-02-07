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

#include "primitives.h"

#include <QPainter>

GraphicsBrushItem::GraphicsBrushItem(QGraphicsItem *parent,
                                     QGraphicsScene *scene)
    : QGraphicsItemGroup(parent, scene)
{
}

GraphicsBrushItem::GraphicsBrushItem(const QPointF &point,
                                     QGraphicsItem *parent,
                                     QGraphicsScene *scene)
    : QGraphicsItemGroup(parent, scene)
{
    addPoint(point);
}

GraphicsBrushItem::GraphicsBrushItem(const QList<QPointF> &points,
                                     QGraphicsItem *parent,
                                     QGraphicsScene *scene)
    : QGraphicsItemGroup(parent, scene)
{
    foreach (const QPointF &point, points) {
        addPoint(point);
    }
}

QPen GraphicsBrushItem::pen() const
{
    return m_pen;
}

void GraphicsBrushItem::setPen(const QPen &pen)
{
    m_pen = pen;
    m_pen.setCapStyle(Qt::RoundCap);

    foreach (QGraphicsLineItem *lineItem, m_lines) {
        lineItem->setPen(m_pen);
    }
}

void GraphicsBrushItem::addPoint(const QPointF &point)
{
    prepareGeometryChange();

    QScopedPointer<QGraphicsLineItem> lineItem;

    if (m_lines.isEmpty()) {
        m_boundingRect = QRectF(point, point);
        lineItem.reset(new QGraphicsLineItem(QLineF(point, point), this));
    } else {
        const qreal x = point.x();
        const qreal y = point.y();

        if (m_boundingRect.left() > x) {
            m_boundingRect.setLeft(x);
        }

        if (m_boundingRect.right() < x) {
            m_boundingRect.setRight(x);
        }

        if (m_boundingRect.top() > y) {
            m_boundingRect.setTop(y);
        }

        if (m_boundingRect.bottom() < y) {
            m_boundingRect.setBottom(y);
        }

        lineItem.reset(new QGraphicsLineItem(QLineF(m_lines.last()->line().p2(), point), this));
    }

    lineItem->setPen(m_pen);
    m_lines.append(lineItem.data());
    addToGroup(lineItem.take());
    update();
}

void GraphicsBrushItem::clearPoints()
{
    prepareGeometryChange();

    QScopedPointer<QGraphicsItem> spItem;
    foreach (QGraphicsItem *item, m_lines) {
        spItem.reset(item);
        removeFromGroup(spItem.data());
    }
    spItem.reset();

    m_lines.clear();
    m_boundingRect = QRectF();
    update();
}

int GraphicsBrushItem::type() const
{
    return Type;
}

QRectF GraphicsBrushItem::boundingRect() const
{
    const qreal width = m_pen.widthF() / 2;
    return m_boundingRect.adjusted(-width, -width, width, width);
}

GraphicsArrowItem::GraphicsArrowItem(QGraphicsItem *parent,
                                     QGraphicsScene *scene)
    : QGraphicsLineItem(parent, scene),
      m_altAngle(30),
      m_altLength(0.2)
{
}

GraphicsArrowItem::GraphicsArrowItem(const QLineF &line,
                                     qreal altAngle,
                                     qreal altLength,
                                     QGraphicsItem *parent,
                                     QGraphicsScene *scene)
    : QGraphicsLineItem(line, parent, scene)
{
    QPen p = pen();
    p.setCapStyle(Qt::RoundCap);
    setPen(p);
    setAltAngle(altAngle);
    setAltLength(altLength);
}

QRectF GraphicsArrowItem::boundingRect() const
{
    QRectF boundingRect = QGraphicsLineItem::boundingRect();

    const qreal width = pen().widthF() / 2;
    const QPointF &lp = leftLine().p2();
    const QPointF &rp = rightLine().p2();
    const QPointF &mp = line().p2();
    const qreal x1 = qMin(mp.x(), qMin(lp.x(), rp.x()));
    const qreal x2 = qMax(mp.x(), qMax(lp.x(), rp.x()));
    const qreal y1 = qMin(mp.y(), qMin(lp.y(), rp.y()));
    const qreal y2 = qMax(mp.y(), qMax(lp.y(), rp.y()));

    return boundingRect.adjusted(width, width, -width, -width)
            .united(QRectF(x1, y1, x2 - x1, y2 - y1))
            .adjusted(-width, -width, width, width);
}

void GraphicsArrowItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QPen p = pen();

    if (p.capStyle() != Qt::RoundCap) {
        p.setCapStyle(Qt::RoundCap);
        setPen(p);
    }

    if (line().p1() != line().p2()) {
        painter->setPen(p);
        painter->drawLine(leftLine());
        painter->drawLine(rightLine());
        QGraphicsLineItem::paint(painter, option, widget);
    }
}

QPainterPath GraphicsArrowItem::shape() const
{
    QPainterPath path;

    if (line() == QLineF()) {
        return path;
    }

    const QLineF &l = line();
    path.moveTo(l.p1());
    path.lineTo(l.p2());

    const QLineF &ll = leftLine();
    path.moveTo(ll.p1());
    path.lineTo(ll.p2());

    const QLineF &rl = rightLine();
    path.moveTo(rl.p1());
    path.lineTo(rl.p2());

    QPainterPathStroker ps;
    ps.setCapStyle(pen().capStyle());

    const qreal penWidthZero = qreal(0.00000001);
    const qreal width = pen().widthF();

    ps.setWidth(width <= 0.0 ? penWidthZero : width);
    ps.setJoinStyle(pen().joinStyle());
    ps.setMiterLimit(pen().miterLimit());

    QPainterPath p = ps.createStroke(path);
    p.addPath(path);
    return p;
}

int GraphicsArrowItem::type() const
{
    return Type;
}

QLineF GraphicsArrowItem::leftLine() const
{
    QLineF newLine(line().p2(), line().p1());
    newLine.setAngle(newLine.angle() - altAngle());
    newLine.setLength(newLine.length() * altLength());
    return newLine;
}

QLineF GraphicsArrowItem::rightLine() const
{
    QLineF newLine(line().p2(), line().p1());
    newLine.setAngle(newLine.angle() + altAngle());
    newLine.setLength(newLine.length() * altLength());
    return newLine;
}
