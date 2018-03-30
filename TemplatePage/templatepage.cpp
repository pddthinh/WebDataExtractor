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

	if(mpDBLogger != NULL)
		mpDBLogger->setLogPath(QLatin1String("templatepage.sql"));

	mpXMLLogger = new XMLLogger(mpLog, QLatin1String("templatepage.xml"));

	mpLog->setFilePath(QLatin1String("templatepage.txt"));
///////////////////////////////////////////////////////////
}

enum enuDatabase {
	eName = 0,
	eDatabaseMax
};

void templatepage::initDatabase() {
	mDbLstColName << QLatin1String("Name");
	mDbLstKey << eName;
	mDbTableName = QLatin1String("templatepage");
}

void templatepage::doStart() {
	meState = E_STATE_INIT;
	loadPage(BASEURL);
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
