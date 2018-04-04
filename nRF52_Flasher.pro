#-------------------------------------------------
#
# Project created by QtCreator 2018-04-02T12:42:32
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++14

TARGET = nRF52-Flasher
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    flashingthread.cpp

HEADERS += \
        mainwindow.h \
    flashingthread.h

FORMS += \
        mainwindow.ui

RESOURCES += \
    resources.qrc

DISTFILES += QT_DEPRECATED_WARNINGS


win32:{
    CONFIG += windows

    RC_ICONS = images/logo.ico
    QMAKE_TARGET_COMPANY = Vega-Absolute
    QMAKE_TARGET_DESCRIPTION = nRF52-Flasher
    QMAKE_TARGET_COPYRIGHT = Vega-Absolute
    QMAKE_TARGET_PRODUCT = nRF52-Flasher
    RC_LANG = 0x0419
    RC_CODEPAGE = 0x04E3
}
