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

#ifndef NOSCROLLTEXTEDIT_H
#define NOSCROLLTEXTEDIT_H

#include <QTextBrowser>

class NoScrollTextEdit : public QTextBrowser
{
    Q_OBJECT
public:
    explicit NoScrollTextEdit(QWidget *parent = 0);

    QSize sizeHint() const;

    void setLabel(const QString &label) { m_label = label; if (toPlainText().isEmpty()) showLabel(); }
    QString label() const { return m_label; }
signals:

public slots:
private slots:
    void onTextChanged();
    //void fitHeightToDocument();
    //void clickedOnALink(const QUrl &url);
protected:
    void resizeEvent(QResizeEvent *event);
    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);
private:
    QString m_label;

    void showLabel();
    void hideLabel();
//    bool ownResize;
};

#endif // NOSCROLLTEXTEDIT_H
