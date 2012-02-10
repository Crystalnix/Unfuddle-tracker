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

#ifndef GRAPHICSITEMEDITOR_H
#define GRAPHICSITEMEDITOR_H

#include <QGraphicsItem>
#include <screensnap/urcommands/urcommands.h>

typedef QList<QSharedPointer<Property> > PropertyList;
typedef QMap<QString, QSharedPointer<Property> > PropertyMap;

class GraphicsUtility;

class GraphicsItemEditor
{
public:
    GraphicsItemEditor();

    virtual ~GraphicsItemEditor() {}

    QUndoCommand * setPropertyToObject(const QString &name, const QVariant &value, QGraphicsItem *item);
    PropertyList properties();

    virtual GraphicsUtility *interactiveEditUtility(QGraphicsItem *item, QGraphicsScene *scene)
    {
        Q_UNUSED(item)
        Q_UNUSED(scene)

        return NULL;
    }

protected:
    template <typename Object_type, typename Property_type,
              typename Write_fun, typename Read_fun>
    void installProperty(const QString &name, Write_fun writeFun, Read_fun readFun)
    {
        m_properties[name] = QSharedPointer<Property>(
                    makeProperty<Object_type, Property_type>(name, writeFun, readFun));
    }

private:
    PropertyMap m_properties;
};

class GraphicsLineItemEditor : public GraphicsItemEditor
{
public:
    GraphicsLineItemEditor();
};

class AbstractGraphicsShapeItemEditor : public GraphicsItemEditor
{
public:
    AbstractGraphicsShapeItemEditor();
};

class GraphicsRectItemEditor : public AbstractGraphicsShapeItemEditor
{
public:
    GraphicsRectItemEditor();
};

class GraphicsBrushItemEditor : public GraphicsItemEditor
{
public:
    GraphicsBrushItemEditor();
};

class GraphicsTextItemEditor : public GraphicsItemEditor
{
public:
    GraphicsTextItemEditor();

    GraphicsUtility *interactiveEditUtility(QGraphicsItem *item, QGraphicsScene *scene);
};

#endif // GRAPHICSITEMEDITOR_H
