include(WDEConfig.pri)

##################################################
QT	+= core gui webkit

DESTDIR = $$buildDir
LIBS += -L$$buildDir -l$$coreLib

INCLUDEPATH += $$quote(../WDELib/)
##################################################

##################################################
# Setting use database
QT += sql # for SQLite (dbManager)
##################################################
