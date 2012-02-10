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

#include "screensnapeditor.h"
#include "ui_screensnapeditor.h"

#include <QWheelEvent>
#include <QGraphicsSceneWheelEvent>
#include <QGraphicsView>
#include <QUndoStack>
#include <QColorDialog>
#include <QFontDialog>
#include <QInputDialog>

#include <screensnap/urcommands/urcommands.h>
#include <screensnap/primitives.h>
#include <screensnap/objectscontrol/graphicsitemcreator.h>
#include <screensnap/objectscontrol/objectseditor.h>

ScreensnapEditor::ScreensnapEditor(const QPixmap &pixmap, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ScreensnapEditor),
    p_undoStack(new QUndoStack(this)),
    p_editor(new ObjectsEditor(this)),
    p_scene(new QGraphicsScene(this)),
    p_view(new QGraphicsView(p_scene, this))
{
    ui->setupUi(this);

    QGraphicsPixmapItem *pixmapItem = p_scene->addPixmap(pixmap);
    p_scene->setSceneRect(pixmapItem->boundingRect());

    p_view->setRenderHint(QPainter::Antialiasing);
    p_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    p_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    p_view->resize(p_scene->sceneRect().size().toSize());

    ui->scrollArea->setWidget(p_view);
    ui->scrollArea->setBackgroundRole(QPalette::Dark);

    initToolBar();
    initProperties();
    initEditors();

    ui->scrollArea->installEventFilter(this);
    p_view->installEventFilter(this);
    p_scene->installEventFilter(this);

    connect(p_scene, SIGNAL(selectionChanged()), SLOT(selectionChanged()));
}

ScreensnapEditor::~ScreensnapEditor()
{
}

bool ScreensnapEditor::eventFilter(QObject *watched, QEvent *event)
{
    Q_UNUSED(watched);

    if (isEditMode() && event->type() == QEvent::GraphicsSceneMouseDoubleClick) {
        QGraphicsSceneMouseEvent *mouseEvent = static_cast<QGraphicsSceneMouseEvent *>(event);
        QGraphicsItem *item = p_scene->itemAt(mouseEvent->scenePos());

        if (item != 0 && p_scene->selectedItems().contains(item)) {
            GraphicsItemEditor *graphicsEditor = p_editor->editor(item->type());
            GraphicsUtility *interactiveUtility = graphicsEditor->interactiveEditUtility(item, p_scene);

            if (interactiveUtility != NULL) {
                setUtility(interactiveUtility);
            }

            event->accept();
            return true;
        }
    }

    if (event->type() != QEvent::Wheel
            && event->type() != QEvent::GraphicsSceneWheel)
    {
        return false;
    }

    qreal rotate = 0;
    bool hasModifier = false;

    if (event->type() == QEvent::Wheel) {
        QWheelEvent *wheelEvent = static_cast<QWheelEvent *>(event);
        rotate = wheelEvent->delta();
        hasModifier = wheelEvent->modifiers() == Qt::ControlModifier;
    } else {
        QGraphicsSceneWheelEvent *wheelEvent = static_cast<QGraphicsSceneWheelEvent *>(event);
        rotate = wheelEvent->delta();
        hasModifier = wheelEvent->modifiers() == Qt::ControlModifier;
    }

    if (!hasModifier) {
        return false;
    }

    Q_ASSERT(rotate != 0);

    qreal scale = (rotate > 0) ? rotate / 100 : 100 / (-rotate);
    scaleView(scale, scale);

    event->accept();
    return true;
}

void ScreensnapEditor::closeEvent(QCloseEvent *event)
{
    const QTransform &transform = p_view->transform();
    qreal sx = transform.m11();
    qreal sy = transform.m22();
    scaleView(1 / sx, 1 / sy);
    deselectItems();
    emit screenshotChanged(QPixmap::grabWidget(p_view));
    scaleView(sx, sy);
    QMainWindow::closeEvent(event);
}

void ScreensnapEditor::resetControls()
{
    p_creator.reset();
    p_utility.reset();
}

