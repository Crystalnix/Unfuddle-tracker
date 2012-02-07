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

#ifndef GRAPHICSTEXTEDIT_H
#define GRAPHICSTEXTEDIT_H

#include <QGraphicsProxyWidget>

class QTextEdit;

class GraphicsTextEdit : public QGraphicsProxyWidget
{
    Q_OBJECT

public:
    explicit GraphicsTextEdit(QGraphicsScene *scene, QGraphicsItem *parent = 0);
    QTextEdit * textEdit();

    QRectF boundingRect() const;
    QPainterPath shape() const;

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

private:
    bool m_resize;
    QRectF resizeRegion() const;

private slots:
    void autoResize();
};

#endif // GRAPHICSTEXTEDIT_H
