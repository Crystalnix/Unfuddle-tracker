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

#include "mainuiwidget.h"
#include "ui_mainuiwidget.h"
#include "unfuddleapiwrapper.h"
#include "unfuddledata.h"
#include "mainwindow.h"
#include "application.h"
#include "noscrolltextedit.h"

#include <QNetworkReply>
#include <QMovie>
#include <QSettings>
#include <QMessageBox>
#include <QTimer>
#include <QSystemTrayIcon>
#include <QUrl>
#include <QFileDialog>

const uint MainUIWidget::trackingIntervalMinutes = 5;
const uint MainUIWidget::updateTicketsIntervalMinutes = 1;

const QStringList MainUIWidget::statuses = QStringList() << "new" << "unaccepted" <<
                                               "reassigned" << "reopened" << "accepted" <<
                                               "resolved" << "closed";
const QStringList MainUIWidget::resolutions = QStringList() << "none" << "fixed" <<
                                                  "works_for_me" << "postponed" <<
                                                  "duplicate" << "will_not_fix" << "invalid";
MainUIWidget::MainUIWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainUIWidget),
    isTrackingTime(false),
    trackingTimer(new QTimer(this)),
    updateTicketsTimer(new QTimer(this))
{
    ui->setupUi(this);

    redIndicatorPix.load(":/images/red-indicator.png");
    greenIndicatorPix.load(":/images/green-indicator.png");

    ui->indicator->setPixmap(redIndicatorPix);
    ui->scrollArea->setWidgetResizable(true);

    ui->ticketStatus->addItems(statuses);
    ui->ticketResolution->addItems(resolutions);

    ui->addCommentText->setLabel("Add ticket comment here...");

    //ui->ticketStatusLayout->setAlignment(ui->ticketStatus, Qt::AlignLeft);
    //ui->ticketStatusLayout->setAlignment(ui->ticketResolution, Qt::AlignLeft);
    //ui->ticketStatusLayout->setAlignment(ui->changeStatus, Qt::AlignLeft);
    hideTicketStatusControls();

    //ui->ticketDescription->viewport()->installEventFilter(this);

    //ComboValidator *projectValidator = new ComboValidator(ui->projectName);
    //ComboValidator *ticketValidator = new ComboValidator(ui->ticketName);

    //ui->projectName->setValidator(projectValidator);
    //ui->ticketName->setValidator(ticketValidator);

    connect(ui->projectName, SIGNAL(activated(int)), SLOT(projectIndexChanged(int)));
    connect(ui->ticketName, SIGNAL(activated(int)), SLOT(ticketIndexChanged(int)));
    connect(this, SIGNAL(refreshComplete()), SLOT(afterRefresh()));
    //connect(ui->refreshBut, SIGNAL(clicked()), SLOT(refreshData()));
    connect(ui->startBut, SIGNAL(clicked()), SLOT(startButClicked()));
    connect(trackingTimer, SIGNAL(timeout()), SLOT(timerFired()));
    connect(updateTicketsTimer, SIGNAL(timeout()), SLOT(updateTickets()));
    connect(ui->ticketStatusLabel, SIGNAL(linkActivated(QString)), SLOT(onTicketStatusLabelClicked(QString)));
    connect(ui->ticketStatus, SIGNAL(currentIndexChanged(QString)), SLOT(statusComboChanged(QString)));

    connect(ui->addCommentText, SIGNAL(textChanged()), SLOT(onCommentTextChanged()));

    UnfuddleAPIWrapper *uapi = UnfuddleAPIWrapper::instance();
    connect(uapi, SIGNAL(getCurrentPersonComplete(unf::Person)), SLOT(gotCurrentPerson(unf::Person)));
    connect(uapi, SIGNAL(getAllPeopleComplete(QList<unf::Person>)), SLOT(gotAllPeople(QList<unf::Person>)));
    connect(uapi, SIGNAL(getProjectsForPersonComplete(QList<unf::Project>,int)), SLOT(gotProjectsForPerson(QList<unf::Project>,int)));
    connect(uapi, SIGNAL(getCommentsForTicketComplete(QList<unf::Comment>,int,int)), SLOT(gotCommentsForTicket(QList<unf::Comment>,int,int)));
    connect(uapi, SIGNAL(saveTimeEntryComplete(int,int,unf::TimeEntry)), SLOT(savedTimeEntry(int,int,unf::TimeEntry)));
    connect(uapi, SIGNAL(changeTicketComplete(int,int)), SLOT(changedTicket(int,int)));
    connect(uapi, SIGNAL(addAttachmentToTicketCommentComplete(int,int,int)), SLOT(addedAttachmentToTicketComment(int,int,int)));

    trackingTimer->setInterval(1000 * 60 * MainUIWidget::trackingIntervalMinutes);
    updateTicketsTimer->start(1000 * 60 * MainUIWidget::updateTicketsIntervalMinutes);

    currentTimeEntry.id = 0;

    this->refreshData(true);
}

