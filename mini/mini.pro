#-------------------------------------------------
#
# Project created by QtCreator 2014-12-17T19:44:39
#
#-------------------------------------------------

QT       += core gui network concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mini
TEMPLATE = app
DESTDIR = $$OUT_PWD/../bin

SOURCES += main.cpp\
        mini.cpp \
    udpsignal.cpp \
    tcpserver.cpp \
    tcpclient.cpp \
    ../Logger/AbstractAppender.cpp \
    ../Logger/AbstractStringAppender.cpp \
    ../Logger/ConsoleAppender.cpp \
    ../Logger/FileAppender.cpp \
    ../Logger/Logger.cpp \
    Profiler/profiler.cpp \
    ProfilerNode/agentudp/udp/udpreceiver.cpp \
    ProfilerNode/agentudp/udp/udpsender.cpp \
    ProfilerNode/agentudp/agentudp.cpp \
    ProfilerNode/profilernode.cpp \
    Manager/applicationmanager.cpp

HEADERS  += mini.h \
    udpsignal.h \
    tcpserver.h \
    tcpclient.h \
    ../Logger/AbstractAppender.h \
    ../Logger/AbstractStringAppender.h \
    ../Logger/ConsoleAppender.h \
    ../Logger/FileAppender.h \
    ../Logger/Logger.h \
    ../Logger/OutputDebugAppender.h \
    Profiler/profiler.h \
    ProfilerNode/agentudp/interfaces/inode.h \
    ProfilerNode/agentudp/interfaces/iobject.h \
    ProfilerNode/agentudp/interfaces/iplugin.h \
    ProfilerNode/agentudp/data.h \
    ProfilerNode/agentudp/qvmgraph.h \
    ProfilerNode/agentudp/streamtostring.h \
    ProfilerNode/agentudp/udp/udpreceiver.h \
    ProfilerNode/agentudp/udp/udpsender.h \
    ProfilerNode/agentudp/udp/udpsocket.h \
    ProfilerNode/agentudp/networkproperties.h \
    ProfilerNode/agentudp/agentudp.h \
    ProfilerNode/profilernode.h \
    Manager/applicationmanager.h \
    ProfilerNode/agentudp/request.h \
    ProfilerNode/agentudp/commander.h

FORMS    += mini.ui
