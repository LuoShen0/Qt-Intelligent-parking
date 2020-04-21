#-------------------------------------------------
#
# Project created by QtCreator 2020-01-02T15:34:09
#
#-------------------------------------------------

QT       += core gui sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = project
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

#添加头文件
INCLUDEPATH+=/home/gec/aip-cpp-sdk-0.7.10

#添加库文件
LIBS+=  -lcurl -lcrypto  -ljsoncpp
