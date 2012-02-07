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

#include "unfuddleapiwrapper.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QAuthenticator>
#include <QNetworkReply>
#include <QBuffer>
#include <QAbstractNetworkCache>
#include <QDomDocument>
#include <QFile>
#include <QFileInfo>

UnfuddleAPIWrapper *UnfuddleAPIWrapper::instance()
{
    static UnfuddleAPIWrapper theInstance;

    return &theInstance;
}

UnfuddleAPIWrapper::UnfuddleAPIWrapper(QObject *parent) :
    QObject(parent),
    isSetSubdomain(false),
    isSetLoginAndPassword(false),
    useSsl(false),
    isOnline(false)
{
    netManager = new QNetworkAccessManager(this);
    connect(netManager, SIGNAL(finished(QNetworkReply*)), SLOT(requestFinished(QNetworkReply*)));
}

void UnfuddleAPIWrapper::setSubdomain(const QString &newSubdomain)
{
    subdomain = newSubdomain;
    isSetSubdomain = true;
}

void UnfuddleAPIWrapper::setLoginAndPassword(const QString &newLogin, const QString &newPassword)
{
    login = newLogin;
    password = newPassword;
    // FIXME: is this really ok? may be troubles caching auth data
    /*
    if (isSetLoginAndPassword)
        netManager->cache()->clear();
    */
    isSetLoginAndPassword = true;
}

QNetworkReply * UnfuddleAPIWrapper::sendGetRequest(const QString &requestUri, const QueryItemsType & queryItems)
{
    if (isSetSubdomain && isSetLoginAndPassword) {
        QNetworkRequest request;
        QUrl url(this->baseUrl() + requestUri);
        url.setQueryItems(queryItems);

        request.setUrl(url);
        request.setRawHeader("Accept", "application/xml");

        return netManager->get(request);
    }
    return 0;
}

QNetworkReply* UnfuddleAPIWrapper::sendPostRequest(const QString &requestUri, const QByteArray &data, const char *contentType)
{
    if (isSetSubdomain && isSetLoginAndPassword) {
        QNetworkRequest request;
        QUrl url(this->baseUrl() + requestUri);

        request.setUrl(url);
        request.setRawHeader("Accept", "application/xml");
        request.setHeader(QNetworkRequest::ContentTypeHeader, contentType);

        return netManager->post(request, data);
    }
    return 0;
}

QNetworkReply* UnfuddleAPIWrapper::sendPutRequest(const QString &requestUri, const QByteArray &data)
{
    if (isSetSubdomain && isSetLoginAndPassword) {
        QNetworkRequest request;
        QUrl url(this->baseUrl() + requestUri);

        request.setUrl(url);
        request.setRawHeader("Accept", "application/xml");
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/xml; charset=utf-8");

        return netManager->put(request, data);
    }
    return 0;
}