void ScreensnapEditor::setCreator(GraphicsItemCreator *creator)
{
    resetControls();
    p_creator.reset(creator);
    p_scene->installEventFilter(p_creator.data());
    connect(p_creator.data(), SIGNAL(itemCreated(QGraphicsItem*)), SLOT(objectCreated(QGraphicsItem*)));

    deselectItems();
}

void ScreensnapEditor::setUtility(GraphicsUtility *utility)
{
    resetControls();
    p_utility.reset(utility);
    p_scene->installEventFilter(p_utility.data());
    connect(p_utility.data(), SIGNAL(utilityFinished(QUndoCommand*)), SLOT(utilityFinished(QUndoCommand*)));

    deselectItems();
}

void ScreensnapEditor::initProperties()
{
    setPen(QPen(QBrush(Qt::SolidPattern), 4));
    setBrush(QBrush());
    setFont(QFont("DejaVu Sans", 14));
}

void ScreensnapEditor::updateProperties()
{
    p_penWidthAction->setText(tr("Width: %1").arg(pen().width()));

    QPainter painter;
    const QRect colorPlace(4, 18, 16, 4);

    QPixmap pen_color(":/screensnap/icons/pen_color");
    painter.begin(&pen_color);
    painter.setBrush(pen().brush());
    painter.drawRect(colorPlace);
    painter.end();
    p_penColorAction->setIcon(QIcon(pen_color));

    QPixmap brush_color(":/screensnap/icons/brush_color");
    painter.begin(&brush_color);
    painter.setBrush(brush());
    painter.drawRect(colorPlace);
    painter.end();
    p_brushColorAction->setIcon(QIcon(brush_color));

    p_brushSolidAction->setChecked(brush().style() != Qt::SolidPattern);

    QPixmap font_color(":/screensnap/icons/font_color");
    painter.begin(&font_color);
    painter.setBrush(QBrush(fontColor()));
    painter.drawRect(colorPlace);
    painter.end();
    p_fontColorAction->setIcon(QIcon(font_color));
}

void ScreensnapEditor::initToolBar()
{
    QAction *action = p_undoStack->createUndoAction(ui->toolBar);
    action->setIcon(QIcon(":/screensnap/icons/undo"));
    action->setShortcut(QKeySequence::Undo);
    ui->toolBar->addAction(action);

    action = p_undoStack->createRedoAction(ui->toolBar);
    action->setIcon(QIcon(":/screensnap/icons/redo"));
    action->setShortcut(QKeySequence::Redo);
    ui->toolBar->addAction(action);

    ui->toolBar->addSeparator();

    // Objects
    QActionGroup *actionGroup = new QActionGroup(this);
    actionGroup->setExclusive(true);

    action = ui->toolBar->addAction(QIcon(":/screensnap/icons/control"),
                                    tr("Control"), this, SLOT(control()));
    action->setShortcut(tr("C"));
    actionGroup->addAction(action);

    action = ui->toolBar->addAction(QIcon(":/screensnap/icons/brush"),
                                    tr("Brush"), this, SLOT(createBrushes()));
    action->setShortcut(tr("B"));
    actionGroup->addAction(action);

    action = ui->toolBar->addAction(QIcon(":/screensnap/icons/line"),
                                    tr("Line"), this, SLOT(createLines()));
    action->setShortcut(tr("L"));
    actionGroup->addAction(action);

    action = ui->toolBar->addAction(QIcon(":/screensnap/icons/rect"),
                                    tr("Rect"), this, SLOT(createRects()));
    action->setShortcut(tr("R"));
    actionGroup->addAction(action);

    action = ui->toolBar->addAction(QIcon(":/screensnap/icons/arrow"),
                                    tr("Arrow"), this, SLOT(createArrows()));
    action->setShortcut(tr("A"));
    actionGroup->addAction(action);

    action = ui->toolBar->addAction(QIcon(":/screensnap/icons/text"),
                                    tr("Text"), this, SLOT(createTexts()));
    action->setShortcut(tr("T"));
    actionGroup->addAction(action);

    action = ui->toolBar->addAction(QIcon(":/screensnap/icons/crop"),
                                    tr("Crop"), this, SLOT(cropUtility()));
    action->setShortcut(tr("W"));
    actionGroup->addAction(action);

    const QList<QAction *> &actions = actionGroup->actions();
    foreach (QAction *a, actions) {
        a->setCheckable(true);
    }

    ui->toolBar->addSeparator();

    p_removeAction = ui->toolBar->addAction(QIcon(":/screensnap/icons/trash"),
                                            tr("Remove items"), this, SLOT(removeSelectedItems()));
    p_removeAction->setShortcut(QKeySequence::Delete);
    connect(p_scene, SIGNAL(selectionChanged()), SLOT(updateRemoveAction()));
    updateRemoveAction();

    ui->toolBar->addSeparator();

    // Properties
    p_penWidthAction = ui->toolBar->addAction(tr("Pen width"), this, SLOT(penWidthAction_triggered()));
    p_penColorAction = ui->toolBar->addAction(tr("Pen color"), this, SLOT(penColorAction_triggered()));
    p_brushColorAction = ui->toolBar->addAction(tr("Brush color"), this, SLOT(brushColorAction_triggered()));
    p_brushSolidAction = ui->toolBar->addAction(QIcon(":/screensnap/icons/brush_solid"),
                                                tr("Solid"), this, SLOT(brushSolidAction_toggled(bool)));
    p_brushSolidAction->setCheckable(true);
    ui->toolBar->addAction(QIcon(":/screensnap/icons/font"), tr("Font"), this, SLOT(fontAction_triggered()));
    p_fontColorAction = ui->toolBar->addAction(tr("Font color"), this, SLOT(fontColorAction_triggered()));
}

