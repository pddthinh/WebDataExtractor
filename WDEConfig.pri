CONFIG(debug, release|debug): buildDir=../wde-build-debug
CONFIG(release, release|debug): buildDir=../wde-build

## using MySQL database
#DEFINES += USE_DATABASE

coreLib = WDELib
contains(DEFINES, USE_DATABASE) {
	coreLib = WDELibMySQL
}

# For Qt5
greaterThan(QT_MAJOR_VERSION, 4): {
    QT          += widgets
    QT          += webkitwidgets
    DEFINES     += WDE_QT5
}

resourceLib = WDEResource