void UnfuddleAPIWrapper::requestFinished(QNetworkReply *reply)
{
    QVariant requestTypeV = reply->property("requestType");
    Q_ASSERT(requestTypeV.isValid());
    RequestType requestType = static_cast<RequestType>(requestTypeV.toInt());

    if (reply->error() == QNetworkReply::NoError) {
        if (!isOnline) {
            isOnline = true;
            emit wentOnline();
        }
        // the following block detects if we use wrong protocol - http instead of https
        // FIXME: what if SSL not available for our account and we use https?
        // probably should handle ssl errors too
        QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        if (statusCode.toInt() == 302) {
            this->setUseSsl(!this->getUseSsl());

            // FIXME: the request which is failed may not necessarily be /account
            // BUG POSSIBILITY IS HIGH!!!
            requestGetAccount();

            return;
        }

        QNetworkReply *nr;

        switch (requestType) {
        case ACCOUNT_LOAD:
            emit getAccountComplete(unf::Account::loadFromDom(rootElement(reply)));
            break;
        case CURRENT_PERSON_LOAD:
            emit getCurrentPersonComplete(unf::Person::loadFromDom(rootElement(reply)));
            break;
        case PERSON_INVOLVEMENTS_LOAD:
            {
                involvementsForProjectsForPerson =
                        unf::Involvement::loadListFromDom(rootElement(reply));
                nr = sendGetRequest("/projects");
                nr->setProperty("requestType", PROJECTS_LOAD);
                nr->setProperty("person_id", reply->property("person_id"));
            }
            break;
        case PROJECTS_LOAD:
            {
                QList<unf::Project> projects =
                        unf::Project::loadListFromDom(rootElement(reply));
                projects = unf::Project::filterProjectListWithInvolvements(
                        projects,
                        involvementsForProjectsForPerson);
                emit getProjectsForPersonComplete(projects,
                                                  reply->property("person_id").toInt());
            }
            break;
        case MILESTONES_LOAD: // Milestones are not used in the program
            break;
        case TICKETS_LOAD:
            {
                int person_id = reply->property("person_id").toInt();
                QList<unf::Ticket> tickets = unf::Ticket::loadListFromDom(rootElement(reply));
                QList<unf::Ticket> tmpTickets;  // filter tickets
                foreach (unf::Ticket t, tickets) {
                    if (t.assignee_id == person_id && t.status != "closed")
                        tmpTickets.push_back(t);
                }
                emit getTicketsForPersonComplete(tmpTickets, person_id,
                                                 reply->property("project_id").toInt());
            }
            break;
        case UPDATE_TIME_ENTRY:
            emit saveTimeEntryComplete(reply->property("project_id").toInt(),
                                       reply->property("ticket_id").toInt(),
                                       timeEntryForSaving);
            break;
        case LIST_TIME_ENTRIES:
        case LIST_AFTER_POST_TIME_ENTRY:
            {
                int project_id = reply->property("project_id").toInt();
                int ticket_id = reply->property("ticket_id").toInt();

                QList<unf::TimeEntry> timeEntries =
                        unf::TimeEntry::loadListFromDom(rootElement(reply));

                unf::TimeEntry curTimeEntry;
                bool found = false;
                foreach (unf::TimeEntry te, timeEntries) {
                    if (te.date == timeEntryForSaving.date &&
                        te.person_id == timeEntryForSaving.person_id) {
                            curTimeEntry = te;
                            found = true;
                            break;
                    }
                }

                if (found) {
                    if (requestType == LIST_TIME_ENTRIES) {
                        curTimeEntry.hours += reply->property("hoursAdd").toFloat();
                        requestSaveTimeEntry(project_id, ticket_id, curTimeEntry);
                    }
                    else if (requestType == LIST_AFTER_POST_TIME_ENTRY)
                        emit saveTimeEntryComplete(project_id, ticket_id, curTimeEntry);
                }
                else {
                    timeEntryForSaving.hours = reply->property("hoursAdd").toFloat();
                    nr = sendPostRequest(
                            QString("/projects/%1/tickets/%2/time_entries")
                                .arg(project_id)
                                .arg(ticket_id),
                            timeEntryForSaving.toXml().toUtf8());
                    nr->setProperty("requestType", POST_NEW_TIME_ENTRY);
                    nr->setProperty("project_id", project_id);
                    nr->setProperty("ticket_id", ticket_id);
                }
            }
            break;
        case POST_NEW_TIME_ENTRY:
            {
                int project_id = reply->property("project_id").toInt();
                int ticket_id = reply->property("ticket_id").toInt();

                nr = sendGetRequest(
                        QString("/projects/%1/tickets/%2/time_entries")
                            .arg(project_id)
                            .arg(ticket_id));
                nr->setProperty("requestType", LIST_AFTER_POST_TIME_ENTRY);
                nr->setProperty("project_id", project_id);
                nr->setProperty("ticket_id", ticket_id);
            }
            break;
        case CHANGE_TICKET:
            emit changeTicketComplete(reply->property("project_id").toInt(),
                                      reply->property("ticket_id").toInt());
            break;
        case ADD_TICKET_COMMENT:
            {
                int project_id = reply->property("project_id").toInt();
                int ticket_id = reply->property("ticket_id").toInt();

                nr = sendGetRequest(QString("/projects/%1/tickets/%2/comments")
                                        .arg(project_id).arg(ticket_id));
                nr->setProperty("requestType", LIST_TICKET_COMMENTS);
                nr->setProperty("project_id", project_id);
                nr->setProperty("ticket_id", ticket_id);
            }
            break;
        case LIST_TICKET_COMMENTS:
            {
                int comment_id = unf::Comment::loadFromDom(
                        rootElement(reply).lastChild().toElement()).id;
                emit addTicketCommentComplete(reply->property("project_id").toInt(),
                                              reply->property("ticket_id").toInt(),
                                              comment_id);
            }
            break;
        case UPLOAD_TICKET_COMMENT_ATTACHMENT:
            {
                int project_id = reply->property("project_id").toInt();
                int ticket_id = reply->property("ticket_id").toInt();
                int comment_id = reply->property("comment_id").toInt();

                unf::Attachment a;
                a.filename = reply->property("filename").toString();
                a.content_type = "application/octet-stream";
                a.upload_key = rootElement(reply).firstChildElement("key").text();

                nr = sendPostRequest(
                        QString("/projects/%1/tickets/%2/comments/%3/attachments")
                            .arg(project_id).arg(ticket_id).arg(comment_id),
                        a.toXml().toUtf8());
                nr->setProperty("requestType", ADD_ATTACHMENT_TO_TICKET_COMMENT);
                nr->setProperty("project_id", project_id);
                nr->setProperty("ticket_id", ticket_id);
                nr->setProperty("comment_id", comment_id);
            }
            break;
        case ADD_ATTACHMENT_TO_TICKET_COMMENT:
            emit addAttachmentToTicketCommentComplete(reply->property("project_id").toInt(),
                                                      reply->property("ticket_id").toInt(),
                                                      reply->property("comment_id").toInt());
            break;
        case TICKET_COMMENTS_LOAD:
            emit getCommentsForTicketComplete(
                    unf::Comment::loadListFromDom(rootElement(reply)),
                    reply->property("project_id").toInt(),
                    reply->property("ticket_id").toInt());
            break;
        case ALL_PEOPLE_LOAD:
            emit getAllPeopleComplete(unf::Person::loadListFromDom(rootElement(reply)));
            break;
        }
    }
    else
        processNetworkError(reply);

    reply->deleteLater();
}

