#-------------------------------------------------
#
# Project created by QtCreator 2011-04-05T19:50:01
#
#-------------------------------------------------

QT       += core gui network xml

TARGET = utracker
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    logindialog.cpp \
    unfuddleapiwrapper.cpp \
    mainuiwidget.cpp \
    unfuddledata.cpp \
    screensnap/property.cpp \
    screensnap/urcommands/urcommands.cpp \
    screensnap/objectscontrol/graphicsitemcreator.cpp \
    screensnap/objectscontrol/objectseditor.cpp \
    screensnap/objectscontrol/graphicsitemeditor.cpp \
    screensnap/objectscontrol/graphicsutility.cpp \
    screensnap/objectscontrol/graphicstextedit.cpp \
    screensnap/editor/screensnapeditor.cpp \
    screensnap/dialog/screensnapdialog.cpp \
    screensnap/screensnap.cpp \
    screensnap/primitives.cpp \
    application.cpp \
    applicationfactory.cpp \
    preferencesdialog.cpp \
    labeledlineedit.cpp \
    qanimationlabel.cpp \
    labeledplaintextedit.cpp \
    choosemultiplefileswidget.cpp \
    choosemultiplefilesentrywidget.cpp \
    noscrolltextedit.cpp \
    mystatusbar.cpp \
    globalshortcut.cpp \
    oneinstance.cpp \
    autorun/autorunimp.cpp \
    autorun/autorun.cpp

HEADERS  += mainwindow.h \
    logindialog.h \
    unfuddleapiwrapper.h \
    mainuiwidget.h \
    unfuddledata.h \
    screensnap/property.h \
    screensnap/urcommands/urcommands.h \
    screensnap/objectscontrol/graphicsitemcreator.h \
    screensnap/objectscontrol/objectseditor.h \
    screensnap/objectscontrol/graphicsitemeditor.h \
    screensnap/objectscontrol/graphicsutility.h \
    screensnap/objectscontrol/graphicstextedit.h \
    screensnap/editor/screensnapeditor.h \
    screensnap/dialog/screensnapdialog.h \
    screensnap/screensnap.h \
    screensnap/primitives.h \
    screensnap/metatypetextdocument.h \
    application.h \
    applicationfactory.h \
    preferencesdialog.h \
    labeledlineedit.h \
    qanimationlabel.h \
    labeledplaintextedit.h \
    choosemultiplefileswidget.h \
    choosemultiplefilesentrywidget.h \
    noscrolltextedit.h \
    mystatusbar.h \
    globalshortcut.h \
    settingskeys.h \
    oneinstance.h \
    autorun/macautorun.h \
    autorun/autorunimp.h \
    autorun/autorun.h

FORMS += \
    logindialog.ui \
    mainuiwidget.ui \
    screensnap/editor/screensnapeditor.ui \
    screensnap/dialog/screensnapdialog.ui \
    preferencesdialog.ui \
    aboutdialog.ui

RESOURCES += \
    resources.qrc

mac {
    HEADERS += macapplication.h \
        autorun/macautorun.h
    OBJECTIVE_SOURCES += macapplication.mm
    LIBS += -framework Cocoa -framework Carbon
    SOURCES += globalshortcut_mac.cpp \
        autorun/macautorun.cpp
    ICON = mac/icon.icns
}
win32 {
    RC_FILE = win/icon.rc
    HEADERS += autorun/winautorun.h
    SOURCES += globalshortcut_win.cpp \
        autorun/winautorun.cpp
}
unix:!macx {
    HEADERS += autorun/x11autorun.h
    SOURCES += globalshortcut_x11.cpp \
        autorun/x11autorun.cpp
}
