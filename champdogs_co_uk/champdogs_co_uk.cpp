#include "champdogs_co_uk.h"
#include <QWebFrame>

/*
 * CSS reference
 * 		http://www.w3schools.com/cssref/css_selectors.asp
 */

#define BASEURL "https://www.champdogs.co.uk/breeder"

champdogs_co_uk::champdogs_co_uk(QWidget *apParent)
	: BaseWebEngine(apParent) {
	// for using custom control, call this function then implement initExtWidget()
	initCustomControl();

	setWindowTitle(QLatin1String("champdogs_co_uk"));
	setBaseURL(BASEURL);

	if(mpDBLogger != NULL)
		mpDBLogger->setLogPath(QLatin1String("champdogs_co_uk.sql"));

	mpXMLLogger = new XMLLogger(mpLog, QLatin1String("champdogs_co_uk.xml"));

	mpLog->setFilePath(QLatin1String("champdogs_co_uk.txt"));
	///////////////////////////////////////////////////////////
	QString homeDir(::getenv("HOME"));
	mpDBManager = new DBManager(homeDir + "/tmp/champdogs_co_uk.db");
	qDebug() << QLatin1String("Open local DB:") << mpDBManager->open()
			 << QLatin1String("--> path: ") << mpDBManager->getDBPath();
	qDebug() << QLatin1String("Create temp table:") << mpDBManager->createTmpTable();
}

enum enuDatabase {
	eName = 0,
	eAddress, eEmail, eTelephone, eWebsite,
	eDogBreed,
	eDatabaseMax
};

void champdogs_co_uk::initDatabase() {
	mDbLstColName << QLatin1String("Name")
				  << QLatin1String("Address")
				  << QLatin1String("Email")
				  << QLatin1String("Telephone")
				  << QLatin1String("Website")
				  << QLatin1String("Dog-Breeds")
					 ;
	mDbLstKey << eName << eEmail;
	mDbTableName = QLatin1String("champdogs_co_uk2");

	mpDBManager->createDataTable(mDbTableName,
								 mDbLstColName,
								 mDbLstKey);
}

void champdogs_co_uk::doStart() {
	do {
		if (mpExtFileLoading->getExtractLink()) {
			meState = E_STATE_INIT;
			loadPage(BASEURL);
			break;
		}

		mlstDataLink = mpDBManager->getAllFromTmpTable();
		int ignore = mpExtFileLoading->getNumberOfSkipItem();
		while (ignore > 0) {
			mlstDataLink.takeFirst();
			ignore --;
		}

		if (!gotoNextDataLink())
			showFinishMsg(QLatin1String("Something is wrong - Data list is empty!"));
	} while (false);
}

void champdogs_co_uk::doWebLoadFinished() {
	switch(meState)
	{
	case E_STATE_INIT:
		getBreederCategories();
		break;

	case E_STATE_GET_SUB_CATEGORY:
		getBreederList();
		break;

	case E_STATE_GET_DETAIL_INFO:
		getBreederDetail();
		break;

	case E_STATE_SUB1: {
		getBreederWebsite();
		break;
	}

	default:
		qDebug() << "default";
		break;
	}
}

////////////////////////////////////////////////////////////////////////
void champdogs_co_uk::getBreederCategories() {
	QWebElement eBreedPics = mpWebView->getElementById(QLatin1String("breed_pics"));
	QWebElementCollection eLinks = eBreedPics.findAll(QLatin1String("a"));
	QWebElement eLink;

	foreach(eLink, eLinks)
		mBreederCategories << QString("%1?start=all").arg(mpWebView->getHrefURL(&eLink));

	if (!gotoNextLink(&mBreederCategories, E_STATE_GET_SUB_CATEGORY))
		showFinishMsg(QLatin1String("Something goes wrong - can not go to next breeder category"));
}

void champdogs_co_uk::getBreederList() {
	QWebElementCollection eLinks = mpWebView->findAll(QLatin1String("a[itemprop=itemListElement]"));
	QWebElement eLink;
	QString txtLink;

	foreach(eLink, eLinks) {
		txtLink = mpWebView->getHrefURL(&eLink);
		mpDBManager->insertTmpTable(txtLink);
	}

	if (!gotoNextLink(&mBreederCategories, E_STATE_GET_SUB_CATEGORY)) {
		qDebug() << QLatin1String("Finish extracting all breeders list");

		// load breeders list from DB
		mlstDataLink = mpDBManager->getAllFromTmpTable();
		if (!gotoNextDataLink())
			showFinishMsg(QLatin1String("Something goes wrong - Data list is empty!"));
	}
}

