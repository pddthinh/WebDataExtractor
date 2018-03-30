TEMPLATE = app
TARGET	= architecture_com

include(../WebDataExtractor.pri)

HEADERS += \
    chartered_practice.h \
    architecture_gui.h \
    architecture_base.h \
    chartered_member.h

SOURCES += \
    chartered_practice.cpp \
    architecture_gui.cpp \
    main.cpp \
    architecture_base.cpp \
    chartered_member.cpp

RESOURCES += \
    architecture_com.qrc
