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

#include "graphicsitemeditor.h"

#include <screensnap/primitives.h>
#include <screensnap/objectscontrol/graphicsutility.h>
#include <screensnap/metatypetextdocument.h>

QUndoCommand * GraphicsItemEditor::setPropertyToObject(const QString &name,
                                                       const QVariant &value,
                                                       QGraphicsItem *item)
{
    PropertyMap::iterator iter = m_properties.find(name);

    return (iter != m_properties.end())
            ? makePropertyCommand(item, value, iter.value())
            : NULL;
}

PropertyList GraphicsItemEditor::properties()
{
    return m_properties.values();
}

GraphicsItemEditor::GraphicsItemEditor()
{
    installProperty<QGraphicsItem, QPointF>
            ("position",
             std::mem_fun(&QGraphicsItem::setPos),
             std::mem_fun(&QGraphicsItem::pos));
}

GraphicsLineItemEditor::GraphicsLineItemEditor()
{
    installProperty<QGraphicsLineItem, QPen>
            ("pen",
             std::mem_fun(&QGraphicsLineItem::setPen),
             std::mem_fun(&QGraphicsLineItem::pen));

    installProperty<QGraphicsLineItem, QLineF>
            ("line",
             std::mem_fun(&QGraphicsLineItem::setLine),
             std::mem_fun(&QGraphicsLineItem::line));
}

AbstractGraphicsShapeItemEditor::AbstractGraphicsShapeItemEditor()
{
    installProperty<QAbstractGraphicsShapeItem, QPen>
            ("pen",
             std::mem_fun(&QAbstractGraphicsShapeItem::setPen),
             std::mem_fun(&QAbstractGraphicsShapeItem::pen));

    installProperty<QAbstractGraphicsShapeItem, QBrush>
            ("brush",
             std::mem_fun(&QAbstractGraphicsShapeItem::setBrush),
             std::mem_fun(&QAbstractGraphicsShapeItem::brush));
}

GraphicsRectItemEditor::GraphicsRectItemEditor()
{
    installProperty<QGraphicsRectItem, QRectF>
            ("rect",
             std::mem_fun(&QGraphicsRectItem::setRect),
             std::mem_fun(&QGraphicsRectItem::rect));
}

GraphicsBrushItemEditor::GraphicsBrushItemEditor()
{
    installProperty<GraphicsBrushItem, QPen>
            ("pen",
             std::mem_fun(&GraphicsBrushItem::setPen),
             std::mem_fun(&GraphicsBrushItem::pen));
}

struct setDocumentWithUpdateScene
{
    void operator() (QGraphicsTextItem *item,
                     QTextDocument *doc) const
    {
        item->setDocument(doc);
        if (QGraphicsScene *scene = item->scene()) {
            scene->update(item->mapRectToScene(item->boundingRect()));
        }
    }
};

GraphicsTextItemEditor::GraphicsTextItemEditor()
{
    installProperty<QGraphicsTextItem, QFont>
            ("font",
             std::mem_fun(&QGraphicsTextItem::setFont),
             std::mem_fun(&QGraphicsTextItem::font));

    installProperty<QGraphicsTextItem, QColor>
            ("font_color",
             std::mem_fun(&QGraphicsTextItem::setDefaultTextColor),
             std::mem_fun(&QGraphicsTextItem::defaultTextColor));

    installProperty<QGraphicsTextItem, QTextDocument *>
            ("document",
             setDocumentWithUpdateScene(),
             std::mem_fun(&QGraphicsTextItem::document));
}

GraphicsUtility * GraphicsTextItemEditor::interactiveEditUtility(QGraphicsItem *item, QGraphicsScene *scene)
{
    return new InteractiveTextEditUtility(static_cast<QGraphicsTextItem *>(item), scene, this);
}
