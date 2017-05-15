#-------------------------------------------------
#
# Project created by QtCreator 2017-04-13T10:49:54
#
#-------------------------------------------------

lessThan(QT_MAJOR_VERSION,5): error("Requires Qt 5")

QT += core gui widgets

TARGET = ace
TEMPLATE = lib
VERSION = 9999
CONFIG += staticlib

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

QMAKE_CXXFLAGS += -std=c++11

unix|win32: LIBS += -lOpenCL

SOURCES += opencldevicedialog.cpp \
    opencldevicemodel.cpp \
    opencldevice.cpp \
    openclevent.cpp \
    openclprogram.cpp \
    openclkernel.cpp \
    utilities.cpp \
    exception.cpp \
    mainwindow.cpp \
    abstractanalyticfactory.cpp \
    setupanalyticdialog.cpp \
    abstractdata.cpp \
    datastream.cpp \
    dataobject.cpp \
    abstractdatafactory.cpp \
    application.cpp \
    opencl.cpp \
    datamanager.cpp \
    datareference.cpp \
    abstractanalytic.cpp

HEADERS  += opencldevicedialog.h \
    opencldevicemodel.h \
    opencldevice.h \
    openclevent.h \
    openclbuffer.h \
    openclprogram.h \
    openclkernel.h \
    utilities.h \
    exception.h \
    mainwindow.h \
    abstractanalyticfactory.h \
    abstractanalytic.h \
    setupanalyticdialog.h \
    abstractdata.h \
    datastream.h \
    dataobject.h \
    abstractdatafactory.h \
    AceCore.h \
    AceOpenCL.h \
    application.h \
    opencl.h \
    datamanager.h \
    datareference.h
