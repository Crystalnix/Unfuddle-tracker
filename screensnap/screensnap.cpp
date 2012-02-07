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

#include "screensnap.h"
#include "mainwindow.h"
#include "mainuiwidget.h"
#include "unfuddledata.h"
#include "unfuddleapiwrapper.h"
#include "application.h"
#include "settingskeys.h"

#include <QDateTime>
#include <QDesktopWidget>
#include <QMenu>
#include <QMessageBox>
#include <QScopedPointer>
#include <QBuffer>
#include <QSystemTrayIcon>
#include <QComboBox>
#include <QSettings>

Screensnap::Screensnap(QObject *parent) :
    QObject(parent)
{
}

QMenu * Screensnap::createScreensnapMenu()
{
    QScopedPointer<QMenu> menu(new QMenu(tr("Screensnap")));

    // Adding action to screensnap menu for snapping all windows
    QAction *action = menu->addAction(tr("Snap the whole desktop"),
                                      this, SLOT(snapWindowByAction()));
    action->setData(-1);

    // Adding actions to screensnap menu for snapping every window
    int screenCount = QApplication::desktop()->screenCount();
    if (screenCount > 1) {
        for (int i = 0; i < screenCount; ++i) {
            action = menu->addAction(tr("Snap display # %1").arg(i + 1),
                                     this, SLOT(snapWindowByAction()));
            action->setData(i);
        }
    }

    return menu.take();
}

void Screensnap::setActiveDisplayShortcut(const QKeySequence &key)
{
    m_activeDisplayShortcut.setShortcut(key);
}

void Screensnap::enableShortcuts()
{
    connect(&m_activeDisplayShortcut, SIGNAL(activated()),
            this, SLOT(snapActiveWindow()));
}

void Screensnap::disableShortcuts()
{
    disconnect(&m_activeDisplayShortcut, SIGNAL(activated()),
               this, SLOT(snapActiveWindow()));
}

void Screensnap::snapWindowByAction()
{
    QAction *action = static_cast<QAction *>(sender());
    Q_ASSERT(action != NULL);

    bool isInt = false;
    int screenNumber = action->data().toInt(&isInt);
    Q_ASSERT(isInt);

    QDesktopWidget *desktop = QApplication::desktop();

    QWidget *screen = desktop->screen(screenNumber);

    if (screen == NULL) {
        QMessageBox::warning(p_dialog.data(), tr("Warning"),
                             tr("There are no screens with this number."));
        return;
    }

    QPixmap screenshot;

    if (desktop->isVirtualDesktop() && screenNumber == -1) {
        screenshot = QPixmap::grabWindow(screen->winId(),
                                         screen->x(), screen->y(),
                                         screen->width(), screen->height());
    } else {
        const QRect &screenRect = desktop->screenGeometry(screenNumber);
        screenshot = QPixmap::grabWindow(screen->winId(),
                                         screenRect.x(), screenRect.y(),
                                         screenRect.width(), screenRect.height());
    }

    showDialog(screenshot);
}

void Screensnap::snapActiveWindow()
{
    QDesktopWidget *desktop = QApplication::desktop();

    const QPoint &pos = QCursor::pos();
    const QRect &screenRect = desktop->screenGeometry(pos);

    QWidget *screen = desktop->screen();

    showDialog(QPixmap::grabWindow(screen->winId(),
                                   screenRect.x(), screenRect.y(),
                                   screenRect.width(), screenRect.height()));
}

void Screensnap::showDialog(const QPixmap &pixmap)
{
    m_screenshot = pixmap;
    if (p_dialog.isNull()) {
        p_dialog.reset(new ScreensnapDialog(m_screenshot));
        if (const MainUIWidget *mainui =
            Application::instance().mainWindow().getMainUI()) {
            unf::Person p = mainui->getCurrentUser();
            p_dialog->setScreenAuthor(p.first_name + " " + p.last_name);
            p_dialog->setScreenAuthorLogin(UnfuddleAPIWrapper::instance()->getLogin() + "@" +
                                           UnfuddleAPIWrapper::instance()->getSubdomain());

            // Inserting tickets to dialog combo box.
            const QList<unf::Ticket> &tickets = mainui->getTickets();
            QComboBox *ticketsBox = p_dialog->getTicketsBox();
            int currentTicket = mainui->getTicketId();

            foreach(const unf::Ticket &ticket, tickets) {
                ticketsBox->addItem(QString("#%1 - %2").arg(ticket.number).arg(ticket.summary));

                if (currentTicket == ticket.id) {
                    ticketsBox->setCurrentIndex(ticketsBox->count() - 1);
                }
            }
        }
        p_dialog->setScreenTime(QDateTime::currentDateTime().toString("MM/dd/yyyy hh:mm:ss"));
        connect(p_dialog.data(), SIGNAL(draw()), SLOT(on_dialog_draw()));
        connect(p_dialog.data(), SIGNAL(accepted()), SLOT(on_dialog_accepted()));
        connect(p_dialog.data(), SIGNAL(rejected()), SLOT(on_dialog_rejected()));
        p_dialog->show();
        p_dialog->raise();
    } else {
        QMessageBox::warning(p_dialog.data(), tr("Warning"),
                             tr("Only one Screensnap at the time is allowed."));
    }
}

