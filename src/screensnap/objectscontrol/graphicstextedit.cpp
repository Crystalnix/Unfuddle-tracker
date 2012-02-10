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

#include "graphicstextedit.h"

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QTextEdit>
#include <QPainter>

GraphicsTextEdit::GraphicsTextEdit(QGraphicsScene *scene,
                                   QGraphicsItem *parent)
    : QGraphicsProxyWidget(parent), m_resize(false)
{
    setWidget(new QTextEdit);
    scene->addItem(this);
    QTextEdit * te = textEdit();
    te->setMinimumSize(1, 1);
    te->setWordWrapMode(QTextOption::NoWrap);
    te->setFrameStyle(QFrame::Panel | QFrame::Plain);
    te->setContextMenuPolicy(Qt::NoContextMenu);
    te->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    te->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    connect(te, SIGNAL(textChanged()), this, SLOT(autoResize()));
}

QTextEdit * GraphicsTextEdit::textEdit()
{
    return static_cast<QTextEdit *>(widget());
}

QRectF GraphicsTextEdit::boundingRect() const
{
    return shape().boundingRect();
}

QPainterPath GraphicsTextEdit::shape() const
{
    QPainterPath path;
    path.addRect(QGraphicsProxyWidget::boundingRect());
    path.addRect(resizeRegion());
    return path;
}

void GraphicsTextEdit::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QGraphicsProxyWidget::paint(painter, option, widget);

    painter->setPen(QPen());
    painter->setBrush(QBrush(Qt::white));
    painter->drawEllipse(resizeRegion());
}

void GraphicsTextEdit::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    const QPointF &firstPoint = event->scenePos();
    if (resizeRegion().contains(mapFromScene(firstPoint))) {
        m_resize = true;
        event->accept();
    } else {
        QGraphicsProxyWidget::mousePressEvent(event);
    }
}

void GraphicsTextEdit::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_resize) {
        m_resize = false;
        event->accept();
    } else {
        QGraphicsProxyWidget::mouseReleaseEvent(event);
    }
}

void GraphicsTextEdit::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_resize) {
        setGeometry(QRectF(geometry().topLeft(), event->scenePos()));
        event->accept();
    } else {
        QGraphicsProxyWidget::mouseMoveEvent(event);
    }
}

QRectF GraphicsTextEdit::resizeRegion() const
{
    const qreal a = 8.0;
    const qreal ad2 = a / 2;
    const QPointF m(ad2, ad2);
    const QPointF &p = QGraphicsProxyWidget::boundingRect().bottomRight();
    return QRectF(p - m, p + m);
}

void GraphicsTextEdit::autoResize()
{
    QTextDocument *document = textEdit()->document();
    const QSizeF &cSize = size();
    const QSizeF &dSize = document->size();
    resize(qMax(cSize.width(), dSize.width() + 2),
           qMax(cSize.height(), dSize.height() + 2));
}