void UnfuddleAPIWrapper::processNetworkError(QNetworkReply *reply)
{
    QString errorMsg;
    QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if (statusCode.isValid()) {
        switch (statusCode.toInt()) {
        case 400: {  // bad request, probably have parameters in xml body
            errorMsg = "<b>Bad Request</b><br/><br/>";
            QDomElement rootEl = rootElement(reply);
            if (!rootEl.isNull()) {
                QDomNodeList dnl = rootEl.elementsByTagName("message");
                for (int i = 0; i < dnl.count(); ++i) {
                    errorMsg += QString("- ") + dnl.at(i).firstChild().toText().data() +
                                "<br/>";
                }
            }
            break;
        }
        case 401:   // unauthorized
            errorMsg = "<b>Unauthorized</b>";
            break;
        case 404:   // not found
            errorMsg = "<b>Not Found</b>";
            break;
        case 405:   // method not allowed (probably logic error)
            errorMsg = "<b>Method Not Allowed</b>";
            break;
        case 406:   // not acceptable
            errorMsg = "<b>Not Acceptable</b>";
            break;
        case 415:   // unsupported media type
            errorMsg = "<b>Unsupported Media Type</b>";
            break;
        case 500:   // internal server error
            errorMsg = "<b>Internal Server Error</b><br/><br/>"
                       "There has been an error from which Unfuddle could not recover. "
                       "There is a high likelihood that an internal server error "
                       "represents a bug on the part of Unfuddle. Unfuddle Support "
                       "automatically receives notifications of these errors for "
                       "further investigation.";
            break;
        default:
            errorMsg = reply->errorString();
            break;
        }
    }
    else {
        isOnline = false;
        emit wentOffline();
        errorMsg = reply->errorString();
    }

    emit networkError(reply->error(), statusCode.isValid(), errorMsg);
}

QString UnfuddleAPIWrapper::baseUrl() const
{
    if (isSetSubdomain)
        return QString(useSsl ? "https" : "http") + QString("://")
                + login + QString(":") + password + QString("@")
                + subdomain + QString(".unfuddle.com/api/v1");

    return "[Empty URL]";
}

void UnfuddleAPIWrapper::connectToNetManagerFinished(const QObject *receiver, const char *method)
{
    connect(netManager, SIGNAL(finished(QNetworkReply*)), receiver, method);
}

QDomElement UnfuddleAPIWrapper::rootElement(QNetworkReply *reply)
{
    QByteArray xmlData = reply->readAll();

    QDomDocument doc;
    QString errorMsg;
    if (doc.setContent(xmlData, &errorMsg)) {
        return doc.documentElement();
    }
    else
        emit xmlError(errorMsg);

    return QDomElement();
}

void UnfuddleAPIWrapper::requestGetAccount()
{
    QNetworkReply *nr = sendGetRequest("/account");
    nr->setProperty("requestType", ACCOUNT_LOAD);
}

void UnfuddleAPIWrapper::requestGetCurrentPerson()
{
    QNetworkReply *nr = sendGetRequest("/people/current");
    nr->setProperty("requestType", CURRENT_PERSON_LOAD);
}

void UnfuddleAPIWrapper::requestGetAllPeople()
{
    QNetworkReply *nr = sendGetRequest("/people");
    nr->setProperty("requestType", ALL_PEOPLE_LOAD);
}