MainUIWidget::~MainUIWidget()
{
    //delete loaderAnimation;
    delete ui;
}


void MainUIWidget::refreshData(bool reloadUser)
{
    ui->projectName->setEnabled(false);
    ui->ticketName->setEnabled(false);
    //ui->editTicketBut->setEnabled(false);

    ui->projectName->clear();
    ui->ticketName->clear();

    //ui->refreshBut->setEnabled(false);
    ui->startBut->setEnabled(false);

    if (reloadUser) {
        UnfuddleAPIWrapper::instance()->requestGetCurrentPerson();
        Application::instance().mainWindow().setStatus("Loading current user data...",
                                            MyStatusBar::STATUS_SPINNER);
    } else {
        UnfuddleAPIWrapper::instance()->requestGetProjectsForPerson(currentUser.id);
        Application::instance().mainWindow().setStatus("Loading current user projects...",
                                            MyStatusBar::STATUS_SPINNER);
    }
}

void MainUIWidget::afterRefresh()
{
    ui->projectName->setEnabled(true);
    ui->ticketName->setEnabled(true);

    //ui->refreshBut->setEnabled(true);

    Application::instance().mainWindow().setStatus("Complete loading data.");
}

void MainUIWidget::projectIndexChanged(int newIndex)
{
    clearTicketComments();

    if (newIndex != -1) {
        ui->projectName->setEnabled(false);
        ui->ticketName->setEnabled(false);
        //ui->refreshBut->setEnabled(false);
        ui->startBut->setEnabled(false);
        //ui->editTicketBut->setEnabled(false);

        ui->ticketName->clear();

        Application::instance().mainWindow().setStatus("Loading tickets for project...",
                                                       MyStatusBar::STATUS_SPINNER);

        UnfuddleAPIWrapper *uapi = UnfuddleAPIWrapper::instance();
        connect(uapi, SIGNAL(getTicketsForPersonComplete(QList<unf::Ticket>,int,int)),
                this, SLOT(gotTicketsForPerson(QList<unf::Ticket>,int,int)));
        uapi->requestGetTicketsForPerson(
                currentUser.id,
                userProjects.at(newIndex).id);
    } else {
        ui->ticketName->clear();
    }
}

void MainUIWidget::ticketIndexChanged(int newIndex)
{
    currentTimeEntry.id = 0;
    if (isTrackingTime) {
        timeTrackingProjectId = userProjects.at(ui->projectName->currentIndex()).id;
        timeTrackingTicketId = tickets.at(ui->ticketName->currentIndex()).id;
    }

    clearTicketComments();

    if (newIndex != -1) {
        QSettings settings("Crystalnix", "utracker");
        settings.setValue(currentBranch(true) + "ticket_id", QVariant(tickets.at(newIndex).id));
        ui->startBut->setEnabled(true);
        //ui->editTicketBut->setEnabled(true);
        ui->ticketDescription->setText(tickets.at(newIndex).description);

        ui->ticketStatusLabel->setText(QString("Ticket status: <a href='#'><u>%1</u></a>")
                                       .arg(tickets.at(newIndex).status));

        Application::instance().mainWindow().setStatus("Loading people...",
                                                       MyStatusBar::STATUS_SPINNER);
        UnfuddleAPIWrapper::instance()->requestGetAllPeople();
    }
    else {
        if (isTrackingTime)
            disableTimeTracking();
        ui->startBut->setEnabled(false);
    }
}

