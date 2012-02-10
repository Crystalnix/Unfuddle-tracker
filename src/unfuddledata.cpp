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

#include "unfuddledata.h"
#include <QDomElement>
#include <QSet>

namespace unf {

// **** Account ****
Account Account::loadFromDom(const QDomElement &rootEl)
{
    Account res;

    QDomNodeList dnlist = rootEl.elementsByTagName("id");
    if (dnlist.length() == 1)
        res.id = dnlist.item(0).firstChild().nodeValue().toInt();
    dnlist = rootEl.elementsByTagName("attachments");
    if (dnlist.length() == 1)
        res.features.attachments = dnlist.item(0).firstChild().nodeValue() == "true";
    dnlist = rootEl.elementsByTagName("time_tracking");
    if (dnlist.length() == 1)
        res.features.time_tracking = dnlist.item(0).firstChild().nodeValue() == "true";

    return res;
}

// **** Person ****
Person Person::loadFromDom(const QDomElement &rootEl)
{
    Person res;

    QDomNodeList dnlist = rootEl.elementsByTagName("id");
    if (dnlist.length() == 1)
        res.id = dnlist.item(0).firstChild().nodeValue().toInt();
    dnlist = rootEl.elementsByTagName("username");
    if (dnlist.length() == 1)
        res.username = dnlist.item(0).firstChild().nodeValue();
    dnlist = rootEl.elementsByTagName("first-name");
    if (dnlist.length() == 1)
        res.first_name = dnlist.item(0).firstChild().nodeValue();
    dnlist = rootEl.elementsByTagName("last-name");
    if (dnlist.length() == 1)
        res.last_name = dnlist.item(0).firstChild().nodeValue();

    return res;
}

QList<Person> Person::loadListFromDom(const QDomElement &rootEl)
{
    QList<Person> res;

    QDomNodeList dnlist = rootEl.elementsByTagName("person");
    for (uint i = 0; i < dnlist.length(); ++i) {
        res.push_back(Person::loadFromDom(dnlist.item(i).toElement()));
    }

    return res;
}

// **** Involvement ****
Involvement Involvement::loadFromDom(const QDomElement &rootEl)
{
    Involvement res;

    QDomNodeList dnlist = rootEl.elementsByTagName("id");
    if (dnlist.length() == 1)
        res.id = dnlist.item(0).firstChild().nodeValue().toInt();
    dnlist = rootEl.elementsByTagName("person-id");
    if (dnlist.length() == 1)
        res.person_id = dnlist.item(0).firstChild().nodeValue().toInt();
    dnlist = rootEl.elementsByTagName("project-id");
    if (dnlist.length() == 1)
        res.project_id = dnlist.item(0).firstChild().nodeValue().toInt();

    return res;
}

QList<Involvement> Involvement::loadListFromDom(const QDomElement &rootEl)
{
    QList<Involvement> res;

    QDomNodeList dnlist = rootEl.elementsByTagName("involvement");
    for (uint i = 0; i < dnlist.length(); ++i) {
        res.push_back(Involvement::loadFromDom(dnlist.item(i).toElement()));
    }

    return res;
}

// **** Project ****
Project Project::loadFromDom(const QDomElement &rootEl)
{
    Project res;

    QDomNodeList dnlist = rootEl.elementsByTagName("id");
    if (dnlist.length() == 1)
        res.id = dnlist.item(0).firstChild().nodeValue().toInt();
    dnlist = rootEl.elementsByTagName("archived");
    if (dnlist.length() == 1)
        res.archived = dnlist.item(0).firstChild().nodeValue() == "true";
    dnlist = rootEl.elementsByTagName("enable-time-tracking");
    if (dnlist.length() == 1)
        res.enable_time_tracking = dnlist.item(0).firstChild().nodeValue() == "true";
    dnlist = rootEl.elementsByTagName("title");
    if (dnlist.length() == 1)
        res.title = dnlist.item(0).firstChild().nodeValue();

    return res;
}

QList<Project> Project::loadListFromDom(const QDomElement &rootEl)
{
    QList<Project> res;

    QDomNodeList dnlist = rootEl.elementsByTagName("project");
    for (uint i = 0; i < dnlist.length(); ++i) {
        res.push_back(Project::loadFromDom(dnlist.item(i).toElement()));
    }

    return res;
}

QList<Project> Project::filterProjectListWithInvolvements(
        const QList<Project> &projects, const QList<Involvement> &involvements)
{
    QSet<int> project_ids;
    foreach (Involvement i, involvements)
        project_ids.insert(i.project_id);

    QList<Project> res;
    foreach (Project p, projects)
        if (project_ids.contains(p.id))
            res.push_back(p);

    return res;
}

// **** Milestone ****
Milestone Milestone::loadFromDom(const QDomElement &rootEl)
{
    Milestone res;

    QDomNodeList dnlist = rootEl.elementsByTagName("id");
    if (dnlist.length() == 1)
        res.id = dnlist.item(0).firstChild().nodeValue().toInt();
    dnlist = rootEl.elementsByTagName("title");
    if (dnlist.length() == 1)
        res.title = dnlist.item(0).firstChild().nodeValue();

    return res;
}

QList<Milestone> Milestone::loadListFromDom(const QDomElement &rootEl)
{
    QList<Milestone> res;

    QDomNodeList dnlist = rootEl.elementsByTagName("milestone");
    for (uint i = 0; i < dnlist.length(); ++i) {
        res.push_back(Milestone::loadFromDom(dnlist.item(i).toElement()));
    }

    return res;
}

// **** Ticket ****
Ticket Ticket::loadFromDom(const QDomElement &rootEl)
{
    Ticket res;

    QDomNodeList dnlist = rootEl.elementsByTagName("id");
    if (dnlist.length() == 1)
        res.id = dnlist.item(0).firstChild().nodeValue().toInt();
    dnlist = rootEl.elementsByTagName("assignee-id");
    if (dnlist.length() == 1)
        res.assignee_id = dnlist.item(0).firstChild().nodeValue().toInt();
    dnlist = rootEl.elementsByTagName("description");
    if (dnlist.length() == 1)
        res.description = dnlist.item(0).firstChild().nodeValue();
    dnlist = rootEl.elementsByTagName("number");
    if (dnlist.length() == 1)
        res.number = dnlist.item(0).firstChild().nodeValue().toInt();
    dnlist = rootEl.elementsByTagName("resolution");
    if (dnlist.length() == 1)
        res.resolution = dnlist.item(0).firstChild().nodeValue();
    dnlist = rootEl.elementsByTagName("status");
    if (dnlist.length() == 1)
        res.status = dnlist.item(0).firstChild().nodeValue();
    dnlist = rootEl.elementsByTagName("summary");
    if (dnlist.length() == 1)
        res.summary = dnlist.item(0).firstChild().nodeValue();

    return res;
}

QList<Ticket> Ticket::loadListFromDom(const QDomElement &rootEl)
{
    QList<Ticket> res;

    QDomNodeList dnlist = rootEl.elementsByTagName("ticket");
    for (uint i = 0; i < dnlist.length(); ++i) {
        res.push_back(Ticket::loadFromDom(dnlist.item(i).toElement()));
    }

    return res;
}

QString Ticket::toXml() const
{
    return QString("<ticket>"
                   "<status>%1</status>"
                   "<resolution>%2</resolution>"
                   "</ticket>")
            .arg(status, resolution);
}

// **** Time entry ****
TimeEntry TimeEntry::loadFromDom(const QDomElement &rootEl)
{
    TimeEntry res;

    QDomNodeList dnlist = rootEl.elementsByTagName("id");
    if (dnlist.length() == 1)
        res.id = dnlist.item(0).firstChild().nodeValue().toInt();
    dnlist = rootEl.elementsByTagName("date");
    if (dnlist.length() == 1)
        res.date = QDate::fromString(dnlist.item(0).firstChild().nodeValue(), "yyyy-MM-dd");
    dnlist = rootEl.elementsByTagName("description");
    if (dnlist.length() == 1)
        res.description = dnlist.item(0).firstChild().nodeValue();
    dnlist = rootEl.elementsByTagName("hours");
    if (dnlist.length() == 1)
        res.hours = dnlist.item(0).firstChild().nodeValue().toFloat();
    dnlist = rootEl.elementsByTagName("person-id");
    if (dnlist.length() == 1)
        res.person_id = dnlist.item(0).firstChild().nodeValue().toInt();

    return res;
}

QList<TimeEntry> TimeEntry::loadListFromDom(const QDomElement &rootEl)
{
    QList<TimeEntry> res;

    QDomNodeList dnlist = rootEl.elementsByTagName("time-entry");
    for (uint i = 0; i < dnlist.length(); ++i) {
        res.push_back(TimeEntry::loadFromDom(dnlist.item(i).toElement()));
    }

    return res;
}

QString TimeEntry::toXml() const {
    return QString("<time-entry>"
                   "<date type=\"date\">%1</date>"
                   "<hours type=\"float\">%2</hours>"
                   "<description>%3</description>"
                   "<person-id type=\"integer\">%4</person-id>"
                   "</time-entry>")
            .arg(date.toString("yyyy-MM-dd"))
            .arg(hours)
            .arg(description)
            .arg(person_id);
}

// **** Comment ****
Comment Comment::loadFromDom(const QDomElement &rootEl)
{
    Comment res;

    res.id = rootEl.firstChildElement("id").text().toInt();
    res.author_id = rootEl.firstChildElement("author-id").text().toInt();
    res.created_at = QDateTime::fromString(rootEl.firstChildElement("created-at").text(),
                                           "yyyy-MM-ddTHH:mm:ssZ");
    res.updated_at = QDateTime::fromString(rootEl.firstChildElement("updated-at").text(),
                                           "yyyy-MM-ddTHH:mm:ssZ");
    res.body = rootEl.firstChildElement("body").text();

    QDomElement el = rootEl.firstChildElement("attachments");
    if (!el.isNull()) {
        res.attachments = Attachment::loadListFromDom(el);
    }

    return res;
}

QList<Comment> Comment::loadListFromDom(const QDomElement &rootEl)
{
    QList<Comment> res;

    QDomNodeList dnlist = rootEl.elementsByTagName("comment");
    for (uint i = 0; i < dnlist.length(); ++i) {
        res.push_back(Comment::loadFromDom(dnlist.item(i).toElement()));
    }

    return res;
}

QString Comment::toXml() const {
    return QString("<comment><body>%1</body></comment>").arg(body);
}

// **** Attachment ****
Attachment Attachment::loadFromDom(const QDomElement &rootEl)
{
    Attachment res;
    res.id = rootEl.firstChildElement("id").text().toInt();
    res.content_type = rootEl.firstChildElement("content-type").text();
    res.filename = rootEl.firstChildElement("filename").text();
    return res;
}

QList<Attachment> Attachment::loadListFromDom(const QDomElement &rootEl)
{
    QList<Attachment> res;

    QDomNodeList dnlist = rootEl.elementsByTagName("attachment");
    for (uint i = 0; i < dnlist.length(); ++i) {
        res.push_back(Attachment::loadFromDom(dnlist.item(i).toElement()));
    }

    return res;
}

QString Attachment::toXml() const {
    return QString("<attachment>"
                   "<filename>%1</filename>"
                   "<content-type>%2</content-type>"
                   "<upload><key>%3</key></upload>"
                   "</attachment>")
            .arg(this->filename)
            .arg(this->content_type)
            .arg(this->upload_key);
}
}
