TEMPLATE = app
TARGET	= mcs-v2
QT += xml

include(../WebDataExtractor.pri)

SOURCES += main.cpp \
	mcsproduct.cpp \
	mcsmainwnd.cpp \
	mcsinstaller.cpp \
        mcs.cpp

HEADERS += \
	mcsproduct.h \
	mcsmainwnd.h \
	mcsinstaller.h \
        mcs.h
