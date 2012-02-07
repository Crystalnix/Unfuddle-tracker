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

#include "choosemultiplefilesentrywidget.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QToolButton>
#include <QFileDialog>
#include "labeledlineedit.h"

bool ChooseMultipleFilesEntryWidget::initialized = false;
QIcon ChooseMultipleFilesEntryWidget::addIcon;
QIcon ChooseMultipleFilesEntryWidget::removeIcon;

ChooseMultipleFilesEntryWidget::ChooseMultipleFilesEntryWidget(QWidget *parent) :
    QWidget(parent),
    mode(MODE_ADD),
    addRemoveButton(new QPushButton(this)),
    filePath(new LabeledLineEdit(this)),
    browseButton(new QToolButton(this))
{
    if (!initialized) {
        addIcon = QIcon(":/images/add_green_button.png");
        removeIcon = QIcon(":/images/minus_red_button.png");
        initialized = true;
    }

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    addRemoveButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    addRemoveButton->setMinimumSize(23, 23);
    addRemoveButton->setMaximumSize(23, 23);
    addRemoveButton->setFlat(true);
    addRemoveButton->setIcon(addIcon);
    addRemoveButton->setIconSize(QSize(23, 23));

    filePath->setLabel("Path of file to attach...");

    browseButton->setText("...");
    browseButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    browseButton->setMinimumSize(27, 23);
    browseButton->setMaximumSize(27, 23);

    layout->addWidget(addRemoveButton, 0);
    layout->addWidget(filePath, 1);
    layout->addWidget(browseButton, 0);

    connect(addRemoveButton, SIGNAL(clicked()), SLOT(onAddRemoveButtonClicked()));
    connect(browseButton, SIGNAL(clicked()), SLOT(onBrowseButtonClicked()));
}

void ChooseMultipleFilesEntryWidget::setMode(Mode _mode)
{
    mode = _mode;
    if (mode == MODE_ADD)
        addRemoveButton->setIcon(addIcon);
    else
        addRemoveButton->setIcon(removeIcon);
}

QString ChooseMultipleFilesEntryWidget::getFilePath() const
{
    return (filePath->text() == filePath->label()) ? "" : filePath->text();
}

void ChooseMultipleFilesEntryWidget::onBrowseButtonClicked()
{
    // FIXME: on windows, dialog execution will not process main loop timers
    // FIXME: default directory name should be chosen depending on the target OS
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                     "/home",
                                                     tr("All files (*.*)"));
    if (!fileName.isEmpty())
        filePath->setText(fileName);
}

void ChooseMultipleFilesEntryWidget::onAddRemoveButtonClicked()
{
    emit addRemoveButtonClicked(this);
}
