TEMPLATE = app
TARGET	= yellowpage

include(../WebDataExtractor.pri)

HEADERS += \
	yellowpage.h

SOURCES += \
	yellowpage.cpp

OTHER_FILES += \
    us-state-list.txt

RESOURCES += \
    YellowPage.qrc
