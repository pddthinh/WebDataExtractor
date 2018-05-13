#include "dogbreeds_co_uk.h"

/*
 * CSS reference
 * 		http://www.w3schools.com/cssref/css_selectors.asp
 */

#define BASEURL "http://www.dog-breeds.co.uk/search/breeders?breed_id=&county_id=&submit=Search+Breeders"

dogbreeds_co_uk::dogbreeds_co_uk(QWidget *apParent)
	: BaseWebEngine(apParent) {
// for using custom control, call this function then implement initExtWidget()
	initCustomControl();

	setWindowTitle(QLatin1String("dogbreeds_co_uk"));
	setBaseURL(BASEURL);

	if(mpDBLogger != NULL)
		mpDBLogger->setLogPath(QLatin1String("dogbreeds_co_uk.sql"));

	mpXMLLogger = new XMLLogger(mpLog, QLatin1String("dogbreeds_co_uk.xml"));

	mpLog->setFilePath(QLatin1String("dogbreeds_co_uk.txt"));
///////////////////////////////////////////////////////////
	QString homeDir(::getenv("HOME"));
	mpDBManager = new DBManager(homeDir + "/tmp/dogbreeds_co_uk.db");
#if 1
	qDebug() << QLatin1String("Open local DB:") << mpDBManager->open()
			 << QLatin1String("--> path: ") << mpDBManager->getDBPath();
	qDebug() << QLatin1String("Create temp table:") << mpDBManager->createTmpTable();
#endif
}

enum enuDatabase {
	eName = 0,
	eAddress, ePhone,
	eBreeds,
	eDatabaseMax
};

void dogbreeds_co_uk::initDatabase() {
	mDbLstColName << QLatin1String("Name")
				  << QLatin1String("Address")
				  << QLatin1String("Phone")
				  << QLatin1String("Breeds")
					 ;
	mDbLstKey << eName << eAddress << ePhone;
	mDbTableName = QLatin1String("dogbreeds_co_uk");
}

void dogbreeds_co_uk::doStart() {
	do {
		if (mpExtFileLoading->getExtractLink()) {
			meState = E_STATE_INIT;
			loadPage(BASEURL);
			break;
		}

		mpDBManager->createDataTable(mDbTableName, mDbLstColName, mDbLstKey);

		mlstDataLink = mpDBManager->getAllFromTmpTable();
		meState = E_STATE_GET_DETAIL_INFO;
		if (!gotoNextDataLink())
			showFinishMsg(QLatin1String("Unknown Error - Data link list is empty!"));
	} while(0);
}

void dogbreeds_co_uk::doWebLoadFinished() {
	switch(meState)
	{
	case E_STATE_INIT:
		getBreedItems();
		break;

	case E_STATE_GET_DETAIL_INFO:
		getBreederDetail();
		break;

	default:
		break;
	}
}

////////////////////////////////////////////////////////////////////////

void dogbreeds_co_uk::getBreedItems() {
	QString link;
	QWebElement element;
	QWebElementCollection eLinks;

	eLinks = mpWebView->findAll(QLatin1String("table.search.breeder a"));
	foreach(element, eLinks) {
		link = mpWebView->getHrefURL(&element);
		mpDBManager->insertTmpTable(link);
	}

	element = mpWebView->findFirst(QLatin1String("ul.pagination li.next a"));
	if (element.isNull())
		showFinishMsg(QLatin1String("Finished extracting all breeder info!"));
	else
		mpWebView->doClick(&element);
}

void dogbreeds_co_uk::getBreederDetail() {
	DataEntry data(mDbLstColName);
	QString tmp;
	QWebElement element;

	element = mpWebView->findFirst(QLatin1String("dl.profile"));
	tmp = element.toPlainText() + "\n";

	Utilities::extractAndUpdate(&data, mDbLstColName[eName], tmp, QLatin1String("Name:\n"));
	Utilities::extractAndUpdate(&data, mDbLstColName[eAddress], tmp, QLatin1String("Address:\n"),
								QLatin1String("\nDaytime"), "\n", true);
	Utilities::extractAndUpdate(&data, mDbLstColName[ePhone], tmp, QLatin1String("Phone Number:\n"));

	element = mpWebView->findFirst(QLatin1String("div.breeds ul.breed-list"));
	QStringList breeds = element.toPlainText().split(QLatin1String("\n"));
	data.updateProperty(eBreeds, breeds, true);

	mpDBManager->insertData(&data, mDbTableName);
	if (!gotoNextDataLink())
		showFinishMsg(QLatin1String("Finished extracting all breeders info"));
}

////////////////////////////////////////////////////////////////////////

WebEngine_Main(dogbreeds_co_uk)
