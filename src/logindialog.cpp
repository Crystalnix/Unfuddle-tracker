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

#include "logindialog.h"
#include "ui_logindialog.h"
#include "unfuddleapiwrapper.h"
#include "application.h"

#include <QSettings>
#include <QMovie>
#include <QLabel>
#include <QNetworkReply>
#include <QMessageBox>
#include <QDomDocument>

LoginDialog::LoginDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);

    ui->subdomainCtl->setLabel("Unfuddle Subdomain");
    ui->loginCtl->setLabel("Login");
    ui->passwordCtl->setLabel("Password");
    ui->passwordCtl->setIsPassword(true);
    ui->loginBut->setFocus();

    QSettings settings("Crystalnix", "utracker");
    if (settings.contains("login/subdomain"))
        ui->subdomainCtl->setText(settings.value("login/subdomain").toString());
    if (settings.contains("login/login"))
        ui->loginCtl->setText(settings.value("login/login").toString());
    if (settings.contains("login/savePassword")) {
        ui->rememberPasswordCtl->setCheckState(settings.value("login/savePassword").toBool() ?
                                               Qt::Checked : Qt::Unchecked);

        if (settings.value("login/savePassword").toBool() &&
            settings.contains("login/password"))
                ui->passwordCtl->setText(settings.value("login/password").toString());
    }

    connect(ui->loginBut, SIGNAL(clicked()), this, SLOT(doLogin()));
}

LoginDialog::~LoginDialog()
{

    delete ui;
}

void LoginDialog::doLogin()
{
    /*
    bool hasErrors = false;
    QString errorsText = "Errors:\n";
    if (ui->subdomainCtl->text().length() == 0) {
        errorsText += "- Subdomain must not be empty\n";
        hasErrors = true;
    }
    if (ui->loginCtl->text().length() == 0) {
        errorsText += "- Login must not be empty\n";
        hasErrors = true;
    }
    if (ui->passwordCtl->text().length() == 0) {
        errorsText += "- Password must not be empty\n";
        hasErrors = true;
    }
    */

    /*
    if (hasErrors) {
        ui->errorsLabel->setText(errorsText);
        return;
    } else
        ui->errorsLabel->setText("");
    */

    startWait();

    UnfuddleAPIWrapper *uapi = UnfuddleAPIWrapper::instance();
    uapi->setSubdomain(ui->subdomainCtl->text());
    uapi->setLoginAndPassword(ui->loginCtl->text(), ui->passwordCtl->text());

    uapi->requestGetAccount();
    connect(uapi, SIGNAL(getAccountComplete(unf::Account)), SLOT(gotAccount(unf::Account)));
    connect(uapi, SIGNAL(networkError(int,int,QString)), SLOT(networkError(int,int,QString)));
    connect(uapi, SIGNAL(xmlError(QString)), SLOT(xmlError(QString)));
}

void LoginDialog::gotAccount(const unf::Account &account)
{
    endWait();

    QSettings settings("Crystalnix", "utracker");
    settings.setValue("login/subdomain", ui->subdomainCtl->text());;
    settings.setValue("login/login", ui->loginCtl->text());
    settings.setValue("login/savePassword", ui->rememberPasswordCtl->checkState() == Qt::Checked);
    if (ui->rememberPasswordCtl->checkState() == Qt::Checked)
        settings.setValue("login/password", ui->passwordCtl->text());
    else
        settings.remove("login/password");

    if (account.features.time_tracking) {
        disconnect(UnfuddleAPIWrapper::instance(), 0, this, 0);
        emit loggedIn();
    }
//    else
//        ui->errorsLabel->setText(QString("Error: Time tracking not enabled for this account"));
}

void LoginDialog::networkError(int networkReplyErrorCode, int hasHttpStatus, const QString &errorMsg)
{
    Q_UNUSED(networkReplyErrorCode)
    Q_UNUSED(hasHttpStatus)
    Q_UNUSED(errorMsg)

    endWait();
    Application::instance().mainWindow().setStatus("Error: Invalid credentials.", MyStatusBar::STATUS_ERROR);
//    ui->errorsLabel->setText(errorMsg);
}

void LoginDialog::xmlError(const QString &errorMsg)
{
    Q_UNUSED(errorMsg)

    endWait();
    Application::instance().mainWindow().setStatus("Error: Invalid credentials.", MyStatusBar::STATUS_ERROR);
//    ui->errorsLabel->setText(errorMsg);
}

void LoginDialog::startWait()
{
    ui->loginCtl->setEnabled(false);
    ui->passwordCtl->setEnabled(false);
    ui->rememberPasswordCtl->setEnabled(false);
    ui->loginBut->setEnabled(false);

    ui->loader->show();

    Application::instance().mainWindow().setStatus("Connecting...", MyStatusBar::STATUS_SPINNER);
}

void LoginDialog::endWait()
{
    ui->loginCtl->setEnabled(true);
    ui->passwordCtl->setEnabled(true);
    ui->rememberPasswordCtl->setEnabled(true);
    ui->loginBut->setEnabled(true);

    ui->loader->hide();
}