QString MainUIWidget::currentBranch(bool withProjectId) const
{
    QString tail = "";
    if (withProjectId && ui->projectName->currentIndex() != -1)
        tail = QString("project-%1/").arg(
                userProjects.at(ui->projectName->currentIndex()).id);

    QSettings settings("Crystalnix", "utracker");
    return settings.value("login/subdomain").toString() + "/" +
            settings.value("login/login").toString() + "/current/" + tail;
}

QString MainUIWidget::timeEntryKeyName(int project_id, int ticket_id, const QDate &date) const
{
    return currentBranch(false) + QString("time-entries/%1/%2/%3").arg(project_id).arg(
            ticket_id).arg(date.toString("yyyy-MM-dd"));
}

QString MainUIWidget::timeEntryKeyName(const QDate &date) const
{
    return timeEntryKeyName(timeTrackingProjectId, timeTrackingTicketId, date);
}

void MainUIWidget::enableTimeTracking()
{
    timeTrackingProjectId = userProjects.at(ui->projectName->currentIndex()).id;
    timeTrackingTicketId = tickets.at(ui->ticketName->currentIndex()).id;

    ui->startBut->setText("Stop");
    ui->timeTrackStatus->setText("Time tracking is ON");
    ui->indicator->setPixmap(greenIndicatorPix);

    isTrackingTime = true;

    trackingTimer->start();
}

void MainUIWidget::disableTimeTracking()
{
    trackingTimer->stop();

    ui->startBut->setText("Start");
    ui->timeTrackStatus->setText("Time tracking is OFF");
    ui->indicator->setPixmap(redIndicatorPix);
    isTrackingTime = false;
}

void MainUIWidget::startButClicked()
{
    if (!isTrackingTime)
        enableTimeTracking();
    else
        disableTimeTracking();
}

void MainUIWidget::timerFired()
{
    QSettings settings("Crystalnix", "utracker");
    QDate curDate = QDate::currentDate();

    float hours = settings.value(timeEntryKeyName(curDate), 0.0f).toFloat();
    hours += static_cast<float>(MainUIWidget::trackingIntervalMinutes) / 60;
    settings.setValue(timeEntryKeyName(curDate), hours);

    unf::TimeEntry te;
    if (currentTimeEntry.id == 0 ||
        currentTimeEntry.date != curDate) {

        te.date = curDate;
        te.person_id = currentUser.id;

        QList<unf::Ticket>::iterator it = tickets.begin();
        while (it != tickets.end() && it->id != timeTrackingTicketId) {
            ++it;
        }

        if (it == tickets.end()) {
            qWarning("No time tracking ticket");
            return;
        }

        te.description = QString("#%1 - %2").arg(it->number).arg(it->summary);

        UnfuddleAPIWrapper::instance()->requestAddTimeToTimeEntry(timeTrackingProjectId,
                                                                  timeTrackingTicketId,
                                                                  te, hours);
    }
    else {
        te = currentTimeEntry;
        te.hours += hours;
        UnfuddleAPIWrapper::instance()->requestSaveTimeEntry(timeTrackingProjectId,
                                                             timeTrackingTicketId, te);
    }
    Application::instance().mainWindow().setStatus("Uploading time tracking data.",
                                        MyStatusBar::STATUS_SPINNER);
}

