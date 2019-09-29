#include "templatepage.h"

/*
 * CSS reference
 * 		http://www.w3schools.com/cssref/css_selectors.asp
 */ 

#define BASEURL ""

templatepage::templatepage(QWidget *apParent)
	: BaseWebEngine(apParent) {
// for using custom control, call this function then implement initExtWidget()
	initCustomControl();

	setWindowTitle(QLatin1String("templatepage"));
	setBaseURL(BASEURL);

#if 0
	if(mpDBLogger != NULL)
		mpDBLogger->setLogPath(QLatin1String("templatepage.sql"));

	mpXMLLogger = new XMLLogger(mpLog, QLatin1String("templatepage.xml"));
#endif

	mpLog->setFilePath(QLatin1String("templatepage.txt"));
///////////////////////////////////////////////////////////
	QString homeDir(::getenv("HOME"));
	mpDBManager = new DBManager(homeDir + "/tmp/templatepage.db");

#ifdef DEBUG
	qDebug() << QLatin1String("Open local DB:") << mpDBManager->open()
			 << QLatin1String("--> path: ") << mpDBManager->getDBPath();
	qDebug() << QLatin1String("Create temp table:") << mpDBManager->createTmpTable();
#else
	mpDBManager->open();
	mpDBManager->createTmpTable();
#endif
}

enum enuDatabase {
	eName = 0,
	eDatabaseMax
};

void templatepage::initDatabase() {
	mDbLstColName << QLatin1String("Name");
	mDbLstKey << eName;
	mDbTableName = QLatin1String("templatepage");

	mpDBManager->createDataTable(mDbTableName,
								 mDbLstColName,
								 mDbLstKey);
}

void templatepage::doStart() {
	do {
		if (mpExtFileLoading->getExtractLink()) {
			meState = E_STATE_INIT;
			loadPage(BASEURL);
			break;
		}

		mlstDataLink = mpDBManager->getAllFromTmpTable();
		qDebug() << QLatin1String("Filtering skipped item result: ")
				 << mpExtFileLoading->filterSkippedItem(&mlstDataLink);

		if (!gotoNextDataLink())
			showFinishMsg(QLatin1String("Something is wrong - Data list is empty!"));
	} while (false);
}

void templatepage::doWebLoadFinished() {
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

WebEngine_Main(templatepage)
