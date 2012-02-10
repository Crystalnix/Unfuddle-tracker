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

#ifndef SCREENSNAPEDITOR_H
#define SCREENSNAPEDITOR_H

#include <QMainWindow>
#include <QScopedPointer>
#include <QMap>
#include <QVariant>
#include <QPen>
#include <QFont>
#include <QCloseEvent>

#include <screensnap/objectscontrol/graphicsutility.h>

namespace Ui {
    class ScreensnapEditor;
}

class QUndoStack;
class ObjectsCreator;
class ObjectsEditor;
class QGraphicsItem;
class QGraphicsView;
class QGraphicsScene;
class GraphicsItemCreator;
class GraphicsUtility;

class ScreensnapEditor : public QMainWindow
{
    Q_OBJECT

    Q_PROPERTY(QPen pen READ pen WRITE setPen)
    Q_PROPERTY(QBrush brush READ brush WRITE setBrush)
    Q_PROPERTY(QFont font READ font WRITE setFont)

public:
    explicit ScreensnapEditor(const QPixmap &pixmap, QWidget *parent = 0);
    ~ScreensnapEditor();

signals:
    void screenshotChanged(const QPixmap &screenshot);

protected:
    bool eventFilter(QObject *watched, QEvent *event);
    void closeEvent(QCloseEvent *event);

private:
    QScopedPointer<Ui::ScreensnapEditor> ui;
    QUndoStack *p_undoStack;
    QScopedPointer<GraphicsItemCreator> p_creator;
    ObjectsEditor *p_editor;
    QGraphicsScene *p_scene;
    QGraphicsView *p_view;
    QScopedPointer<GraphicsUtility> p_utility;

    // Properties
    QMap<QString, QVariant> m_properties;

    inline QPen pen() const { return m_properties["pen"].value<QPen>(); }
    inline void setPen(const QPen &pen) { m_properties["pen"] = pen;
                                          changePropertyOfSelectedItems("pen", pen);
                                          updateProperties(); }

    inline QBrush brush() const { return m_properties["brush"].value<QBrush>(); }
    inline void setBrush(const QBrush &brush) { m_properties["brush"] = brush;
                                                changePropertyOfSelectedItems("brush", brush);
                                                updateProperties(); }

    inline QFont font() const { return m_properties["font"].value<QFont>(); }
    inline void setFont(const QFont &font) { m_properties["font"] = font;
                                             changePropertyOfSelectedItems("font", font);
                                             updateProperties(); }

    inline QColor fontColor() const { return m_properties["font_color"].value<QColor>(); }
    inline void setFontColor(const QColor &fontColor) { m_properties["font_color"] = fontColor;
                                                        changePropertyOfSelectedItems("font_color", fontColor);
                                                        updateProperties(); }

    QAction *p_removeAction;
    QAction *p_penColorAction;
    QAction *p_penWidthAction;
    QAction *p_brushColorAction;
    QAction *p_brushSolidAction;
    QAction *p_fontAction;
    QAction *p_fontColorAction;

    void resetControls();
    void setCreator(GraphicsItemCreator *creator);
    void setUtility(GraphicsUtility *utility);
    void initProperties();
    void updateProperties();
    void initToolBar();
    void initEditors();
    void changePropertyOfSelectedItems(const QString &property, const QVariant &value);
    void scaleView(qreal sx, qreal sy);
    void deselectItems();

    inline bool isEditMode();

private slots:
    void objectCreated(QGraphicsItem *item);
    void utilityFinished(QUndoCommand *command);

    // Toolbar slots
    void control();
    void createBrushes();
    void createLines();
    void createRects();
    void createArrows();
    void createTexts();
    void cropUtility();

    // Properties slots
    void penColorAction_triggered();
    void penWidthAction_triggered();
    void brushColorAction_triggered();
    void brushSolidAction_toggled(bool checked);
    void fontAction_triggered();
    void fontColorAction_triggered();

    void removeSelectedItems();
    void updateRemoveAction();
    void selectionChanged();
};

#endif // SCREENSNAPEDITOR_H