// network callbacks ------------------------------------------------------
void MainUIWidget::gotCurrentPerson(const unf::Person &curPerson)
{
    currentUser = curPerson;
    ui->nameLabel->setText(currentUser.first_name + " " + currentUser.last_name);
    UnfuddleAPIWrapper::instance()->requestGetProjectsForPerson(currentUser.id);
    Application::instance().mainWindow().setStatus("Loading current user projects...",
                                        MyStatusBar::STATUS_SPINNER);
}

void MainUIWidget::gotProjectsForPerson(const QList<unf::Project> &projects, int person_id)
{
    Q_UNUSED(person_id)

    QSettings settings("Crystalnix", "utracker");
    userProjects = projects;

    QList<unf::Project> tmpProjects;
    foreach (unf::Project p, userProjects)
        if (!p.archived && p.enable_time_tracking)  // filter currentProjects by active status
            tmpProjects.push_back(p);
    userProjects = tmpProjects;

    int currentProjectId = settings.value(currentBranch() + "project_id",
                                          QVariant(static_cast<int>(0))).toInt();

    int currentProjectIndex = 0;
    int i = 0;
    foreach (unf::Project p, userProjects) {
        ui->projectName->addItem(p.title);
        if (p.id == currentProjectId)
            currentProjectIndex = i;
        ++i;
    }

    emit refreshComplete();

    if (userProjects.length() > 0) {
        ui->projectName->setCurrentIndex(currentProjectIndex);
        projectIndexChanged(currentProjectIndex);
    }
    else {
        ui->ticketDescription->clear();
        QMessageBox msg;
        msg.setText("No projects found for " + currentUser.first_name + " " +
                    currentUser.last_name);
        msg.exec();
    }
    Application::instance().mainWindow().setStatus("Projects data loaded successfully.");
}

void MainUIWidget::gotTicketsForPerson(const QList<unf::Ticket> &_tickets, int person_id, int project_id)
{
    Q_UNUSED(person_id)

    UnfuddleAPIWrapper *uapi = UnfuddleAPIWrapper::instance();
    disconnect(uapi, SIGNAL(getTicketsForPersonComplete(QList<unf::Ticket>,int,int)),
               this, SLOT(gotTicketsForPerson(QList<unf::Ticket>,int,int)));
    QSettings settings("Crystalnix", "utracker");
    settings.setValue(currentBranch() + "project_id", QVariant(project_id));
    tickets = _tickets;

    // TODO: filter and sort tickets appropriately
    QList<unf::Ticket> tmpTickets;
    foreach (unf::Ticket t, tickets) {
        if (t.assignee_id == currentUser.id && t.status != "closed")
            tmpTickets.push_back(t);
    }
    tickets = tmpTickets;

    int currentTicketId = settings.value(currentBranch(true) + "ticket_id",
                                         QVariant(static_cast<int>(0))).toInt();
    int currentTicketIndex = 0;
    int i = 0;
    foreach (unf::Ticket t, tickets) {
        ui->ticketName->addItem(QString("#%1 - %2").arg(t.number).arg(t.summary));
        if (t.id == currentTicketId)
            currentTicketIndex = i;
        ++i;
    }

    emit refreshComplete();

    if (tickets.length() > 0) {
        Application::instance().enableScreensnap();
        Application::instance().mainWindow().setStatus("Tickets data loaded successfully.");
        ui->ticketName->setCurrentIndex(currentTicketIndex);
        ticketIndexChanged(currentTicketIndex);
        setEnableComments(true);
    }
    else {
        Application::instance().disableScreensnap();
        Application::instance().mainWindow().setStatus("No tickets loaded.");
        ui->ticketDescription->clear();
        setEnableComments(false);
        ui->ticketStatusLabel->setText(tr("No active tickets"));
        QMessageBox msg;
        msg.setText("No active tickets assigned to " + currentUser.first_name + " " +
                    currentUser.last_name);
        msg.exec();
    }

}

void MainUIWidget::gotAllPeople(const QList<unf::Person> &allPeople)
{
    people = allPeople;

    Application::instance().mainWindow().setStatus("Loading comments for ticket.",
                                                   MyStatusBar::STATUS_SPINNER);
    UnfuddleAPIWrapper::instance()->requestGetCommentsForTicket(getProjectId(),
                                                                getTicketId());
}

