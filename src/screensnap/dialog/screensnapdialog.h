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

#ifndef SCREENSNAPDIALOG_H
#define SCREENSNAPDIALOG_H

#include <QDialog>
#include <QScopedPointer>

namespace Ui {
    class ScreensnapDialog;
}

class QComboBox;

class ScreensnapDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ScreensnapDialog(const QPixmap &pixmap, QWidget *parent = 0);
    ~ScreensnapDialog();

    void setScreenAuthor(const QString &author);
    QString screenAuthor() const;

    void setScreenAuthorLogin(const QString &loginString);
    QString screenAuthorLogin() const;

    void setScreenTime(const QString &time);
    QString screenTime() const;

    //void setScreenTitle(const QString &title);
    //QString screenTitle();

    void setScreenDescription(const QString &description);
    QString screenDescription() const;
    QString screenDescriptionLabel() const;

    void setScreenshot(const QPixmap &screenshot);

    QComboBox * getTicketsBox();

signals:
    void draw();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    QScopedPointer<Ui::ScreensnapDialog> ui;
};

#endif // SCREENSNAPDIALOG_H
