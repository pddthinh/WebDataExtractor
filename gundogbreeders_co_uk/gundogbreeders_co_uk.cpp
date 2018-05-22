#include "gundogbreeders_co_uk.h"

/*
 * CSS reference
 * 		http://www.w3schools.com/cssref/css_selectors.asp
 */

#define BASEURL "http://www.gundogbreeders.co.uk/findabreeder/"

#define TBL_BREEDER		"breeder"

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
#if 0
	qDebug() << QLatin1String("Open local DB:") << mpDBManager->open()
			 << QLatin1String("--> path: ") << mpDBManager->getDBPath();
	qDebug() << QLatin1String("Create temp table:") << mpDBManager->createTmpTable();
#else
	mpDBManager->open();
#endif
}

enum enuDatabase {
	eName = 0,
	eEmail, eWebsite, eTwitter, eFacebook,
	ePhone, eCounty,
	eBreeds,
	eDatabaseMax
};

void gundogbreeders_co_uk::initDatabase() {
	mDbLstColName << QLatin1String("Name")
				  << QLatin1String("Email") << QLatin1String("Website") << QLatin1String("Twitter") << QLatin1String("Facebook")
				  << QLatin1String("Phone") << QLatin1String("County")
				  << QLatin1String("Breeds")
				;
	mDbLstKey << eName << ePhone << eWebsite << eCounty;
	mDbTableName = QLatin1String("gundogbreeders_co_uk");
}

void gundogbreeders_co_uk::doStart() {
	do {
		if (mpExtFileLoading->getExtractLink()) {
			meState = E_STATE_INIT;
			loadPage(BASEURL);
			break;
		}

		mpDBManager->createDataTable(mDbTableName, mDbLstColName, mDbLstKey);

		mlstDataLink = mpDBManager->getAllFromTmpTable(QLatin1String(TBL_BREEDER));
		if (!gotoNextDataLink())
			showFinishMsg(QLatin1String("Something is wrong - Data list is empty"));
	} while (false);
}

void gundogbreeders_co_uk::doWebLoadFinished() {
	switch(meState)
	{
	case E_STATE_INIT:
		getAllCountry();
		break;

	case E_STATE_SUB1:
		getAllCounty();
		break;

	case E_STATE_SUB2:
		getAllBreeder();
		break;

	case E_STATE_GET_DETAIL_INFO:
		getDetailBreeder();
		break;

	default:
		break;
	}
}

////////////////////////////////////////////////////////////////////////

void gundogbreeders_co_uk::getAllCountry() {
	QWebElementCollection elements;
	QWebElement link;

	mpDBManager->createTmpTable(QLatin1String(TBL_BREEDER));

	elements = mpWebView->findAll(QLatin1String("div.feature_panel_inner a"));
	foreach(link, elements)
		listCountry << mpWebView->getHrefURL(&link);

	if (!gotoNextLink(&listCountry, E_STATE_SUB1))
		showFinishMsg(QLatin1String("Something is wrong - Country list is empty"));
}

void gundogbreeders_co_uk::getAllCounty() {
	QWebElementCollection links;
	QWebElement link;

	links = mpWebView->findAll(QLatin1String("div.colDropInner a"));
	foreach(link, links)
		listCounty << mpWebView->getHrefURL(&link);

	if (!gotoNextLink(&listCounty, E_STATE_SUB2)) {
		qDebug() << QLatin1String("Done for one country, trying to continue next ...");

		if (!gotoNextLink(&listCountry, E_STATE_SUB1))
			showFinishMsg(QLatin1String("Done extracting breeders"));
	}
}

void gundogbreeders_co_uk::getAllBreeder() {
	QWebElementCollection links;
	QWebElement link;
	QString linkTxt;

	links = mpWebView->findAll(QLatin1String("a.btn"));
	foreach(link, links) {
		linkTxt = mpWebView->getHrefURL(&link);
		mpDBManager->insertTmpTable(linkTxt, QLatin1String(TBL_BREEDER));
	}

	if (!gotoNextLink(&listCounty, E_STATE_SUB2)) {
		qDebug() << QLatin1String("Done for one country, trying to continue next ...");

		if (!gotoNextLink(&listCountry, E_STATE_SUB1))
			showFinishMsg(QLatin1String("Done extracting breeders"));
	}
}

void gundogbreeders_co_uk::getDetailBreeder() {
	QWebElement element;
	DataEntry data(mDbLstColName);
	QString text;

	updateData(&data, eName, QLatin1String("h1"));
	text = mpWebView->findFirst(QLatin1String("div.colDropInner")).toPlainText();

	Utilities::extractAndUpdate(&data, mDbLstColName[eEmail], text, QLatin1String("Email: "));
	Utilities::extractAndUpdate(&data, mDbLstColName[eWebsite], text, QLatin1String("Website: "));
	Utilities::extractAndUpdate(&data, mDbLstColName[eTwitter], text, QLatin1String("Twitter: "));
	Utilities::extractAndUpdate(&data, mDbLstColName[eFacebook], text, QLatin1String("Facebook: "));
	Utilities::extractAndUpdate(&data, mDbLstColName[ePhone], text, QLatin1String("Phone: "));
	Utilities::extractAndUpdate(&data, mDbLstColName[eCounty], text, QLatin1String("County: "));
	Utilities::extractAndUpdate(&data, mDbLstColName[eBreeds], text, QLatin1String("We breed: "));

	mpDBManager->insertData(&data, mDbTableName);

	if (!gotoNextDataLink())
		showFinishMsg(QLatin1String("Done extracting all breeders info"));
}
////////////////////////////////////////////////////////////////////////

WebEngine_Main(gundogbreeders_co_uk)