void MainUIWidget::gotCommentsForTicket(const QList<unf::Comment> &comments, int project_id, int ticket_id)
{
    if (project_id == getProjectId() && ticket_id == getTicketId()) {
        foreach (unf::Comment com, comments) {
            unf::Person p = personById(com.author_id);
            QString header = "By <b>" + p.first_name + " " + p.last_name + "</b> at " +
                             com.created_at.toString("yyyy-MM-dd HH:mm:ss") + " GMT<br/><br/>";
            QString footer = "";
            if (com.attachments.length())
            {
                footer += "<br/><br/>Attachments:<ul>";
                foreach (unf::Attachment a, com.attachments) {
                    footer += QString("<li><a href='%1://%2.unfuddle.com/projects/%3"
                                      "/tickets/%4/comments/%5/attachments/%6/download'>"
                                      "%7</a></li>")
                        .arg(UnfuddleAPIWrapper::instance()->getUseSsl() ? "https" : "http")
                        .arg(UnfuddleAPIWrapper::instance()->getSubdomain())
                        .arg(project_id).arg(ticket_id).arg(com.id).arg(a.id)
                        .arg(a.filename);
                }
                footer += "</ul>";
            }

            footer += QString("<br /><div>"
                      "<a href='#%1'><img src=':/images/attach.png' width='16' height='16' /> Attach file...</a>"
                      "</div>").arg(com.id);

            addTicketComment(header + com.body + footer);
        }

        if (comments.length())
            Application::instance().mainWindow().setStatus("Ticket comments loaded successfully.");
        else
            Application::instance().mainWindow().setStatus("No comments for this ticket.");
    }
}

void MainUIWidget::savedTimeEntry(int project_id, int ticket_id, const unf::TimeEntry &time_entry)
{
    QSettings settings("Crystalnix", "utracker");
    currentTimeEntry = time_entry;
    QString key = timeEntryKeyName(project_id, ticket_id, time_entry.date);
    settings.remove(key);
    Application::instance().mainWindow().setStatus("Time entry saved successfully.");
}

void MainUIWidget::changedTicket(int project_id, int ticket_id)
{
    Q_UNUSED(project_id)
    Q_UNUSED(ticket_id)

    unf::Ticket t = tickets.at(ui->ticketName->currentIndex());

    ui->ticketStatusLabel->setText(QString("Ticket status: <a href='#'>%1</a>")
                                   .arg(t.status));
    Application::instance().mainWindow().setStatus("Ticket status changed successfully.");
    Application::instance().trayIcon().showMessage("Ticket status changed.",
                                                             "You have successfully changed "
                                                             "the ticket status.");
}

void MainUIWidget::addedTicketComment(int project_id, int ticket_id, int comment_id)
{
    Q_UNUSED(comment_id)

    UnfuddleAPIWrapper *uapi = UnfuddleAPIWrapper::instance();
    disconnect(uapi, SIGNAL(addTicketCommentComplete(int,int,int)),
               this, SLOT(addedTicketComment(int,int,int)));
    ui->addCommentText->clear();
    ui->scrollArea->ensureWidgetVisible(ui->ticketDescription);

    if (project_id == getProjectId() && ticket_id == getTicketId()) {
        Application::instance().mainWindow().setStatus("Loading comments for ticket...",
                                                       MyStatusBar::STATUS_SPINNER);
        clearTicketComments();
        UnfuddleAPIWrapper::instance()->requestGetCommentsForTicket(project_id,
                                                                    ticket_id);
    }
}

void MainUIWidget::addedAttachmentToTicketComment(int project_id, int ticket_id, int comment_id)
{
    Q_UNUSED(comment_id)

    if (project_id == getProjectId() && ticket_id == getTicketId()) {
        Application::instance().mainWindow().setStatus("Loading comments for ticket...",
                                                       MyStatusBar::STATUS_SPINNER);
        clearTicketComments();
        UnfuddleAPIWrapper::instance()->requestGetCommentsForTicket(project_id,
                                                                    ticket_id);
    }

}

