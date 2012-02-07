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

#ifndef MAINUIWIDGET_H
#define MAINUIWIDGET_H

#include <QWidget>
#include "unfuddledata.h"

namespace Ui {
    class MainUIWidget;
}

class QNetworkReply;
class QTimer;
class QUrl;

class MainUIWidget : public QWidget
{
    Q_OBJECT

public:
    static const QStringList statuses;
    static const QStringList resolutions;

    explicit MainUIWidget(QWidget *parent = 0);
    ~MainUIWidget();

    unf::Person getCurrentUser() const { return currentUser; }
    int getProjectId() const;
    int getTicketId() const;
    unf::Person personById(int person_id) const;

    const QList<unf::Ticket> & getTickets() const;

    static const uint trackingIntervalMinutes;
    static const uint updateTicketsIntervalMinutes;
signals:
    void unauthorizedAccess();
    void refreshComplete();


public slots:
    void refreshData(bool reloadUser = false);
private slots:
    //void refreshButClicked();
    void on_changeStatus_clicked();
    void statusComboChanged(const QString &status);
    void onTicketStatusLabelClicked(const QString &anchor);
    void on_addComment_clicked();
    void startButClicked();
    void projectIndexChanged(int newIndex);
    void ticketIndexChanged(int newIndex);
    void afterRefresh();
    void timerFired();
    void updateTickets();
    void ticketsUpdated(const QList<unf::Ticket> &tickets, int person_id, int project_id);

    void gotCurrentPerson(const unf::Person &curPerson);
    void gotAllPeople(const QList<unf::Person> &allPeople);
    void gotProjectsForPerson(const QList<unf::Project> &projects, int person_id);
    void gotTicketsForPerson(const QList<unf::Ticket> &tickets, int person_id, int project_id);
    void gotCommentsForTicket(const QList<unf::Comment> &comments, int project_id, int ticket_id);
    void savedTimeEntry(int project_id, int ticket_id, const unf::TimeEntry &time_entry);
    void changedTicket(int project_id, int ticket_id);

    void addedTicketComment(int project_id, int ticket_id, int comment_id);
    void addedAttachmentToTicketComment(int project_id, int ticket_id, int comment_id);

    void networkError(int networkReplyErrorCode, int hasHttpStatus, const QString &errorMsg);
    void xmlError(const QString &errorMsg);

    void wentOffline();
    void wentOnline();

    void onCommentTextChanged();
    void commentAnchorClicked(const QUrl &link);
protected:
     //bool eventFilter(QObject *obj, QEvent *ev);

private:
    Ui::MainUIWidget *ui;
    QPixmap redIndicatorPix;
    QPixmap greenIndicatorPix;

    unf::Person currentUser;
    QList<unf::Person> people;
    QList<unf::Project> userProjects;
    QList<unf::Ticket> tickets;

    bool isTrackingTime;
    int timeTrackingProjectId;
    int timeTrackingTicketId;
    unf::TimeEntry currentTimeEntry;

    QTimer *trackingTimer;
    QTimer *updateTicketsTimer;

    void enableTimeTracking();
    void disableTimeTracking();

    QString currentBranch(bool withProjectId = false) const;
    QString timeEntryKeyName(const QDate &date) const;
    QString timeEntryKeyName(int project_id, int ticket_id, const QDate &date) const;

    void addTicketComment(const QString &text);
    void clearTicketComments();

    void showTicketStatusControls();
    void hideTicketStatusControls();

    void setEnableComments(bool enable);
};

#endif // MAINUIWIDGET_H
