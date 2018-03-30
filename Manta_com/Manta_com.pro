TEMPLATE = app
TARGET	= Manta_com

include(../WebDataExtractor.pri)

HEADERS += \
	Manta_com.h

SOURCES += \
	Manta_com.cpp

OTHER_FILES += \
    us-state-name-full.txt

RESOURCES += \
    Manta.qrc
