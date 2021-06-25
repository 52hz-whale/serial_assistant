#-------------------------------------------------
#
# Project created by QtCreator 2016-07-28T14:26:19
#
#-------------------------------------------------

QT       += core gui serialport sql charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SerialHelper
TEMPLATE = app


SOURCES += main.cpp\
        chartview.cpp \
        mainwindow.cpp

HEADERS  += mainwindow.h \
    chartview.h

FORMS    += mainwindow.ui

CONFIG += mobility
MOBILITY = 

RESOURCES += \
    res.qrc

