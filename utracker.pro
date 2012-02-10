#-------------------------------------------------
#
# Project created by QtCreator 2011-04-05T19:50:01
#
#-------------------------------------------------

QT       += core gui network xml

TARGET = utracker
TEMPLATE = app


SOURCES += src/main.cpp\
        src/mainwindow.cpp \
    src/logindialog.cpp \
    src/unfuddleapiwrapper.cpp \
    src/mainuiwidget.cpp \
    src/unfuddledata.cpp \
    src/screensnap/property.cpp \
    src/screensnap/urcommands/urcommands.cpp \
    src/screensnap/objectscontrol/graphicsitemcreator.cpp \
    src/screensnap/objectscontrol/objectseditor.cpp \
    src/screensnap/objectscontrol/graphicsitemeditor.cpp \
    src/screensnap/objectscontrol/graphicsutility.cpp \
    src/screensnap/objectscontrol/graphicstextedit.cpp \
    src/screensnap/editor/screensnapeditor.cpp \
    src/screensnap/dialog/screensnapdialog.cpp \
    src/screensnap/screensnap.cpp \
    src/screensnap/primitives.cpp \
    src/application.cpp \
    src/applicationfactory.cpp \
    src/preferencesdialog.cpp \
    src/labeledlineedit.cpp \
    src/qanimationlabel.cpp \
    src/labeledplaintextedit.cpp \
    src/choosemultiplefileswidget.cpp \
    src/choosemultiplefilesentrywidget.cpp \
    src/noscrolltextedit.cpp \
    src/mystatusbar.cpp \
    src/globalshortcut.cpp \
    src/oneinstance.cpp \
    src/autorun/autorunimp.cpp \
    src/autorun/autorun.cpp

HEADERS  += src/mainwindow.h \
    src/logindialog.h \
    src/unfuddleapiwrapper.h \
    src/mainuiwidget.h \
    src/unfuddledata.h \
    src/screensnap/property.h \
    src/screensnap/urcommands/urcommands.h \
    src/screensnap/objectscontrol/graphicsitemcreator.h \
    src/screensnap/objectscontrol/objectseditor.h \
    src/screensnap/objectscontrol/graphicsitemeditor.h \
    src/screensnap/objectscontrol/graphicsutility.h \
    src/screensnap/objectscontrol/graphicstextedit.h \
    src/screensnap/editor/screensnapeditor.h \
    src/screensnap/dialog/screensnapdialog.h \
    src/screensnap/screensnap.h \
    src/screensnap/primitives.h \
    src/screensnap/metatypetextdocument.h \
    src/application.h \
    src/applicationfactory.h \
    src/preferencesdialog.h \
    src/labeledlineedit.h \
    src/qanimationlabel.h \
    src/labeledplaintextedit.h \
    src/choosemultiplefileswidget.h \
    src/choosemultiplefilesentrywidget.h \
    src/noscrolltextedit.h \
    src/mystatusbar.h \
    src/globalshortcut.h \
    src/settingskeys.h \
    src/oneinstance.h \
    src/autorun/macautorun.h \
    src/autorun/autorunimp.h \
    src/autorun/autorun.h

FORMS += \
    src/logindialog.ui \
    src/mainuiwidget.ui \
    src/screensnap/editor/screensnapeditor.ui \
    src/screensnap/dialog/screensnapdialog.ui \
    src/preferencesdialog.ui \
    src/aboutdialog.ui

RESOURCES += \
    resources/resources.qrc

INCLUDEPATH += src/

mac {
    HEADERS += src/macapplication.h \
        src/autorun/macautorun.h
    OBJECTIVE_SOURCES += src/macapplication.mm
    LIBS += -framework Cocoa -framework Carbon
    SOURCES += src/globalshortcut_mac.cpp \
        src/autorun/macautorun.cpp
    ICON = resources/mac/icon.icns
}
win32 {
    RC_FILE = resources/win/icon.rc
    HEADERS += src/autorun/winautorun.h
    SOURCES += src/globalshortcut_win.cpp \
        src/autorun/winautorun.cpp
}
unix:!macx {
    HEADERS += src/autorun/x11autorun.h
    SOURCES += src/globalshortcut_x11.cpp \
        src/autorun/x11autorun.cpp
}
