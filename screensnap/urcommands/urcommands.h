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

#ifndef URCOMMANDS_H
#define URCOMMANDS_H

#include <QUndoCommand>
#include <QSharedPointer>
#include <QRectF>
#include <screensnap/property.h>

class QGraphicsScene;
class QGraphicsView;
class QGraphicsItem;

class AddCommand : public QUndoCommand
{
public:
    explicit AddCommand(QGraphicsScene *scene, QGraphicsItem *item, QUndoCommand *parent = 0);
    ~AddCommand();

    void undo();
    void redo();

private:
    QGraphicsScene *p_scene;
    QGraphicsItem *p_item;
    bool isAdded;
};

class DelCommand : public QUndoCommand
{
public:
    explicit DelCommand(QGraphicsItem *item, QUndoCommand *parent = 0);
    ~DelCommand();

    void undo();
    void redo();

private:
    QGraphicsScene *p_scene;
    QGraphicsItem *p_item;
    bool isDeleted;
};

class PropertyCommand : public QUndoCommand
{
public:
    explicit PropertyCommand(QGraphicsItem *object,
                             const QVariant &newValue,
                             const QSharedPointer<Property> &property,
                             QUndoCommand *parent = 0)
        : QUndoCommand(parent), p_object(object), m_value(newValue),
          p_property(property) {}

    void undo() { swapValues(); }
    void redo() { swapValues(); }

private:
    QGraphicsItem *p_object;
    QVariant m_value;
    QSharedPointer<Property> p_property;

    void swapValues()
    {
        const QVariant &tmp = p_property->get(p_object);
        p_property->set(p_object, m_value);
        m_value = tmp;
    }
};

PropertyCommand *
makePropertyCommand(QGraphicsItem *object, const QVariant &newValue,
                    const QSharedPointer<Property> &property, QUndoCommand *parent = 0);

class CropCommand : public QUndoCommand
{
public:
    explicit CropCommand(const QRectF &rect,
                         QGraphicsScene *scene,
                         QGraphicsView *view,
                         QUndoCommand *parent = 0)
        : QUndoCommand(parent), p_scene(scene), p_view(view), m_rect(rect) {}

    void undo() { swapRects(); }
    void redo() { swapRects(); }

private:
    QGraphicsScene *p_scene;
    QGraphicsView *p_view;
    QRectF m_rect;

    void swapRects();
};

#endif // URCOMMANDS_H