void ScreensnapEditor::initEditors()
{
    p_editor->installEditor(GraphicsBrushItem::Type, new GraphicsBrushItemEditor);
    p_editor->installEditor(QGraphicsLineItem::Type, new GraphicsLineItemEditor);
    p_editor->installEditor(QGraphicsRectItem::Type, new GraphicsRectItemEditor);
    p_editor->installEditor(GraphicsArrowItem::Type, new GraphicsLineItemEditor);
    p_editor->installEditor(QGraphicsTextItem::Type, new GraphicsTextItemEditor);
}

void ScreensnapEditor::changePropertyOfSelectedItems(const QString &property, const QVariant &value)
{
    const QList<QGraphicsItem *> &selectedItems = p_scene->selectedItems();

    if (!selectedItems.isEmpty()) {
        p_undoStack->beginMacro(QString());

        foreach (QGraphicsItem *item, selectedItems) {
            QUndoCommand *cmd = p_editor->setPropertyToObject(property, value, item);
            if (cmd != NULL) {
                p_undoStack->push(cmd);
            }
        }

        p_undoStack->endMacro();
    }
}

void ScreensnapEditor::scaleView(qreal sx, qreal sy)
{
    p_view->scale(sx, sy);
    const QTransform &transform = p_view->transform();
    const QSizeF &size = p_scene->sceneRect().size();
    p_view->resize(qRound(transform.m11() * size.width()) + 2,
                   qRound(transform.m22() * size.height()) + 2);
}

void ScreensnapEditor::deselectItems()
{
    const QList<QGraphicsItem *> &selectedItems = p_scene->selectedItems();

    foreach (QGraphicsItem *item, selectedItems) {
        item->setSelected(false);
    }
}

void ScreensnapEditor::objectCreated(QGraphicsItem *item)
{
    item->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
    p_undoStack->beginMacro(QString());
    p_undoStack->push(new AddCommand(p_scene, item));

    for (QMap<QString, QVariant>::const_iterator it = m_properties.begin(); it != m_properties.end(); ++it) {
        QUndoCommand *cmd = p_editor->setPropertyToObject(it.key(), it.value(), item);
        if (cmd != NULL) {
            p_undoStack->push(cmd);
        }
    }

    p_undoStack->endMacro();
}

