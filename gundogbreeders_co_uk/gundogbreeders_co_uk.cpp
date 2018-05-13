#include "gundogbreeders_co_uk.h"

/*
 * CSS reference
 * 		http://www.w3schools.com/cssref/css_selectors.asp
 */

#define BASEURL ""

gundogbreeders_co_uk::gundogbreeders_co_uk(QWidget *apParent)
	: BaseWebEngine(apParent) {
// for using custom control, call this function then implement initExtWidget()
	initCustomControl();

	setWindowTitle(QLatin1String("gundogbreeders_co_uk"));
	setBaseURL(BASEURL);

	if(mpDBLogger != NULL)
		mpDBLogger->setLogPath(QLatin1String("gundogbreeders_co_uk.sql"));

	mpXMLLogger = new XMLLogger(mpLog, QLatin1String("gundogbreeders_co_uk.xml"));

	mpLog->setFilePath(QLatin1String("gundogbreeders_co_uk.txt"));
///////////////////////////////////////////////////////////

	QString homeDir(::getenv("HOME"));
	mpDBManager = new DBManager(homeDir + "/tmp/gundogbreeders_co_uk.db");
#if 1
	qDebug() << QLatin1String("Open local DB:") << mpDBManager->open()
			 << QLatin1String("--> path: ") << mpDBManager->getDBPath();
	qDebug() << QLatin1String("Create temp table:") << mpDBManager->createTmpTable();
#endif
}

enum enuDatabase {
	eName = 0,
	eDatabaseMax
};

void gundogbreeders_co_uk::initDatabase() {
	mDbLstColName << QLatin1String("Name");
	mDbLstKey << eName;
	mDbTableName = QLatin1String("gundogbreeders_co_uk");
}

void gundogbreeders_co_uk::doStart() {
	meState = E_STATE_INIT;
	loadPage(BASEURL);
}

void gundogbreeders_co_uk::doWebLoadFinished() {
	switch(meState)
	{
	case E_STATE_INIT:
		qDebug() << "E_STATE_INIT";
		break;

	case E_STATE_GET_MAIN_CATEGORY:
		qDebug() << "E_STATE_GET_MAIN_CATEGORY";
		break;

	case E_STATE_GET_SUB_CATEGORY:
		qDebug() << "E_STATE_GET_SUB_CATEGORY";
		break;

	case E_STATE_GET_DETAIL_INFO:
		qDebug() << "E_STATE_GET_DETAIL_INFO";
		break;

	default:
		qDebug() << "default";
		break;
	}
}

////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////

WebEngine_Main(gundogbreeders_co_uk)
