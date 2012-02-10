/*
*    Unfuddle Tracker is a time tracking tool for Unfuddle service.
*    Copyright (C) 2012	Viatcheslav Gachkaylo <vgachkaylo@crystalnix.com>
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

#ifndef UNFUDDLEDATA_H
#define UNFUDDLEDATA_H

#include <QString>
#include <QList>
#include <QDate>

class QDomElement;

namespace unf {

struct Account {
    int id;

    struct {
        bool attachments;
        bool time_tracking;
    } features;

    Account() : id(0) { features.attachments = false; features.time_tracking = false; }

    static Account loadFromDom(const QDomElement &rootEl);
    //static QList<Account> loadListFromDom(const QDomElement &rootEl);
};

struct Person {
    int id;
    QString first_name;
    QString last_name;
    QString username;

    Person() : id(0) {}

    static Person loadFromDom(const QDomElement &rootEl);
    static QList<Person> loadListFromDom(const QDomElement &rootEl);
};

struct Involvement {
    int id;
    int person_id;
    int project_id;

    Involvement() : id(0), person_id(0), project_id(0) {}

    static Involvement loadFromDom(const QDomElement &rootEl);
    static QList<Involvement> loadListFromDom(const QDomElement &rootEl);
};

struct Project {
    int id;
    bool archived;
    bool enable_time_tracking;
    QString title;

    Project() : id(0), archived(false), enable_time_tracking(false) {}

    static Project loadFromDom(const QDomElement &rootEl);
    static QList<Project> loadListFromDom(const QDomElement &rootEl);
    static QList<Project> filterProjectListWithInvolvements(const QList<Project> &projects,
                                                           const QList<Involvement> &involvements);
};

struct Milestone {
    int id;
    QString title;

    Milestone() : id(0) {}

    static Milestone loadFromDom(const QDomElement &rootEl);
    static QList<Milestone> loadListFromDom(const QDomElement &rootEl);
};

struct Ticket {
    int id;
    int assignee_id;
    QString description;
    int number;
    QString resolution;
    QString status;
    QString summary;

    Ticket() : id(0), assignee_id(0), number(0) {}

    QString toXml() const;
    static Ticket loadFromDom(const QDomElement &rootEl);
    static QList<Ticket> loadListFromDom(const QDomElement &rootEl);
};

struct TimeEntry {
    int id;
    QDate date;
    QString description;
    float hours;
    int person_id;

    TimeEntry() : id(0), hours(0.0), person_id(0) {}

    QString toXml() const;
    static TimeEntry loadFromDom(const QDomElement &rootEl);
    static QList<TimeEntry> loadListFromDom(const QDomElement &rootEl);
};

struct Attachment;

struct Comment {
    int id;
    int author_id;
    QString body;
    QDateTime created_at;
    QDateTime updated_at;

    Comment() : id(0), author_id(0) {}

    QList<Attachment> attachments;

    QString toXml() const;
    static Comment loadFromDom(const QDomElement &rootEl);
    static QList<Comment> loadListFromDom(const QDomElement &rootEl);
};

struct Attachment {
    int id;
    QString filename;
    QString content_type;
    QString upload_key;

    Attachment() : id(0) {}

    QString toXml() const;
    static Attachment loadFromDom(const QDomElement &rootEl);
    static QList<Attachment> loadListFromDom(const QDomElement &rootEl);
};

}

#endif // UNFUDDLEDATA_H