void ScreensnapEditor::utilityFinished(QUndoCommand *command)
{
    if (command != NULL) {
        p_undoStack->push(command);
    } else {
        resetControls();
    }
}

void ScreensnapEditor::createBrushes()
{
    setCreator(new GraphicsBrushItemCreator(p_scene));
}

void ScreensnapEditor::createLines()
{
    setCreator(new GraphicsLineItemCreator(p_scene));
}

void ScreensnapEditor::createRects()
{
    setCreator(new GraphicsRectItemCreator(p_scene));
}

void ScreensnapEditor::createArrows()
{
    setCreator(new GraphicsArrowItemCreator(p_scene));
}

void ScreensnapEditor::createTexts()
{
    setCreator(new GraphicsTextItemCreator(font(), p_scene));
}

void ScreensnapEditor::control()
{
    resetControls();
}

void ScreensnapEditor::cropUtility()
{
    setUtility(new CropUtility(p_scene, p_view));
}

void ScreensnapEditor::penColorAction_triggered()
{
    QPen newPen = pen();
    QColorDialog colorDialog(newPen.color(), this);
    if (colorDialog.exec() == QDialog::Accepted) {
        newPen.setColor(colorDialog.selectedColor());
        setPen(newPen);
    }
}

void ScreensnapEditor::penWidthAction_triggered()
{
    QPen newPen = pen();
    QInputDialog inputDialog(this);
    inputDialog.setInputMode(QInputDialog::IntInput);
    inputDialog.setIntRange(0, 24);
    inputDialog.setIntStep(1);
    inputDialog.setIntValue(newPen.width());
    if (inputDialog.exec() == QDialog::Accepted) {
        newPen.setWidth(inputDialog.intValue());
        setPen(newPen);
    }
}

void ScreensnapEditor::brushColorAction_triggered()
{
    QBrush newBrush = brush();
    QColorDialog colorDialog(newBrush.color(), this);
    if (colorDialog.exec() == QDialog::Accepted) {
        newBrush.setColor(colorDialog.selectedColor());
        setBrush(newBrush);
    }
}

void ScreensnapEditor::brushSolidAction_toggled(bool checked)
{
    QBrush newBrush = brush();
    newBrush.setStyle(checked ? Qt::NoBrush : Qt::SolidPattern);
    setBrush(newBrush);
}

void ScreensnapEditor::fontAction_triggered()
{
    QFont newFont = font();
    QFontDialog fontDialog(newFont, this);
    if (fontDialog.exec() == QDialog::Accepted) {
        newFont = fontDialog.currentFont();
        setFont(newFont);
    }
}

void ScreensnapEditor::fontColorAction_triggered()
{
    QColor newFontColor = fontColor();
    QColorDialog colorDialog(newFontColor, this);
    if (colorDialog.exec() == QDialog::Accepted) {
        newFontColor = colorDialog.selectedColor();
        setFontColor(newFontColor);
    }
}

void ScreensnapEditor::removeSelectedItems()
{
    const QList<QGraphicsItem *> &selectedItems = p_scene->selectedItems();

    p_undoStack->beginMacro(QString());

    foreach (QGraphicsItem *item, selectedItems) {
        item->setSelected(false);
        p_undoStack->push(new DelCommand(item));
    }

    p_undoStack->endMacro();
}

void ScreensnapEditor::updateRemoveAction()
{
    p_removeAction->setDisabled(p_scene->selectedItems().isEmpty());
}

void ScreensnapEditor::selectionChanged()
{
    const QList<QGraphicsItem *> &selectedItems = p_scene->selectedItems();

    if (!selectedItems.isEmpty()) {
        QGraphicsItem *item = selectedItems.first();

        const PropertyList &properties = p_editor->editor(item->type())->properties();

        foreach (const QSharedPointer<Property> &property, properties) {
            QMap<QString, QVariant>::iterator iter = m_properties.find(property->name());
            if (iter != m_properties.end()) {
                iter.value() = property->get(item);
            }
        }

        updateProperties();
    }
}

bool ScreensnapEditor::isEditMode()
{
    return p_creator.isNull() && p_utility.isNull();
}
