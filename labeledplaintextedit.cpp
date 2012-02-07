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

#include "labeledplaintextedit.h"

LabeledPlainTextEdit::LabeledPlainTextEdit(QWidget *parent) :
    QPlainTextEdit(parent),
    m_label("Input text...")
{
    //showLabel();
    connect(this, SIGNAL(textChanged()), SLOT(onTextChanged()));
}

void LabeledPlainTextEdit::focusInEvent(QFocusEvent *event)
{
    QPlainTextEdit::focusInEvent(event);
    if (toPlainText() == m_label) {
        hideLabel();
    }
}

void LabeledPlainTextEdit::focusOutEvent(QFocusEvent *event)
{
    if (toPlainText().isEmpty()) {
        showLabel();
    }

    QPlainTextEdit::focusOutEvent(event);
}

void LabeledPlainTextEdit::onTextChanged()
{
    QString text = toPlainText();
    if (text.isEmpty() && !hasFocus())
        showLabel();
    if (!text.isEmpty() && text != m_label)
        setStyleSheet("");
}

void LabeledPlainTextEdit::showLabel()
{
    setStyleSheet("color: gray; font-style: italic");
    setPlainText(m_label);
}

void LabeledPlainTextEdit::hideLabel()
{
    setStyleSheet("");
    clear();
}
