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

#include "mystatusbar.h"
#include "qanimationlabel.h"
#include "application.h"

#include <QStyle>

MyStatusBar::MyStatusBar(QWidget *parent) :
    QStatusBar(parent),
    spinner(new QAnimationLabel(":/images/ajax-loader.gif")),
    errorIcon(new QLabel),
    statusMsg(new QLabel)
{
    QIcon _errorIcon = QApplication::style()->standardIcon(QStyle::SP_MessageBoxCritical);
    errorIcon->setPixmap(_errorIcon.pixmap(16, 16));
    errorIcon->setScaledContents(false);
    errorIcon->setFixedSize(16, 16);
    errorIcon->setAlignment(Qt::AlignLeft);

    statusMsg->setMaximumHeight(16);

    this->addWidget(spinner, 0);
    this->addWidget(errorIcon, 0);
    this->addWidget(statusMsg, 1);

    spinner->hide();
    errorIcon->hide();
}

void MyStatusBar::setStatus(const QString &message, Status s)
{
    switch (s) {
    case STATUS_ERROR:

        spinner->hide();
        spinner->stop();
        errorIcon->show();
        statusMsg->setStyleSheet("font-size: 10px; color: red");
        break;
    case STATUS_SPINNER:

        spinner->show();
        spinner->start();
        errorIcon->hide();
        statusMsg->setStyleSheet("font-size: 10px; color: black");
        break;
    default:
        spinner->hide();
        spinner->stop();
        errorIcon->hide();
        statusMsg->setStyleSheet("font-size: 10px; color: black");
        break;
    }

    statusMsg->setText(message);
}