void MainUIWidget::networkError(int networkReplyErrorCode, int hasHttpStatus, const QString &errorMsg)
{
    Q_UNUSED(hasHttpStatus)
    Q_UNUSED(errorMsg)

    currentTimeEntry.id = 0;

    if (networkReplyErrorCode == QNetworkReply::AuthenticationRequiredError) {
        disableTimeTracking();

        QMessageBox msg;
        msg.setText("Login credentials supplied are invalid.<br/>"
                    "Reverting to login screen.");
        msg.exec();

        emit unauthorizedAccess();
    }

    Application::instance().mainWindow().setStatus("Network error.",
                                                   MyStatusBar::STATUS_ERROR);
    //ui->errorLabel->setText("Network error:" + errorMsg + "<br/>" + ui->errorLabel->text());
}

void MainUIWidget::xmlError(const QString &errorMsg)
{
    Q_UNUSED(errorMsg)

    //ui->errorLabel->setText("XML parse error<br/>" + ui->errorLabel->text());
    Application::instance().mainWindow().setStatus("XML parse error.",
                                                   MyStatusBar::STATUS_ERROR);
}

void MainUIWidget::wentOnline()
{
    Application::instance().mainWindow().setStatus("Went online.");
    //ui->errorLabel->setText("<span class='color:green'>Went online</span><br/>" +
    //                        ui->errorLabel->text());
}

void MainUIWidget::wentOffline()
{
    Application::instance().mainWindow().setStatus("Went offline.",
                                                   MyStatusBar::STATUS_ERROR);
    //ui->errorLabel->setText("<span class='color:red'>Went offline</span><br/>" +
    //                        ui->errorLabel->text());

    //ui->loading->hide();
    //ui->refreshBut->setEnabled(true);
}

int MainUIWidget::getProjectId() const
{
    int projectIndex = ui->projectName->currentIndex();
    if (projectIndex != -1)
        return userProjects.at(projectIndex).id;

    return 0;
}

int MainUIWidget::getTicketId() const
{
    int ticketIndex = ui->ticketName->currentIndex();
    if (ticketIndex != -1)
        return tickets.at(ticketIndex).id;
    return 0;
}

//bool MainUIWidget::eventFilter(QObject *obj, QEvent *ev)
//{
//    if (obj->objectName() == ui->ticketDescription->viewport()->objectName()) {
//        if (ev->type() == QEvent::MouseButtonDblClick &&
//            ((QMouseEvent*)ev)->button() == Qt::LeftButton &&
//            ui->ticketName->currentIndex() != -1 &&
//            ui->projectName->currentIndex() != -1) {
//                on_editTicketBut_clicked();
//                return true;
//        }
//        else
//            return false;
//    }
//    else {
//        return QObject::eventFilter(obj, ev);
//    }
//}

void MainUIWidget::on_addComment_clicked()
{
    if (ui->addComment->text() == "Add comment") {
        ui->scrollArea->ensureWidgetVisible(ui->addCommentText);
        ui->addCommentText->setFocus();
    }
    else if (ui->ticketName->currentIndex() != -1 && ui->projectName->currentIndex() != -1) {

        int projectId = userProjects.at(ui->projectName->currentIndex()).id;
        int ticketId = tickets.at(ui->ticketName->currentIndex()).id;

        UnfuddleAPIWrapper *uapi = UnfuddleAPIWrapper::instance();
        uapi->requestAddTicketComment(projectId, ticketId,
                                        ui->addCommentText->toPlainText());
        connect(uapi, SIGNAL(addTicketCommentComplete(int,int,int)),
                SLOT(addedTicketComment(int,int,int)));
        Application::instance().mainWindow().setStatus("Adding ticket comment...",
                                                       MyStatusBar::STATUS_SPINNER);
    }
}

