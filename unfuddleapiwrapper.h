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

#ifndef UNFUDDLEAPIWRAPPER_H
#define UNFUDDLEAPIWRAPPER_H

#include <QObject>
#include <QPair>
#include <QList>

#include "unfuddledata.h"

class QNetworkAccessManager;
class QNetworkReply;
class QAuthenticator;
class QDomElement;

typedef QPair<QString, QString > QueryItemType;
typedef QList<QueryItemType > QueryItemsType;

class UnfuddleAPIWrapper : public QObject
{
    Q_OBJECT
public:
    static UnfuddleAPIWrapper *instance();

    // property accessors
    QString getLogin() const { return login; }
    QString getSubdomain() const { return subdomain; }
    void setSubdomain(const QString &newSubdomain);
    void setLoginAndPassword(const QString &newLogin, const QString &newPassword);
    bool getUseSsl() const { return useSsl; }
    void setUseSsl(bool shouldUseSsl) { useSsl = shouldUseSsl; }

    void connectToNetManagerFinished(const QObject *receiver, const char *method);

    QNetworkReply* sendGetRequest(const QString &request, const QueryItemsType &queryItems = QueryItemsType());
    QNetworkReply* sendPostRequest(const QString &request, const QByteArray &data, const char *contentType="application/xml; charset=utf-8");
    QNetworkReply* sendPutRequest(const QString &request, const QByteArray &data);

    void requestGetAccount();
    void requestGetCurrentPerson();
    void requestGetAllPeople();
    void requestGetProjectsForPerson(int person_id);
    void requestGetTicketsForPerson(int person_id, int project_id);
    void requestGetCommentsForTicket(int project_id, int ticket_id);
    void requestAddTimeToTimeEntry(int project_id, int ticket_id, const unf::TimeEntry &time_entry, float hoursAdd);
    void requestSaveTimeEntry(int project_id, int ticket_id, const unf::TimeEntry &time_entry);
    void requestChangeTicket(int project_id, const unf::Ticket &ticket);
    void requestAddTicketComment(int project_id, int ticket_id, const QString &commentBody);

    // returns success status, true if successful
    bool requestAddAttachmentToTicketComment(int project_id, int ticket_id, int comment_id,
                                             const QString &filePath,
                                             const char *contentType="application/octet-stream");
    void requestAddAttachmentToTicketComment(int project_id, int ticket_id, int comment_id,
                                             const QString &displayFileName,
                                             const QByteArray &data,
                                             const char *contentType="application/octet-stream");
signals:
    void getAccountComplete(const unf::Account &account);
    void getCurrentPersonComplete(const unf::Person &curPerson);
    void getAllPeopleComplete(const QList<unf::Person> &people);
    void getProjectsForPersonComplete(const QList<unf::Project> &projects, int person_id);
    void getTicketsForPersonComplete(const QList<unf::Ticket> &tickets, int person_id, int project_id);
    void getCommentsForTicketComplete(const QList<unf::Comment> &comments, int project_id, int ticket_id);
    void saveTimeEntryComplete(int project_id, int ticket_id, const unf::TimeEntry &time_entry);
    void changeTicketComplete(int project_id, int ticket_id);
    void addTicketCommentComplete(int project_id, int ticket_id, int comment_id);
    void addAttachmentToTicketCommentComplete(int project_id, int ticket_id, int comment_id);
    void networkError(int networkReplyErrorCode, int hasHttpStatus, const QString &errorMsg);
    void xmlError(const QString &errorMsg);

    void wentOffline();
    void wentOnline();

public slots:

private slots:
    void requestFinished(QNetworkReply *reply);

private:
    enum RequestType {
        ACCOUNT_LOAD,
        CURRENT_PERSON_LOAD,
        ALL_PEOPLE_LOAD,
        PERSON_INVOLVEMENTS_LOAD,
        PROJECTS_LOAD,
        MILESTONES_LOAD,
        TICKETS_LOAD,

        LIST_TIME_ENTRIES,
        UPDATE_TIME_ENTRY,
        POST_NEW_TIME_ENTRY,
        LIST_AFTER_POST_TIME_ENTRY,

        CHANGE_TICKET,

        ADD_TICKET_COMMENT,
        LIST_TICKET_COMMENTS,   // for getting ticket id to add attachment to
        UPLOAD_TICKET_COMMENT_ATTACHMENT,
        ADD_ATTACHMENT_TO_TICKET_COMMENT,

        TICKET_COMMENTS_LOAD,   // just get ticket comments, no further actions
    };

    QDomElement rootElement(QNetworkReply *reply);
    QString baseUrl() const;
    void processNetworkError(QNetworkReply *reply);

    explicit UnfuddleAPIWrapper(QObject *parent = 0);
    ~UnfuddleAPIWrapper() {};
    Q_DISABLE_COPY(UnfuddleAPIWrapper);

    bool isSetSubdomain;
    bool isSetLoginAndPassword;

    bool useSsl;
    bool isOnline;

    QString subdomain;
    QString login;
    QString password;

    QNetworkAccessManager *netManager;

    QList<unf::Involvement> involvementsForProjectsForPerson;
    unf::TimeEntry timeEntryForSaving;
};

#endif // UNFUDDLEAPIWRAPPER_H
