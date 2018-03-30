TEMPLATE = app
TARGET	= yellowpage_au

include(../WebDataExtractor.pri)

HEADERS += \
	yellowpage_au.h

SOURCES += \
	yellowpage_au.cpp

OTHER_FILES += \
    autralian_state.txt

RESOURCES += \
    YellowpageAU.qrc
