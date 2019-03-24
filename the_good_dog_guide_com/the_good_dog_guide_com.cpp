#include "the_good_dog_guide_com.h"

/*
 * CSS reference
 * 		http://www.w3schools.com/cssref/css_selectors.asp
 */

#define BASEURL "https://www.thegooddogguide.com/dog-grooming-groomers"

//""

the_good_dog_guide_com::the_good_dog_guide_com(QWidget *apParent)
	: BaseWebEngine(apParent) {
// for using custom control, call this function then implement initExtWidget()
	initCustomControl();

	setWindowTitle(QLatin1String("the_good_dog_guide_com"));
	setBaseURL(BASEURL);

	if(mpDBLogger != NULL)
		mpDBLogger->setLogPath(QLatin1String("the_good_dog_guide_com.sql"));

	mpXMLLogger = new XMLLogger(mpLog, QLatin1String("the_good_dog_guide_com.xml"));

	mpLog->setFilePath(QLatin1String("the_good_dog_guide_com.txt"));
///////////////////////////////////////////////////////////
	QString homeDir(::getenv("HOME"));
	mpDBManager = new DBManager(homeDir + "/tmp/the_good_dog_guide_com.db");
	qDebug() << QLatin1String("Open local DB:") << mpDBManager->open()
			 << QLatin1String("--> path: ") << mpDBManager->getDBPath();
	qDebug() << QLatin1String("Create temp table:") << mpDBManager->createTmpTable();
}

enum enuDatabase {
	eName = 0,
	eAddress, eEmail, eTelephone, eWebsite,
	eServices,
	eDatabaseMax
};

void the_good_dog_guide_com::initDatabase() {
	mDbLstColName << QLatin1String("Name")
				  << QLatin1String("Address")
				  << QLatin1String("Email")
				  << QLatin1String("Telephone")
				  << QLatin1String("Website")
				  << QLatin1String("Services")
					 ;
	mDbLstKey << eName << eAddress;
	mDbTableName = QLatin1String("the_good_dog_guide_com");

	mpDBManager->createDataTable(mDbTableName,
								 mDbLstColName,
								 mDbLstKey);
}

void the_good_dog_guide_com::doStart() {
	do {
		if (mpExtFileLoading->getExtractLink()) {
			meState = E_STATE_INIT;
			loadPage(BASEURL);
			break;
		}

		// process from database
		mlstDataLink = mpDBManager->getAllFromTmpTable();
		int ignore = mpExtFileLoading->getNumberOfSkipItem();
		while (ignore > 0) {
			mlstDataLink.takeFirst();
			ignore --;
		}

		if (!gotoNextDataLink())
			showFinishMsg(QLatin1String("ERROR: Data list is empty!"));
	} while(false);
}

void the_good_dog_guide_com::doWebLoadFinished() {
	switch(meState)
	{
	case E_STATE_INIT:
		getCountyLinks();
		break;

	case E_STATE_GET_SUB_CATEGORY:
		getGroomerLinks();
		break;

	case E_STATE_GET_DETAIL_INFO:
		getGroomerDetail();
		break;

	case E_STATE_SUB1:
		getGroomerDetailWebLink();
		break;

	default:
		qDebug() << "default";
		break;
	}
}

////////////////////////////////////////////////////////////////////////

void the_good_dog_guide_com::getCountyLinks() {
	QWebElementCollection eOptions = mpWebView->getElementById(QLatin1String("map-form"))
									 .findAll(QLatin1String("div.locator-select select option"));
	QString url;
	QString optValue;
	foreach(QWebElement element, eOptions) {
		optValue = element.attribute(QLatin1String("value"));
		if (optValue.isEmpty()) continue;

		url = QString("https://www.thegooddogguide.com%1")
			  .arg(optValue);

		qDebug() << QLatin1String("URL: ") << url;

		mlstCategoryLink << url;
	}

	if (!gotoNextCategoryLink()) {
		showFinishMsg(QLatin1String("ERROR: Category link list is empty!"));
	}
}

void the_good_dog_guide_com::getGroomerLinks() {
	QWebElementCollection eLinks = mpWebView->getElementById(QLatin1String("main-content"))
								   .findAll(QLatin1String("div.content-box div.listing-footer a.nonlinky"));
	QString url;
	int count = 0;
	foreach(QWebElement element, eLinks) {
		url = mpWebView->getHrefURL(&element);
		mpDBManager->insertTmpTable(url);
		count ++;
	}

	qDebug() << QLatin1String("Groomer count: ") << count;

	if (!gotoNextCategoryLink()) {
		showFinishMsg(QLatin1String("Finished extracting all Groomer links"));
	}
}

void the_good_dog_guide_com::getGroomerDetail() {
	QWebElement eMainContent = mpWebView->getElementById(QLatin1String("main-content"))
							   .findFirst(QLatin1String("div.content-box"));
	updateData(&mData, eName, QLatin1String("div.content-title-box h1"), &eMainContent);

	QString strTmp = eMainContent.findFirst(QLatin1String("div#listing-address")).toPlainText();
	mData.updatePropertyString(eAddress, strTmp, true);

	QWebElement eDetails = eMainContent.findFirst(QLatin1String("div.listing-header div.column div.details"));
	Utilities::extractAndUpdate(&mData, mDbLstColName[eTelephone], eDetails.toPlainText(), QLatin1String("Tel:"));

	QString webURL;
	QWebElementCollection eLinks = eDetails.findAll(QLatin1String("a"));
	foreach(QWebElement ele, eLinks) {
		if (ele.toPlainText() != QLatin1String("Visit website")) continue;

		webURL = mpWebView->getHrefURL(&ele);
		mData.updatePropertyString(eWebsite, webURL);
	}

	strTmp = eMainContent.findFirst(QLatin1String("div.listing-header div.bullets")).toPlainText();
	QStringList tmpList = strTmp.split(QLatin1String("\n"));
	mData.updateProperty(eServices, tmpList);

	do {
		// try to load the website page
		if (!webURL.isEmpty()) {
			meState = E_STATE_SUB1;
			loadPage(webURL);
			break;
		}

		mpDBManager->insertData(&mData, mDbTableName);
		mData.clearData();

		// next data
		if (!gotoNextDataLink())
			showFinishMsg(QLatin1String("Finish extracting all data!"));
	} while (false);
}

void the_good_dog_guide_com::getGroomerDetailWebLink() {
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

WebEngine_Main(the_good_dog_guide_com)
