#-------------------------------------------------
#
# Project created by QtCreator 2010-11-13T08:13:08
#
#-------------------------------------------------

include(../WebDataExtractor.pri)

QT       += webkit testlib gui

TARGET = utwdelibtest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += utwdelibtest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
