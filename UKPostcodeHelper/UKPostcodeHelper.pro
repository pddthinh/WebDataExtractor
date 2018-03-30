TEMPLATE = app
TARGET	= ukpc
QT += sql

include(../WebDataExtractor.pri)

HEADERS += \
    postcodedb.h

SOURCES += \
    postcodedb.cpp \
    main.cpp
