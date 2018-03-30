SOURCES += \
	main.cpp \
    restaurantcom.cpp

HEADERS += \
    restaurantcom.h

##################################################
QT	+= core gui webkit
HEADERS += ../WDELib/WDEInclude.h \

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
contains( DEFINES, USE_DATABASE ) {
	QT += sql
}
##################################################
