#-------------------------------------------------
#
# Project created by QtCreator 2017-03-28T21:34:38
#
#-------------------------------------------------

QT       += core gui network serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TcpSocketHelper
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        dialog.cpp \
    modbusdialog.cpp \
    dlt645dialog.cpp \
    JQChecksum.cpp

HEADERS  += dialog.h \
    modbusdialog.h \
    dlt645dialog.h \
    JQChecksum.h

FORMS    += dialog.ui \
    modbusdialog.ui \
    dlt645dialog.ui

CONFIG += C++11

CONFIG += sdk_no_version_check

ICON = myapp.icns
#RC_ICONS = myapp.ico
