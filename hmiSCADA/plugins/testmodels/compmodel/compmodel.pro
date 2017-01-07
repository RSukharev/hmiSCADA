QT += core
QT -= gui

TARGET = compmodel
TEMPLATE = lib
CONFIG += plugin
SOURCES += compmodel.cpp
HEADERS += compmodel.h \
    ../../interfaces/iplugin.h \
    ../../interfaces/iobject.h \
    ../../interfaces/imodel.h
