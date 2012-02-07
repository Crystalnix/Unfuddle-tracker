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

#include "screensnapdialog.h"
#include "ui_screensnapdialog.h"

#include <QMouseEvent>

ScreensnapDialog::ScreensnapDialog(const QPixmap &pixmap, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ScreensnapDialog)
{
    ui->setupUi(this);
    ui->descriptionEdit->setLabel("Comment...");

    setScreenshot(pixmap);

    ui->screenLabel->installEventFilter(this);

    connect(ui->drawButton, SIGNAL(clicked()), SIGNAL(draw()));
}

ScreensnapDialog::~ScreensnapDialog()
{
}

void ScreensnapDialog::setScreenAuthor(const QString &author)
{
    ui->nameLabel->setText(author);
}

QString ScreensnapDialog::screenAuthor() const
{
    return ui->nameLabel->text();
}

void ScreensnapDialog::setScreenAuthorLogin(const QString &loginString)
{
    ui->userLabel->setText("(" + loginString + ")");
}

QString ScreensnapDialog::screenAuthorLogin() const
{
    QString t = ui->userLabel->text();
    return t.mid(1, t.length() - 2);    // remove brackets
}


void ScreensnapDialog::setScreenTime(const QString &time)
{
    ui->dateTimeLabel->setText(time);
}

QString ScreensnapDialog::screenTime() const
{
    return ui->dateTimeLabel->text();
}

//void ScreensnapDialog::setScreenTitle(const QString &title)
//{
//    ui->titleEdit->setText(title);
//}

//QString ScreensnapDialog::screenTitle()
//{
//    return ui->titleEdit->text();
//}

void ScreensnapDialog::setScreenDescription(const QString &description)
{
    ui->descriptionEdit->setText(description);
}

QString ScreensnapDialog::screenDescription() const
{
    return ui->descriptionEdit->toPlainText();
}

QString ScreensnapDialog::screenDescriptionLabel() const
{
    return ui->descriptionEdit->label();
}

void ScreensnapDialog::setScreenshot(const QPixmap &screenshot)
{
    ui->screenLabel->setPixmap(screenshot.scaledToWidth(320, Qt::SmoothTransformation));
}

bool ScreensnapDialog::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->screenLabel
            && event->type() == QEvent::MouseButtonDblClick)
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            emit draw();
        }
    }

    return false;
}

QComboBox * ScreensnapDialog::getTicketsBox()
{
    return ui->ticketsBox;
}
