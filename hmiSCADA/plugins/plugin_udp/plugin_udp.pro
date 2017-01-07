QT += core network
QT -= gui

TARGET = pluginudp
TEMPLATE = lib
CONFIG += plugin
SOURCES += pluginudp.cpp \
    udp/udpsender.cpp \
    udp/udpreceiver.cpp \
    ../../../Logger/AbstractAppender.cpp \
    ../../../Logger/AbstractStringAppender.cpp \
    ../../../Logger/ConsoleAppender.cpp \
    ../../../Logger/FileAppender.cpp \
    ../../../Logger/Logger.cpp
HEADERS += pluginudp.h \
    udp/udpsender.h \
    udp/udpsocket.h \
    udp/udpreceiver.h \
    ../interfaces/iplugin.h \
    ../interfaces/iobject.h \
    ../../qvmgraph.h \
    ../../../Logger/AbstractAppender.h \
    ../../../Logger/AbstractStringAppender.h \
    ../../../Logger/ConsoleAppender.h \
    ../../../Logger/FileAppender.h \
    ../../../Logger/Logger.h \
    ../../../Logger/OutputDebugAppender.h

DESTDIR = $$OUT_PWD/../../../bin/plugins

