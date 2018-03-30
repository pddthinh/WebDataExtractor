TEMPLATE = app
TARGET	= ciphe.org.uk

include(../WebDataExtractor.pri)

HEADERS += \
	ciphe_org_uk.h \
#	dblogger.h

SOURCES += \
	ciphe_org_uk.cpp \
#	dblogger.cpp

OTHER_FILES +=

RESOURCES += \
	resource.qrc