void MainUIWidget::addTicketComment(const QString &text)
{
    if (!ui->commentsContainer->isVisible())
        ui->commentsContainer->show();

    QVBoxLayout *layout = ui->commentsContainerLayout;

    NoScrollTextEdit *commentControl = new NoScrollTextEdit(this);
    commentControl->setText(text);
    commentControl->setReadOnly(true);

    connect(commentControl, SIGNAL(anchorClicked(QUrl)), SLOT(commentAnchorClicked(QUrl)));

    layout->addWidget(commentControl, 1);
}

void MainUIWidget::clearTicketComments()
{
    QVBoxLayout *layout = ui->commentsContainerLayout;
    while (layout->count()) {
        QWidget *w = layout->itemAt(0)->widget();
        layout->removeWidget(w);
        delete w;
    }

    ui->commentsContainer->hide();
}

unf::Person MainUIWidget::personById(int person_id) const
{
    foreach (unf::Person p, people)
        if (p.id == person_id)
            return p;

    unf::Person unknown;
    unknown.id = 0;
    unknown.first_name = "unknown";
    unknown.last_name = "unknown";
    unknown.username = "unknown";
    return unknown;
}

const QList<unf::Ticket> & MainUIWidget::getTickets() const
{
    return tickets;
}

void MainUIWidget::onTicketStatusLabelClicked(const QString &anchor)
{
    Q_UNUSED(anchor)

    if (ui->ticketStatus->isVisible())
        hideTicketStatusControls();
    else if (ui->ticketName->currentIndex() != -1) {
        unf::Ticket t = tickets.at(ui->ticketName->currentIndex());
        ui->ticketStatus->setCurrentIndex(statuses.indexOf(t.status));
        ui->ticketResolution->setCurrentIndex((resolutions.indexOf(t.resolution) == -1) ?
                                              0 : resolutions.indexOf(t.resolution));
        showTicketStatusControls();
    }
}

void MainUIWidget::on_changeStatus_clicked()
{
    if (ui->ticketName->currentIndex() != -1 && ui->projectName->currentIndex() != -1) {
        unf::Ticket &t = tickets[ui->ticketName->currentIndex()];
        t.status = ui->ticketStatus->currentText();
        t.resolution = (ui->ticketResolution->currentText() == "none") ?
                       "" : ui->ticketResolution->currentText();

        hideTicketStatusControls();

        UnfuddleAPIWrapper::instance()->requestChangeTicket(getProjectId(), t);
        Application::instance().mainWindow().setStatus("Uploading new ticket data.",
                                            MyStatusBar::STATUS_SPINNER);
    }
}

void MainUIWidget::showTicketStatusControls()
{
    for (int i = 0; i < ui->ticketStatusLayout->count()-1; ++i)
        ui->ticketStatusLayout->itemAt(i)->widget()->show();
    if (ui->ticketStatus->currentText() != "resolved" &&
        ui->ticketStatus->currentText() != "closed")
            ui->ticketResolution->hide();
}

void MainUIWidget::hideTicketStatusControls()
{
    for (int i = 0; i < ui->ticketStatusLayout->count()-1; ++i)
        ui->ticketStatusLayout->itemAt(i)->widget()->hide();
}

void MainUIWidget::statusComboChanged(const QString &status)
{
    if (status == "resolved" || status == "closed")
        ui->ticketResolution->show();
    else
        ui->ticketResolution->hide();
}

void MainUIWidget::onCommentTextChanged()
{
    if (ui->addCommentText->toPlainText() == ui->addCommentText->label() ||
        ui->addCommentText->toPlainText().isEmpty()) {
            ui->addComment->setText("Add comment");
    }
    else
        ui->addComment->setText("Submit comment");
}

