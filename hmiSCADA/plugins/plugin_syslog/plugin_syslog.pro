QT += core network script
QT -= gui

TARGET = plugin_syslog
TEMPLATE = lib
CONFIG += plugin
SOURCES += plugin_syslog.cpp \
    ../../../Logger/AbstractAppender.cpp \
    ../../../Logger/AbstractStringAppender.cpp \
    ../../../Logger/ConsoleAppender.cpp \
    ../../../Logger/FileAppender.cpp \
    ../../../Logger/Logger.cpp \
    udp/udpreceiver.cpp \
    udp/udpsender.cpp
HEADERS += plugin_syslog.h \
    ../interfaces/iplugin.h \
    ../interfaces/iobject.h \
    ../../qvmgraph.h \
    ../../../Logger/AbstractAppender.h \
    ../../../Logger/AbstractStringAppender.h \
    ../../../Logger/ConsoleAppender.h \
    ../../../Logger/FileAppender.h \
    ../../../Logger/Logger.h \
    ../../../Logger/OutputDebugAppender.h \
    ../interfaces/imodel.h \
    ../../resourcepath.h \
    udp/udpreceiver.h \
    udp/udpsender.h \
    udp/udpsocket.h

DESTDIR = $$OUT_PWD/../../../bin/plugins