void UnfuddleAPIWrapper::requestGetProjectsForPerson(int person_id)
{
    QNetworkReply *nr = sendGetRequest(QString("/people/%1/involvements").arg(person_id));
    nr->setProperty("requestType", PERSON_INVOLVEMENTS_LOAD);
    nr->setProperty("person_id", person_id);
}

void UnfuddleAPIWrapper::requestGetTicketsForPerson(int person_id, int project_id)
{
    QNetworkReply *nr = sendGetRequest(QString("/projects/%1/tickets").arg(project_id));
    nr->setProperty("requestType", TICKETS_LOAD);
    nr->setProperty("person_id", person_id);
    nr->setProperty("project_id", project_id);
}

void UnfuddleAPIWrapper::requestAddTimeToTimeEntry(int project_id, int ticket_id, const unf::TimeEntry &time_entry, float hoursAdd)
{
    timeEntryForSaving = time_entry;
    QNetworkReply *nr = sendGetRequest(QString("/projects/%1/tickets/%2/time_entries")
                                            .arg(project_id)
                                            .arg(ticket_id));
    nr->setProperty("requestType", LIST_TIME_ENTRIES);
    nr->setProperty("project_id", project_id);
    nr->setProperty("ticket_id", ticket_id);
    nr->setProperty("hoursAdd", hoursAdd);
}

void UnfuddleAPIWrapper::requestSaveTimeEntry(int project_id, int ticket_id, const unf::TimeEntry &time_entry)
{
    timeEntryForSaving = time_entry;
    QNetworkReply *nr = sendPutRequest(QString("/projects/%1/tickets/%2/time_entries/%3")
                                .arg(project_id)
                                .arg(ticket_id)
                                .arg(time_entry.id),
                            time_entry.toXml().toUtf8());
    nr->setProperty("requestType", UPDATE_TIME_ENTRY);
    nr->setProperty("project_id", project_id);
    nr->setProperty("ticket_id", ticket_id);
}

void UnfuddleAPIWrapper::requestChangeTicket(int project_id, const unf::Ticket &ticket)
{
    QNetworkReply *nr = sendPutRequest(QString("/projects/%1/tickets/%2")
                                            .arg(project_id)
                                            .arg(ticket.id),
                                       ticket.toXml().toUtf8());
    nr->setProperty("requestType", CHANGE_TICKET);
    nr->setProperty("project_id", project_id);
    nr->setProperty("ticket_id", ticket.id);
}

void UnfuddleAPIWrapper::requestAddTicketComment(int project_id, int ticket_id, const QString &commentBody)
{
    unf::Comment c;
    c.body = commentBody;

    QNetworkReply *nr = sendPostRequest(QString("/projects/%1/tickets/%2/comments")
                                            .arg(project_id)
                                            .arg(ticket_id),
                                        c.toXml().toUtf8());
    nr->setProperty("requestType", ADD_TICKET_COMMENT);
    nr->setProperty("project_id", project_id);
    nr->setProperty("ticket_id", ticket_id);
}

bool UnfuddleAPIWrapper::requestAddAttachmentToTicketComment(int project_id, int ticket_id, int comment_id, const QString &filePath, const char *contentType)
{
    QFileInfo pathInfo(filePath);
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    requestAddAttachmentToTicketComment(project_id, ticket_id, comment_id,
                                        pathInfo.fileName(), file.readAll(), contentType);
    return true;
}

void UnfuddleAPIWrapper::requestAddAttachmentToTicketComment(int project_id, int ticket_id,
                                         int comment_id,
                                         const QString &displayFileName,
                                         const QByteArray &data,
                                         const char *contentType)
{
    QNetworkReply *nr = sendPostRequest(
            QString("/projects/%1/tickets/%2/comments/%3/attachments/upload")
                .arg(project_id).arg(ticket_id).arg(comment_id),
            data,
            contentType);
    nr->setProperty("requestType", UPLOAD_TICKET_COMMENT_ATTACHMENT);
    nr->setProperty("project_id", project_id);
    nr->setProperty("ticket_id", ticket_id);
    nr->setProperty("comment_id", comment_id);
    nr->setProperty("filename", displayFileName);

}

void UnfuddleAPIWrapper::requestGetCommentsForTicket(int project_id, int ticket_id)
{
    QNetworkReply *nr = sendGetRequest(
            QString("/projects/%1/tickets/%2/comments")
                .arg(project_id).arg(ticket_id),
            QueryItemsType() << QueryItemType("attachments", "true"));
    nr->setProperty("requestType", TICKET_COMMENTS_LOAD);
    nr->setProperty("project_id", project_id);
    nr->setProperty("ticket_id", ticket_id);
}
