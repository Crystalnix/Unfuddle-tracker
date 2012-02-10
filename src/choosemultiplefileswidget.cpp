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

#include "choosemultiplefileswidget.h"
#include "choosemultiplefilesentrywidget.h"

#include <QVBoxLayout>
#include <QFile>

ChooseMultipleFilesWidget::ChooseMultipleFilesWidget(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    ChooseMultipleFilesEntryWidget *firstEntry = new ChooseMultipleFilesEntryWidget(this);
    entries.push_back(firstEntry);
    layout->addWidget(firstEntry, 0);
    firstEntry->show();

    connect(firstEntry, SIGNAL(addRemoveButtonClicked(ChooseMultipleFilesEntryWidget*)), SLOT(onAddRemove(ChooseMultipleFilesEntryWidget*)));
}

void ChooseMultipleFilesWidget::onAddRemove(ChooseMultipleFilesEntryWidget *eventSource)
{
    if (eventSource->getMode() == ChooseMultipleFilesEntryWidget::MODE_ADD) {
        eventSource->setMode(ChooseMultipleFilesEntryWidget::MODE_REMOVE);

        ChooseMultipleFilesEntryWidget *newEntry = new ChooseMultipleFilesEntryWidget(this);
        entries.push_back(newEntry);
        layout()->addWidget(newEntry);
        newEntry->show();

        connect(newEntry, SIGNAL(addRemoveButtonClicked(ChooseMultipleFilesEntryWidget*)), SLOT(onAddRemove(ChooseMultipleFilesEntryWidget*)));
    }
    else {
        entries.removeOne(eventSource);
        layout()->removeWidget(eventSource);
        delete eventSource;
    }
}

QList<QString> ChooseMultipleFilesWidget::getFileNames() const
{
    QList<QString> res;

    foreach (ChooseMultipleFilesEntryWidget *w, entries) {
        res.push_back(w->getFilePath());
    }

    return res;
}

QList<QString> ChooseMultipleFilesWidget::getExistingFileNames() const
{
    QList<QString> res;

    foreach (ChooseMultipleFilesEntryWidget *w, entries) {
        if (QFile::exists(w->getFilePath()))
            res.push_back(w->getFilePath());
    }

    return res;
}