void champdogs_co_uk::getBreederDetail() {
	updateData(&mData, eName, QLatin1String("div.breeder h1.kennel"));
	QString text = mpWebView->findFirst(QLatin1String("div.breeder p.header")).toPlainText();
	QStringList textData;
	QStringList tmpList = text.split(QLatin1String("\n"));
	tmpList.removeFirst(); // first is name, ignore

	foreach (text, tmpList) {
		text = text.trimmed();
		if (text.isEmpty()) continue;

		textData << text;
	}

	QWebElementCollection eLinks = mpWebView->findAll(QLatin1String("div.breeder p.header a"));
	QWebElement eLink;

	QString wsURL;
	QString wsText;
	foreach(eLink, eLinks) {
		if (eLink.toPlainText().contains(QLatin1Char('@'))) {
			mData.updatePropertyString(eEmail, eLink.toPlainText());
			continue;
		}

		wsURL = mpWebView->getHrefURL(&eLink);
		if (eLink.toPlainText().trimmed() == QLatin1String("Visit our website")
				|| wsURL.contains(QLatin1String("/offsite.cgi?id="))) {
			mData.updatePropertyString(eWebsite, wsURL);
			wsText = eLink.toPlainText();
			continue;
		}
	}

	if (!mData.getPropertyString(eWebsite).isEmpty()) {
		textData.removeAll(wsText);
		textData.removeAll(QLatin1String("Visit our website"));
	}
	if (!mData.getPropertyString(eEmail).isEmpty())
		textData.removeAll(mData.getPropertyString(eEmail));

	int size = textData.count();
	int i = size - 1;

	// search for telephone
	QStringList telephone;
	while ((i > 0)
		   &&
		   (QLatin1Char('+') == textData.at(i).at(0)
			|| (QLatin1Char('0') <= textData.at(i).at(0)
				&& textData.at(i).at(0) <= QLatin1Char('9')))) {
		telephone.insert(0, textData.at(i));
		i --;
	}
	mData.updateProperty(eTelephone, telephone, true, false);
	//-----------------------------------------------

	// get address
	foreach (text, telephone)
		textData.removeAll(text);
	textData.removeAll(QLatin1String("View Map"));
	mData.updateProperty(eAddress, textData, true, false);
	//-----------------------------------------------

	// get dog-breeds
	QWebElement eElement = mpWebView->findFirst(QLatin1String("div.breeder ul.bullet"));
	QWebElementCollection eLis = eElement.findAll(QLatin1String("li"));
	QStringList breeds;
	foreach (eElement, eLis)
		breeds << eElement.toPlainText().trimmed();
	mData.updateProperty(eDogBreed, breeds);
	//-----------------------------------------------

	do {
		// try to load the website page
		if (!mData.getPropertyString(eWebsite).isEmpty()) {
			meState = E_STATE_SUB1;
			loadPage(mData.getPropertyString(eWebsite));
			break;
		}

		mpDBManager->insertData(&mData, mDbTableName);
		mData.clearData();

		// next data
		if (!gotoNextDataLink())
			showFinishMsg(QLatin1String("Finish extracting all data!"));
	} while (false);
}

void champdogs_co_uk::getBreederWebsite() {
	do {
		bool continueWaiting = true;
		QString url = mpWebView->url().toString();

		// check for redirected page
		do {
			if (url != mData.getPropertyString(eWebsite)) {
				continueWaiting = false;

				if (url == QLatin1String("https://outlook.office.com/owa/")
						|| url == QLatin1String("https://www.sky.com/")
						|| url == QLatin1String("https://www.cjb.net/")
						|| url == QLatin1String("https://www.aol.com/")
						|| url == QLatin1String("https://sg.yahoo.com/?p=us")
						|| url.contains(QLatin1String("https://accounts.google.com/ServiceLogin?service=mail&passive=true"))
						|| url == QLatin1String("http://x3.hu/nemletezik")
						|| url == QLatin1String("http://www.virginmedia.com/")) {
					url.clear();
				}

				break;
			}

			// check for error page
			if (!mblLastLoadStatus) {
				url.clear();
				continueWaiting = false;
				break;
			}
		} while (false);
		if (continueWaiting) break;

		mData.updatePropertyString(eWebsite, url);
		mpDBManager->insertData(&mData, mDbTableName);
		mData.clearData();

		// next data
		if (!gotoNextDataLink())
			showFinishMsg(QLatin1String("Finish extracting all data!"));
	} while (false);
}
////////////////////////////////////////////////////////////////////////

WebEngine_Main(champdogs_co_uk)
