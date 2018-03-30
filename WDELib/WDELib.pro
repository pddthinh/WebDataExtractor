TEMPLATE = lib

include(../WDEConfig.pri)

VERSION = 1.2.0

TARGET  = $$coreLib
DESTDIR = $$buildDir

#DEFINES += USE_JQUERY

win32 {
	DEFINES += WDE_LIB
	CONFIG += dll
}

QT	+= core gui webkit
QT	+= xml
QT	+= network
QT	+= sql

SOURCES += \
	BaseWebEngine/basewebengine.cpp \
	BaseWebEngine/webengine.cpp \
	Logger/logger.cpp \
	Logger/loggertask.cpp \
	Logger/dblogger.cpp \
	DataEntry/dataentry.cpp \
	Utilities/utilities.cpp \
	Database/sqlquerybuilder.cpp \
	Logger/xmllogger.cpp \
	Extension/extfileloading.cpp \
	Utilities/ContactSeeker.cpp \
	Extension/textfilterwidget.cpp \
	Utilities/xmlutil.cpp \
	Database/dbmanager.cpp

HEADERS  += \
	WDE.h \
	WDEInclude.h \
	WDEDef.h \
	BaseWebEngine/basewebengine.h \
	BaseWebEngine/webengine.h \
	Logger/logger.h \
	Logger/loggertask.h \
	Logger/dblogger.h \
	DataEntry/dataentry.h \
	Utilities/utilities.h \
	Database/sqlquerybuilder.h \
	Logger/xmllogger.h \
	Extension/extfileloading.h \
	Utilities/ContactSeeker.h \
	Extension/textfilterwidget.h \
	Utilities/xmlutil.h \
	Database/dbmanager.h

contains(DEFINES, USE_DATABASE) {
	SOURCES += Database/mysqlhandler.cpp
	HEADERS += Database/mysqlhandler.h

	message("Setting use database ...")
}

RESOURCES += \
	resource/resource.qrc
