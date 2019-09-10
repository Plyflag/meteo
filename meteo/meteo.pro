#-------------------------------------------------
#
# Project created by QtCreator 2019-09-06T15:28:23
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = meteo
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    requeteowm.cpp \
    SNClientHTTP.cpp

HEADERS += \
    requeteowm.h \
    SNClientHTTP.h
    win32:LIBS += C:\Qt\Qt5.3.2\Tools\mingw482_32\i686-w64-mingw32\lib\libws2_32.a
