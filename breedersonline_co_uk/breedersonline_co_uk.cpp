#include "breedersonline_co_uk.h"

#include <QNetworkProxy>

/*
 * CSS reference
 * 		http://www.w3schools.com/cssref/css_selectors.asp
 */ 

#define BASEURL "https://www.breedersonline.co.uk/pedigree-breeder.php?adtype=DB&breedid=5"

breedersonline_co_uk::breedersonline_co_uk(QWidget *apParent)
	: BaseWebEngine(apParent) {
// for using custom control, call this function then implement initExtWidget()
	initCustomControl();

	setWindowTitle(QLatin1String("breedersonline_co_uk"));
	setBaseURL(BASEURL);

	if(mpDBLogger != NULL)
		mpDBLogger->setLogPath(QLatin1String("breedersonline_co_uk.sql"));

	mpXMLLogger = new XMLLogger(mpLog, QLatin1String("breedersonline_co_uk.xml"));

	mpLog->setFilePath(QLatin1String("breedersonline_co_uk.txt"));
///////////////////////////////////////////////////////////
#if 1
	QNetworkProxy proxy;
	proxy.setType(QNetworkProxy::Socks5Proxy);
	proxy.setType(QNetworkProxy::HttpProxy);
	proxy.setHostName(QLatin1String("sin2.sme.zscalertwo.net"));
	proxy.setPort(80);
	QNetworkProxy::setApplicationProxy(proxy);
#endif
}

enum enuDatabase {
	eName = 0,
	eDatabaseMax
};

void breedersonline_co_uk::initDatabase() {
	mDbLstColName << QLatin1String("Name");
	mDbLstKey << eName;
	mDbTableName = QLatin1String("breedersonline_co_uk");
}

void breedersonline_co_uk::doStart() {
	meState = E_STATE_INIT;
	loadPage(BASEURL);
}

void breedersonline_co_uk::doWebLoadFinished() {
	switch(meState)
	{
	case E_STATE_INIT:
		qDebug() << "Calling loadList ...";
		loadList();
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
void breedersonline_co_uk::loadList() {
	mpWebView->doScroll(0, 1000);
}
////////////////////////////////////////////////////////////////////////

WebEngine_Main(breedersonline_co_uk)
