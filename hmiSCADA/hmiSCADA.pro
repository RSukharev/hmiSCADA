QT       += core gui network concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
#qtHaveModule(opengl):
QT += opengl

QMAKE_MAC_SDK.macosx.version = 10.11

CONFIG -= app_bundle

CONFIG += qt

HEADERS += \
    controller.h \
    data.h \
    visualiser.h \
    qvmgraph.h \
    scene/figures/node.h \
    scene/guiproperties.h \
    plugins/interfaces/iobject.h \
    plugins/interfaces/iplugin.h \
    plugins/networkproperties.h \
    Logger/AbstractAppender.h \
    Logger/AbstractStringAppender.h \
    Logger/ConsoleAppender.h \
    Logger/FileAppender.h \
    Logger/Logger.h \
    Logger/OutputDebugAppender.h \
    plugins/request.h \
    scene/mainwidget.h \
    scene/figures/arc.h \
    commander.h \
    resourcepath.h \
    objectmap.h \
    producer.h \
    streamstring.h \
    scene/pixmapholder.h

TEMPLATE = app

TARGET = hmiSCADA

SOURCES += main.cpp \
    controller.cpp \
    visualiser.cpp \
    scene/figures/node.cpp \
    Logger/AbstractAppender.cpp \
    Logger/AbstractStringAppender.cpp \
    Logger/ConsoleAppender.cpp \
    Logger/FileAppender.cpp \
    Logger/Logger.cpp \
    scene/mainwidget.cpp \
    scene/figures/arc.cpp

FORMS += \
    mainwidget.ui

DISTFILES +=