void Screensnap::finishScreensnap()
{
    p_editor.reset();
    p_dialog.reset();
    m_screenshot = QPixmap();
}

void Screensnap::on_dialog_accepted()
{
    UnfuddleAPIWrapper *uapi = UnfuddleAPIWrapper::instance();
    if (const MainUIWidget *mainui =
        Application::instance().mainWindow().getMainUI()) {
        QString commentBody;
//        if (!p_dialog->screenTitle().isEmpty())
//            commentBody += p_dialog->screenTitle() + "\n\n";
        if (!p_dialog->screenDescription().isEmpty() &&
            p_dialog->screenDescription() != p_dialog->screenDescriptionLabel())
            commentBody += p_dialog->screenDescription();
        if (commentBody.isEmpty())
            commentBody = QString("User (%1@%2) snapshot")
                                .arg(uapi->getLogin(), uapi->getSubdomain());

        int ticketIndex = p_dialog->getTicketsBox()->currentIndex();
        const QList<unf::Ticket> &tickets = mainui->getTickets();

        if (ticketIndex != -1 && ticketIndex < tickets.count()) {
            uapi->requestAddTicketComment(mainui->getProjectId(),
                                          tickets.at(ticketIndex).id,
                                          commentBody);
            connect(uapi, SIGNAL(addTicketCommentComplete(int,int,int)), SLOT(addedTicketComment(int,int,int)));
        } else {
            QMessageBox::warning(p_dialog.data(), tr("No ticket"), tr("This ticket does not exist"));
            finishScreensnap();
        }
    }
}

void Screensnap::on_dialog_rejected()
{
    finishScreensnap();
}

void Screensnap::on_dialog_draw()
{
    if (p_editor.isNull()) {
        p_editor.reset(new ScreensnapEditor(m_screenshot, p_dialog.data()));
        connect(p_editor.data(), SIGNAL(screenshotChanged(QPixmap)),
                SLOT(on_editor_screenshotChanged(QPixmap)));
    }

    p_editor->show();
    p_editor->raise();
}

void Screensnap::on_editor_screenshotChanged(const QPixmap &pixmap)
{
    m_screenshot = pixmap;
    p_dialog->setScreenshot(pixmap);
}

void Screensnap::addedTicketComment(int project_id, int ticket_id, int comment_id)
{
    UnfuddleAPIWrapper *uapi = UnfuddleAPIWrapper::instance();
    disconnect(uapi, 0, this, 0);
    QString displayFileName = QString("screensnap-%1-%2.png")
                              .arg(uapi->getLogin())
                              .arg(p_dialog->screenTime()
                                        .replace(QString("/"), QString("-")));

    QByteArray bytes;
    QBuffer buffer(&bytes);
    buffer.open(QIODevice::WriteOnly);
    m_screenshot.save(&buffer, "PNG");
    buffer.close();

    uapi->requestAddAttachmentToTicketComment(project_id, ticket_id, comment_id,
                                              displayFileName, bytes, "image/png");
    connect(uapi, SIGNAL(addAttachmentToTicketCommentComplete(int,int,int)),
            SLOT(addedAttachmentToTicketComment(int,int,int)));
}

void Screensnap::addedAttachmentToTicketComment(int project_id, int ticket_id, int comment_id)
{
    Q_UNUSED(project_id)
    Q_UNUSED(ticket_id)
    Q_UNUSED(comment_id)

    UnfuddleAPIWrapper *uapi = UnfuddleAPIWrapper::instance();
    disconnect(uapi, 0, this, 0);

    finishScreensnap();
    Application::instance().trayIcon().showMessage("Upload finished",
        "Uploading snapshot image file as a ticket comment attachment is finished.");
}
