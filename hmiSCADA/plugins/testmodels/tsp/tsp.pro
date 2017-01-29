QT += core
QT -= gui

TARGET = tsp
TEMPLATE = lib
CONFIG += plugin
SOURCES += tsp.cpp \
    ../../../../Logger/AbstractAppender.cpp \
    ../../../../Logger/AbstractStringAppender.cpp \
    ../../../../Logger/ConsoleAppender.cpp \
    ../../../../Logger/FileAppender.cpp \
    ../../../../Logger/Logger.cpp
HEADERS += tsp.h \
    ../../interfaces/iplugin.h \
    ../../interfaces/iobject.h \
    ../../interfaces/imodel.h \
    ../../../../Logger/AbstractAppender.h \
    ../../../../Logger/AbstractStringAppender.h \
    ../../../../Logger/ConsoleAppender.h \
    ../../../../Logger/FileAppender.h \
    ../../../../Logger/Logger.h \
    ../../../../Logger/OutputDebugAppender.h

DESTDIR = $$OUT_PWD/../../../../bin/plugins
