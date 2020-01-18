#include "liverylist_co_uk.h"

/*
 * CSS reference
 * 		http://www.w3schools.com/cssref/css_selectors.asp
 */

#define BASEURL "https://www.liverylist.co.uk/yards/page/1/"

liverylist_co_uk::liverylist_co_uk(QWidget *apParent)
	: BaseWebEngine(apParent) {
	// for using custom control, call this function then implement initExtWidget()
	initCustomControl();

	setWindowTitle(QLatin1String("liverylist_co_uk"));
	setBaseURL(BASEURL);

#if 0
	if(mpDBLogger != NULL)
		mpDBLogger->setLogPath(QLatin1String("liverylist_co_uk.sql"));

	mpXMLLogger = new XMLLogger(mpLog, QLatin1String("liverylist_co_uk.xml"));
#endif

	mpLog->setFilePath(QLatin1String("liverylist_co_uk.txt"));
	///////////////////////////////////////////////////////////
	QString homeDir(::getenv("HOME"));
	mpDBManager = new DBManager(homeDir + "/tmp/liverylist_co_uk.db");

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
	eAddress,
	eContact, eMobile,
	eWeb,
	eFacebook,
	eDatabaseMax
};

void liverylist_co_uk::initDatabase() {
	mDbLstColName << QLatin1String("Name")
				  << QLatin1String("Address")
				  << QLatin1String("Contact") << QLatin1String("Mobile")
				  << QLatin1String("Website")
				  << QLatin1String("Facebook")
					 ;
	mDbLstKey << eName << eAddress << eContact << eMobile;
	mDbTableName = QLatin1String("liverylist_co_uk");

	mpDBManager->createDataTable(mDbTableName,
								 mDbLstColName,
								 mDbLstKey);
}

void liverylist_co_uk::doStart() {
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

void liverylist_co_uk::doWebLoadFinished() {
	switch(meState)
	{
	case E_STATE_INIT:
		parseLiveryProfile();
		break;

	case E_STATE_GET_DETAIL_INFO:
		parseProfileDetail();
		break;

	default:
		qDebug() << "default";
		break;
	}
}

////////////////////////////////////////////////////////////////////////

void liverylist_co_uk::parseLiveryProfile() {
	QWebElementCollection collection = mpWebView->findAll(QLatin1String("h2.geodir-entry-title a"));
	QWebElement link;
	foreach (link, collection)
		mpDBManager->insertTmpTable(mpWebView->getHrefURL(&link));

	QWebElement nextLink;
	collection = mpWebView->findAll(QLatin1String("div.Navi.gd-navi strong"));
	foreach (link, collection) {
		if (link.toPlainText() == QLatin1String(">")) {
			nextLink = link.parent();
			break;
		}
	}

	if (!nextLink.isNull()) {
		mpWebView->doClick(&nextLink);
	} else {
		showFinishMsg(QLatin1String("Finished extracting all profile links"));
	}
}

void liverylist_co_uk::parseProfileDetail() {
	QWebElement element = mpWebView->findFirst(QLatin1String("ul#breadcrumbs li"));
	QStringList lstName = element.toPlainText().split(QLatin1String(" > "));
	mData.updatePropertyString(eName, lstName.takeLast());

	QString text;

	element = mpWebView->findFirst(QLatin1String("div.geodir_more_info.post_address"));
	QStringList lstAddress = element.toPlainText()
							 .replace(QLatin1String("Yard Address:"), QLatin1String(""))
							 .split("\n");
	mData.updateProperty(eAddress, lstAddress);

	updateData(&mData, eContact, QLatin1String("div.geodir_more_info.geodir_contact a"));

	element = mpWebView->findFirst(QLatin1String("div.geodir_more_info.geodir_MobileNumber"));
	if (!element.isNull()) {
		text = element.toPlainText().replace(QLatin1String("Mobile Number:"), QLatin1String(""));
		mData.updatePropertyString(eMobile, text);
	}

	element = mpWebView->findFirst(QLatin1String("div.geodir_more_info.geodir_website span.geodir-i-website a"));
	if (!element.isNull()){
		text = mpWebView->getHrefURL(&element);
		mData.updatePropertyString(eWeb, text);
	}

	element = mpWebView->findFirst(QLatin1String("div.geodir_more_info.geodir_facebook span.geodir-i-website a"));
	if (!element.isNull()){
		text = mpWebView->getHrefURL(&element);
		mData.updatePropertyString(eFacebook, text);
	}

	mpDBManager->insertData(&mData, mDbTableName);

	if (!gotoNextDataLink())
		showFinishMsg(QLatin1String("Finished extracting all profile!"));
}

////////////////////////////////////////////////////////////////////////

WebEngine_Main(liverylist_co_uk)
