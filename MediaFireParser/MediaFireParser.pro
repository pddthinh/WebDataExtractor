TARGET = mediafireparser
TEMPLATE = app

##################################################
QT	+= core gui webkit
HEADERS += ../WDELib/WDEInclude.h \
    mainwnd.h

unix: {
	DESTDIR = ../bin
	LIBS += -L../lib -lWDELib
}

win32: {
	DESTDIR = ../WDE-build
	LIBS += $$quote(-L../WDE-build) -lWDELib
}

INCLUDEPATH += $$quote(../WDELib/)
##################################################

##################################################
# Setting use database
contains(DEFINES, USE_DATABASE) {
	QT += sql
}
##################################################

SOURCES += \
	mediafire.cpp \
    mainwnd.cpp \
    main.cpp

HEADERS += 	mediafire.h \
