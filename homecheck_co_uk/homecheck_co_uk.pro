TEMPLATE = app
TARGET	= homecheck_co

include(../WebDataExtractor.pri)

QT += network

HEADERS += \
    homecheck_co_uk.h \
    cookie.h

SOURCES += \
    homecheck_co_uk.cpp \
    cookie.cpp