void MainUIWidget::commentAnchorClicked(const QUrl &link)
{
    if (link.host().indexOf("unfuddle.com") != -1)
        return;

    int comment_id = link.fragment().toInt();

    QFileDialog dlg(this);
    dlg.setWindowTitle("Choose a file to attach");
    dlg.setFileMode(QFileDialog::ExistingFile);

    // FIXME: dialog event loop may stop our time tracking timer from functioning properly
    if (dlg.exec()) {
        if (UnfuddleAPIWrapper::instance()->requestAddAttachmentToTicketComment(getProjectId(),
                                                                                getTicketId(),
                                                                                comment_id,
                                                                                dlg.selectedFiles().at(0))) {
            Application::instance().mainWindow().setStatus("Uploading ticket attachment...",
                                                           MyStatusBar::STATUS_SPINNER);
        }
        else {
            QMessageBox msg;
            msg.setText("Unable to open file.");
            msg.setIcon(QMessageBox::Warning);
            msg.exec();
        }
    }
}

void MainUIWidget::updateTickets()
{
    UnfuddleAPIWrapper *uapi = UnfuddleAPIWrapper::instance();
    connect(uapi, SIGNAL(getTicketsForPersonComplete(QList<unf::Ticket>,int,int)),
            this, SLOT(ticketsUpdated(QList<unf::Ticket>,int,int)));
    uapi->requestGetTicketsForPerson(getCurrentUser().id, getProjectId());
}

void MainUIWidget::ticketsUpdated(const QList<unf::Ticket> &ts, int person_id, int project_id)
{
    UnfuddleAPIWrapper *uapi = UnfuddleAPIWrapper::instance();
    disconnect(uapi, SIGNAL(getTicketsForPersonComplete(QList<unf::Ticket>,int,int)),
               this, SLOT(ticketsUpdated(QList<unf::Ticket>,int,int)));

    if (person_id != getCurrentUser().id || project_id != getProjectId()) {
        qWarning("Tickets for another user or project");
        return;
    }

    QList<unf::Ticket>::const_iterator cur_it = tickets.begin();
    QList<unf::Ticket>::const_iterator new_it = ts.begin();

    QList<const unf::Ticket *> addedTickets;
    QList<const unf::Ticket *> deletedTickets;

    while (cur_it != tickets.end() && new_it != ts.end()) {
        if (cur_it->id == new_it->id) {
            ++cur_it;
            ++new_it;
        } else if (cur_it->id > new_it->id) {
            addedTickets.push_back(&(*new_it));
            ++new_it;
        } else {
            deletedTickets.push_back(&(*cur_it));
            ++cur_it;
        }
    }

    while (new_it != ts.end()) {
        addedTickets.push_back(&(*new_it));
        ++new_it;
    }

    while (cur_it != tickets.end()) {
        deletedTickets.push_back(&(*cur_it));
        ++cur_it;
    }

    if (!addedTickets.isEmpty() || !deletedTickets.isEmpty()) {
        QString notificationText;

        if (!addedTickets.isEmpty()) {
            notificationText.append(tr("Tasks were assigned to you:"));
            notificationText.append("\n");

            foreach (const unf::Ticket *ticket, addedTickets) {
                notificationText.append(QString("#%1 %2\n")
                                        .arg(ticket->number)
                                        .arg(ticket->summary));
            }
        }

        if (!deletedTickets.isEmpty()) {
            notificationText.append(tr("Tasks were reassigned to another user:"));
            notificationText.append("\n");

            foreach (const unf::Ticket *ticket, deletedTickets) {
                notificationText.append(QString("#%1 %2\n")
                                        .arg(ticket->number)
                                        .arg(ticket->summary));
            }
        }

        Application::instance().trayIcon().showMessage(tr("Tasks changes"), notificationText);

        ui->ticketName->clear();
        gotTicketsForPerson(ts, person_id, project_id);
    }
}

void MainUIWidget::setEnableComments(bool enable)
{
    ui->ticketDescription->setVisible(enable);
    ui->label->setVisible(enable);
    ui->addCommentText->setVisible(enable);
    ui->addComment->setVisible(enable);
}
