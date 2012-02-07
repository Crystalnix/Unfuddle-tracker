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

#include "urcommands.h"

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>

AddCommand::AddCommand(QGraphicsScene *scene, QGraphicsItem *item, QUndoCommand *parent)
    : QUndoCommand(parent), p_scene(scene), p_item(item), isAdded(false)
{
}

AddCommand::~AddCommand()
{
    if (!isAdded) {
        delete p_item;
    }
}

void AddCommand::undo()
{
    p_scene->removeItem(p_item);
    p_scene->update(p_item->boundingRect());
    isAdded = false;
}

void AddCommand::redo()
{
    p_scene->addItem(p_item);
    p_scene->update(p_item->boundingRect());
    isAdded = true;
}

DelCommand::DelCommand(QGraphicsItem *item, QUndoCommand *parent)
    : QUndoCommand(parent), p_scene(item->scene()), p_item(item), isDeleted(false)
{
}

DelCommand::~DelCommand()
{
    if (isDeleted) {
        delete p_item;
    }
}

void DelCommand::undo()
{
    p_scene->addItem(p_item);
    p_scene->update(p_item->boundingRect());
    isDeleted = false;
}

void DelCommand::redo()
{
    p_scene->removeItem(p_item);
    p_scene->update(p_item->boundingRect());
    isDeleted = true;
}

PropertyCommand *
makePropertyCommand(QGraphicsItem *object, const QVariant &newValue,
                    const QSharedPointer<Property> &property, QUndoCommand *parent)
{
    return new PropertyCommand(object, newValue, property, parent);
}

void CropCommand::swapRects()
{
    const QRectF &tmp = p_scene->sceneRect();
    p_scene->setSceneRect(m_rect);
    const QTransform &transform = p_view->transform();
    const QSizeF &size = m_rect.size();
    p_view->resize(qRound(transform.m11() * size.width()) + 2,
                   qRound(transform.m22() * size.height()) + 2);
    m_rect = tmp;
}
