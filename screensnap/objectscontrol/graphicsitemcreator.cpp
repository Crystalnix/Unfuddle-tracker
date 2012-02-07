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

#include "graphicsitemcreator.h"

#include <QGraphicsItem>
#include <QGraphicsProxyWidget>
#include <QTextEdit>
#include <QGraphicsSceneMouseEvent>
#include <screensnap/primitives.h>
#include <QDebug>

GraphicsLineItemCreator::GraphicsLineItemCreator(QGraphicsScene *scene)
    : GraphicsItemCreator(scene)
{
}

bool GraphicsLineItemCreator::eventFilter(QObject *watched, QEvent *event)
{
    Q_UNUSED(watched);

    switch (event->type()) {
    case QEvent::GraphicsSceneMouseDoubleClick:
    case QEvent::GraphicsSceneMousePress: {
        QGraphicsSceneMouseEvent *mouseEvent = static_cast<QGraphicsSceneMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            const QPointF &firstPoint = mouseEvent->scenePos();
            p_line.reset(new QGraphicsLineItem(QLineF(firstPoint, firstPoint), 0, scene()));
            return true;
        } else {
            break;
        }
    }
    case QEvent::GraphicsSceneMouseMove: {
        QGraphicsSceneMouseEvent *mouseEvent = static_cast<QGraphicsSceneMouseEvent *>(event);
        if (mouseEvent->buttons() == Qt::LeftButton) {
            updatePoints(mouseEvent->buttonDownScenePos(Qt::LeftButton), mouseEvent->scenePos());
            return true;
        } else {
            break;
        }
    }
    case QEvent::GraphicsSceneMouseRelease: {
        QGraphicsSceneMouseEvent *mouseEvent = static_cast<QGraphicsSceneMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            const QPointF &firstPoint = mouseEvent->buttonDownScenePos(Qt::LeftButton);
            const QPointF &lastPoint = mouseEvent->scenePos();
            updatePoints(firstPoint, lastPoint);
            scene()->removeItem(p_line.data());
            if (firstPoint != lastPoint) {
                emit itemCreated(p_line.take());
            }
            return true;
        } else {
            break;
        }
    }
    default:
        break;
    }

    return false;
}

void GraphicsLineItemCreator::updatePoints(const QPointF &firstPoint, const QPointF &point)
{
    p_line->setLine(QLineF(firstPoint, point));
}

GraphicsRectItemCreator::GraphicsRectItemCreator(QGraphicsScene *scene)
    : GraphicsItemCreator(scene)
{
}

bool GraphicsRectItemCreator::eventFilter(QObject *watched, QEvent *event)
{
    Q_UNUSED(watched);

    switch (event->type()) {
    case QEvent::GraphicsSceneMouseDoubleClick:
    case QEvent::GraphicsSceneMousePress: {
        QGraphicsSceneMouseEvent *mouseEvent = static_cast<QGraphicsSceneMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            const QPointF &firstPoint = mouseEvent->scenePos();
            p_rect.reset(new QGraphicsRectItem(QRectF(firstPoint, firstPoint), 0, scene()));
            return true;
        } else {
            break;
        }
    }
    case QEvent::GraphicsSceneMouseMove: {
        QGraphicsSceneMouseEvent *mouseEvent = static_cast<QGraphicsSceneMouseEvent *>(event);
        if (mouseEvent->buttons() == Qt::LeftButton) {
            updatePoints(mouseEvent->buttonDownScenePos(Qt::LeftButton), mouseEvent->scenePos());
            return true;
        } else {
            break;
        }
    }
    case QEvent::GraphicsSceneMouseRelease: {
        QGraphicsSceneMouseEvent *mouseEvent = static_cast<QGraphicsSceneMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            const QPointF &firstPoint = mouseEvent->buttonDownScenePos(Qt::LeftButton);
            const QPointF &lastPoint = mouseEvent->scenePos();
            updatePoints(firstPoint, lastPoint);
            scene()->removeItem(p_rect.data());
            if (firstPoint != lastPoint) {
                emit itemCreated(p_rect.take());
            }
            return true;
        } else {
            break;
        }
    }
    default:
        break;
    }

    return false;
}

