#include "labeledlineedit.h"

LabeledLineEdit::LabeledLineEdit(QWidget *parent) :
    QLineEdit(parent),
    m_label("Input text..."),
    m_isPassword(false)
{
    //showLabel();
    connect(this, SIGNAL(textChanged(QString)), SLOT(onTextChanged(QString)));
}

void LabeledLineEdit::setIsPassword(bool isPassword)
{
    m_isPassword = isPassword;
    if ((isPassword && text() == m_label) || !/*
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

isPassword)
        setEchoMode(QLineEdit::Normal);
    else
        setEchoMode(QLineEdit::Password);
}

void LabeledLineEdit::focusInEvent(QFocusEvent *event)
{
    QLineEdit::focusInEvent(event);
    if (text() == m_label) {
        hideLabel();
    }
}

void LabeledLineEdit::focusOutEvent(QFocusEvent *event)
{
    if (text().isEmpty()) {
        showLabel();
    }

    QLineEdit::focusOutEvent(event);
}

void LabeledLineEdit::onTextChanged(const QString &text)
{
    if (text.isEmpty() && !hasFocus())
        showLabel();
    if (!text.isEmpty() && text != m_label) {
        setStyleSheet("");
        if (m_isPassword)
            setEchoMode(QLineEdit::Password);
    }
}

void LabeledLineEdit::showLabel()
{
    setStyleSheet("color: gray; font-style: italic");
    setText(m_label);

    if (m_isPassword)
        setEchoMode(QLineEdit::Normal);
}

void LabeledLineEdit::hideLabel()
{
    setStyleSheet("");
    clear();

    if (m_isPassword)
        setEchoMode(QLineEdit::Password);
}
