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

#ifndef PROPERTY_H
#define PROPERTY_H

#include <QString>
#include <QVariant>

class QGraphicsItem;

class Property
{
public:
    explicit Property(const QString &name) : m_name(name) {}
    virtual ~Property() {}

    QString name() const { return m_name; }
    virtual QVariant get(const QGraphicsItem *item) const = 0;
    virtual void set(QGraphicsItem *item, const QVariant &value) const = 0;

private:
    const QString m_name;
};

template <typename Object_type, typename Property_type,
          typename Write_fun, typename Read_fun>
class PropertyTemplate : public Property
{
public:
    explicit PropertyTemplate(const QString &name,
                              Write_fun writeFun,
                              Read_fun readFun)
        : Property(name), f_write(writeFun), f_read(readFun) {}

    QVariant get(const QGraphicsItem *item) const
    {
        const Object_type *object = dynamic_cast<const Object_type *>(item);

        Q_ASSERT(object != NULL);

        return qVariantFromValue(f_read(object));
    }

    void set(QGraphicsItem *item, const QVariant &value) const
    {
        Object_type *object = dynamic_cast<Object_type *>(item);

        Q_ASSERT(object != NULL);

        f_write(object, qVariantValue<Property_type>(value));
    }

private:
    Write_fun f_write;
    Read_fun f_read;
};

template <typename Object_type, typename Property_type,
          typename Write_fun, typename Read_fun>
Property * makeProperty(const QString &name, Write_fun writeFun, Read_fun readFun)
{
    return new PropertyTemplate<Object_type, Property_type, Write_fun, Read_fun>
            (name, writeFun, readFun);
}

#endif // PROPERTY_H
