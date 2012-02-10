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

#include "graphicsutility.h"

#include <screensnap/urcommands/urcommands.h>
#include <screensnap/objectscontrol/graphicsitemeditor.h>
#include <screensnap/metatypetextdocument.h>

#include <QTextEdit>
#include <QGraphicsRectItem>
#include <QMouseEvent>
#include <QGraphicsSceneMouseEvent>

CropUtility::CropUtility(QGraphicsScene *scene, QGraphicsView *view)
    : GraphicsUtility(scene), p_view(view)
{
}

bool CropUtility::eventFilter(QObject *watched, QEvent *event)
{
    Q_UNUSED(watched);

    switch (event->type()) {
    case QEvent::GraphicsSceneMouseDoubleClick:
    case QEvent::GraphicsSceneMousePress: {
        QGraphicsSceneMouseEvent *mouseEvent = static_cast<QGraphicsSceneMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            const QPointF &firstPoint = mouseEvent->scenePos();
            m_rect.setRect(QRectF(firstPoint, firstPoint));
            scene()->addItem(&m_rect);
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
            const QPointF &point = mouseEvent->scenePos();
            updatePoints(firstPoint, point);
            scene()->removeItem(&m_rect);
            if (firstPoint != point) {
                emit utilityFinished(new CropCommand(m_rect.rect(), scene(), p_view));
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

void CropUtility::updatePoints(const QPointF &firstPoint, const QPointF &point)
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

    m_rect.setRect(x1, y1, x2 - x1, y2 - y1);
}

InteractiveTextEditUtility::InteractiveTextEditUtility(QGraphicsTextItem *textItem,
                                                       QGraphicsScene *scene,
                                                       GraphicsItemEditor *editor)
    : GraphicsUtility(scene), p_textItem(textItem), p_editor(editor),
      p_proxy(new GraphicsTextEdit(scene))
{
    p_proxy->setPos(p_textItem->pos());
    p_proxy->resize(p_textItem->boundingRect().size() + QSizeF(2, 2));
    p_textItem->hide();

    QTextEdit *te = p_proxy->textEdit();
    te->setDocument(p_textItem->document()->clone(p_textItem));
    te->setFocus();
}

InteractiveTextEditUtility::~InteractiveTextEditUtility()
{
    p_textItem->show();
}

bool InteractiveTextEditUtility::eventFilter(QObject *watched, QEvent *event)
{
    Q_UNUSED(watched);

    switch (event->type()) {
    case QEvent::GraphicsSceneMousePress: {
        QGraphicsSceneMouseEvent *mouseEvent = static_cast<QGraphicsSceneMouseEvent *>(event);
        const QPointF &point = mouseEvent->scenePos();
        if (!p_proxy->boundingRect().contains(p_proxy->mapFromScene(point))) {
            emit utilityFinished(p_editor->setPropertyToObject("document", qVariantFromValue(p_proxy->textEdit()->document()), p_textItem));
            emit utilityFinished(NULL);
            return false;
        } else {
            event->ignore();
        }
        break;
    }
    default:
        break;
    }

    return false;
}