void GraphicsRectItemCreator::updatePoints(const QPointF &firstPoint, const QPointF &point)
{
    qreal x1 = firstPoint.x();
    qreal x2 = point.x();

    if (x1 > x2) {
        qSwap(x1, x2);
    }

    qreal y1 = firstPoint.y();
    qreal y2 = point.y();

    if (y1 > y2) {
        qSwap(y1, y2);
    }

    p_rect->setRect(x1, y1, x2 - x1, y2 - y1);
}

GraphicsBrushItemCreator::GraphicsBrushItemCreator(QGraphicsScene *scene)
    : GraphicsItemCreator(scene)
{
}

bool GraphicsBrushItemCreator::eventFilter(QObject *watched, QEvent *event)
{
    Q_UNUSED(watched);

    switch (event->type()) {
    case QEvent::GraphicsSceneMouseDoubleClick:
    case QEvent::GraphicsSceneMousePress: {
        QGraphicsSceneMouseEvent *mouseEvent = static_cast<QGraphicsSceneMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            p_brushItem.reset(new GraphicsBrushItem(mouseEvent->scenePos(), 0, scene()));
            return true;
        } else {
            break;
        }
    }
    case QEvent::GraphicsSceneMouseMove: {
        QGraphicsSceneMouseEvent *mouseEvent = static_cast<QGraphicsSceneMouseEvent *>(event);
        if (mouseEvent->buttons() == Qt::LeftButton) {
            updatePoints(mouseEvent->scenePos());
            return true;
        } else {
            break;
        }
    }
    case QEvent::GraphicsSceneMouseRelease: {
        QGraphicsSceneMouseEvent *mouseEvent = static_cast<QGraphicsSceneMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            updatePoints(mouseEvent->scenePos());
            scene()->removeItem(p_brushItem.data());
            emit itemCreated(p_brushItem.take());
            return true;
        } else {
            break;
        }
    }
    default:
        break;
    }

    return false;
}

void GraphicsBrushItemCreator::updatePoints(const QPointF &point)
{
    p_brushItem->addPoint(point);
}

GraphicsArrowItemCreator::GraphicsArrowItemCreator(QGraphicsScene *scene)
    : GraphicsItemCreator(scene)
{
}

bool GraphicsArrowItemCreator::eventFilter(QObject *watched, QEvent *event)
{
    Q_UNUSED(watched);

    switch (event->type()) {
    case QEvent::GraphicsSceneMouseDoubleClick:
    case QEvent::GraphicsSceneMousePress: {
        QGraphicsSceneMouseEvent *mouseEvent = static_cast<QGraphicsSceneMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            const QPointF &firstPoint = mouseEvent->scenePos();
            p_arrow.reset(new GraphicsArrowItem(QLineF(firstPoint, firstPoint), 30, 0.2, 0, scene()));
            return true;
        } else {
            break;
        }
    }
    case QEvent::GraphicsSceneMouseMove: {
        QGraphicsSceneMouseEvent *mouseEvent = static_cast<QGraphicsSceneMouseEvent *>(event);
        if (mouseEvent->buttons() == Qt::LeftButton) {
            updatePoints(mouseEvent->buttonDownScenePos(Qt::LeftButton), mouseEvent->scenePos());
            return true;
        } else {
            break;
        }
    }
    case QEvent::GraphicsSceneMouseRelease: {
        QGraphicsSceneMouseEvent *mouseEvent = static_cast<QGraphicsSceneMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            const QPointF &firstPoint = mouseEvent->buttonDownScenePos(Qt::LeftButton);
            const QPointF &lastPoint = mouseEvent->scenePos();
            updatePoints(firstPoint, lastPoint);
            scene()->removeItem(p_arrow.data());
            if (firstPoint != lastPoint) {
                emit itemCreated(p_arrow.take());
            }
            return true;
        } else {
            break;
        }
    }
    default:
        break;
    }

    return false;
}

