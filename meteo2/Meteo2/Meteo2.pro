TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    meteoowm.cpp \
    requeteowm.cpp \
    SNClientHTTP.cpp

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    meteoowm.h \
    requeteowm.h \
    SNClientHTTP.h


win32:LIBS += C:\Qt\Qt5.3.2\Tools\mingw482_32\i686-w64-mingw32\lib\libws2_32.a