void GraphicsArrowItemCreator::updatePoints(const QPointF &firstPoint, const QPointF &point)
{
    p_arrow->setLine(QLineF(firstPoint, point));
}

GraphicsTextItemCreator::GraphicsTextItemCreator(const QFont &font, QGraphicsScene *scene)
    : GraphicsItemCreator(scene),
      p_proxy(new GraphicsTextEdit(scene)), resizing(false)
{
    p_proxy->textEdit()->setFont(font);
    p_proxy->textEdit()->installEventFilter(this);
    p_proxy->hide();
}

GraphicsTextItemCreator::~GraphicsTextItemCreator()
{
    if (!p_proxy->textEdit()->toPlainText().isEmpty()) {
        createTextItem();
    }
}

bool GraphicsTextItemCreator::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == p_proxy->textEdit()) {
        switch (event->type()) {
        case QEvent::KeyPress: {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->key() == Qt::Key_Return
                    && keyEvent->modifiers() & Qt::ControlModifier)
            {
                createTextItem();
                return true;
            }
        }
        case QEvent::FocusOut:
        {
            QFocusEvent *focusEvent = static_cast<QFocusEvent *>(event);
            if (focusEvent->lostFocus()) {
                if (!p_proxy->textEdit()->toPlainText().isEmpty()) {
                    createTextItem();
                    return true;
                } else {
                    hideElems();
                }
            }
        }
        default: {
            return false;
        }
        }
    }

    if (watched != scene()) {
        return false;
    }

    switch (event->type()) {
    case QEvent::GraphicsSceneMouseDoubleClick:
    case QEvent::GraphicsSceneMousePress: {
        QGraphicsSceneMouseEvent *mouseEvent = static_cast<QGraphicsSceneMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton
                && !scene()->items(mouseEvent->scenePos()).contains(p_proxy.data()))
        {
            p_proxy->clearFocus();
            const QPointF &firstPoint = mouseEvent->scenePos();
            p_proxy->setPos(firstPoint);
            p_proxy->resize(0, 0);
            updatePoints(firstPoint, firstPoint);
            showElems();
            resizing = true;
            return true;
        } else {
            break;
        }
    }
    case QEvent::GraphicsSceneMouseMove: {
        QGraphicsSceneMouseEvent *mouseEvent = static_cast<QGraphicsSceneMouseEvent *>(event);
        if (mouseEvent->buttons() == Qt::LeftButton && resizing) {
            updatePoints(mouseEvent->buttonDownScenePos(Qt::LeftButton), mouseEvent->scenePos());
            return true;
        } else if (!scene()->items(mouseEvent->scenePos()).contains(p_proxy.data())) {
            return true;
        } else {
            break;
        }
    }
    case QEvent::GraphicsSceneMouseRelease: {
        QGraphicsSceneMouseEvent *mouseEvent = static_cast<QGraphicsSceneMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton && resizing) {
            updatePoints(mouseEvent->buttonDownScenePos(Qt::LeftButton), mouseEvent->scenePos());
            p_proxy->textEdit()->setFocus();
            resizing = false;
            return true;
        } else {
            break;
        }
    }
    default:
        break;
    }

    return false;
}

void GraphicsTextItemCreator::updatePoints(const QPointF &firstPoint, const QPointF &point)
{
    p_proxy->textEdit()->resize(point.x() - firstPoint.x(),
                                point.y() - firstPoint.y());
}

void GraphicsTextItemCreator::showElems()
{
    p_proxy->show();
}

void GraphicsTextItemCreator::hideElems()
{
    p_proxy->hide();
}

void GraphicsTextItemCreator::createTextItem()
{
    QScopedPointer<QGraphicsTextItem> item(new QGraphicsTextItem);
    item->setDocument(p_proxy->textEdit()->document()->clone(item.data()));
    item->setPos(p_proxy->pos());
    item->document()->setPageSize(item->document()->pageSize());
    p_proxy->textEdit()->clear();
    hideElems();
    emit itemCreated(item.take());
}
