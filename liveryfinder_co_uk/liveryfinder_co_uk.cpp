#include "liveryfinder_co_uk.h"

/*
 * CSS reference
 * 		http://www.w3schools.com/cssref/css_selectors.asp
 */

#define BASEURL ""

liveryfinder_co_uk::liveryfinder_co_uk(QWidget *apParent)
	: BaseWebEngine(apParent) {
	// for using custom control, call this function then implement initExtWidget()
	initCustomControl();

	setWindowTitle(QLatin1String("liveryfinder_co_uk"));
	setBaseURL(BASEURL);

#if 0
	if(mpDBLogger != NULL)
		mpDBLogger->setLogPath(QLatin1String("liveryfinder_co_uk.sql"));

	mpXMLLogger = new XMLLogger(mpLog, QLatin1String("liveryfinder_co_uk.xml"));
#endif

	mpLog->setFilePath(QLatin1String("liveryfinder_co_uk.txt"));
	///////////////////////////////////////////////////////////
	QString homeDir(::getenv("HOME"));
	mpDBManager = new DBManager(homeDir + "/tmp/liveryfinder_co_uk.db");

#ifdef DEBUG
	qDebug() << QLatin1String("Open local DB:") << mpDBManager->open()
			 << QLatin1String("--> path: ") << mpDBManager->getDBPath();
	qDebug() << QLatin1String("Create temp table:") << mpDBManager->createTmpTable();
#else
	// Release build
	mpDBManager->open();
	mpDBManager->createTmpTable();
#endif
}

enum enuDatabase {
	eName = 0,
	eAddress, eTel,
	eFeatures,
	eDatabaseMax
};

void liveryfinder_co_uk::initDatabase() {
	mDbLstColName << QLatin1String("Name")
				  << QLatin1String("Address")
				  << QLatin1String("Telephone")
				  << QLatin1String("Features")
					 ;
	mDbLstKey << eName << eTel;
	mDbTableName = QLatin1String("liveryfinder_co_uk");

	mpDBManager->createDataTable(mDbTableName,
								 mDbLstColName,
								 mDbLstKey);
}

void liveryfinder_co_uk::doStart() {
	do {
		if (mpExtFileLoading->getExtractLink()) {
			meState = E_STATE_INIT;

			// Hardcoded the category links
			mlstCategoryLink << QLatin1String("http://www.liveryfinder.co.uk/find-a-listing.htm?7"); // North Scotland
			mlstCategoryLink << QLatin1String("http://www.liveryfinder.co.uk/find-a-listing.htm?6"); // Central Scotland
			mlstCategoryLink << QLatin1String("http://www.liveryfinder.co.uk/find-a-listing.htm?8"); // South Scotland
			mlstCategoryLink << QLatin1String("http://www.liveryfinder.co.uk/find-a-listing.htm?14"); // Yorkshire
			mlstCategoryLink << QLatin1String("http://www.liveryfinder.co.uk/find-a-listing.htm?3"); // North East
			mlstCategoryLink << QLatin1String("http://www.liveryfinder.co.uk/find-a-listing.htm?1"); // East Midlands
			mlstCategoryLink << QLatin1String("http://www.liveryfinder.co.uk/find-a-listing.htm?2"); // Eastern
			mlstCategoryLink << QLatin1String("http://www.liveryfinder.co.uk/find-a-listing.htm?9"); // South East
			mlstCategoryLink << QLatin1String("http://www.liveryfinder.co.uk/find-a-listing.htm?4"); // North West
			mlstCategoryLink << QLatin1String("http://www.liveryfinder.co.uk/find-a-listing.htm?13"); // West Midlands
			mlstCategoryLink << QLatin1String("http://www.liveryfinder.co.uk/find-a-listing.htm?11"); // North Wales
			mlstCategoryLink << QLatin1String("http://www.liveryfinder.co.uk/find-a-listing.htm?12"); // South Wales
			mlstCategoryLink << QLatin1String("http://www.liveryfinder.co.uk/find-a-listing.htm?10"); // South West
			mlstCategoryLink << QLatin1String("http://www.liveryfinder.co.uk/find-a-listing.htm?5"); // Northen Ireland

			gotoNextCategoryLink(E_STATE_GET_MAIN_CATEGORY);
			break;
		}

		mlstDataLink = mpDBManager->getAllFromTmpTable();
		qDebug() << QLatin1String("Filtering skipped item result: ")
				 << mpExtFileLoading->filterSkippedItem(&mlstDataLink);

		if (!gotoNextDataLink())
			showFinishMsg(QLatin1String("Something is wrong - Data list is empty!"));
	} while (false);
}

void liveryfinder_co_uk::doWebLoadFinished() {
	switch(meState)
	{
	case E_STATE_GET_MAIN_CATEGORY:
		parseRegionProfile();
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

void liveryfinder_co_uk::parseRegionProfile() {
	do {
		QWebElementCollection linkList;

		linkList = mpWebView->findFirst(QLatin1String("div#Mid"))
				   .findAll(QLatin1String("div.Article div.Buttons div.Middle a.LinkWithImage.Magnify.SmallText.Green"));
		QWebElement link;

		foreach(link, linkList)
			mpDBManager->insertTmpTable(mpWebView->getHrefURL(&link));

		QWebElement pagerContainer = mpWebView->findFirst(QLatin1String("div#Paging span#ContentPlaceHolder1_ucListings_Region_dpPager"));
		link = pagerContainer.findFirst(QLatin1String("a.NextPrev.Next"));
		if (!link.isNull()) {
			mpWebView->doClick(&link); // go to next result page
			break;
		}

		// go to next category page
		if (!gotoNextCategoryLink(E_STATE_GET_MAIN_CATEGORY)) {
			showFinishMsg(QLatin1String("Finished extracting all profile links"));
			break;
		}
	} while(false);
}

void liveryfinder_co_uk::parseProfileDetail() {
	QWebElement eContentContainer = mpWebView->findFirst(QLatin1String("div#ContentInner"));

	updateData(&mData, eName, QLatin1String("span#ContentPlaceHolder1_lblTitle"), &eContentContainer);

	eContentContainer = eContentContainer.findFirst(QLatin1String("div#Left"));

	QStringList lstData = eContentContainer.findFirst(QLatin1String("span#ContentPlaceHolder1_lblAddress"))
						  .toPlainText().split("\n");
	mData.updateProperty(eAddress, lstData);

	updateData(&mData, eTel, QLatin1String("span#ContentPlaceHolder1_lblTel"), &eContentContainer);

	QWebElementCollection liFeatures = eContentContainer.findFirst(QLatin1String("ul#ContentPlaceHolder1_blFeatues"))
									   .findAll(QLatin1String("li"));
	QWebElement li;
	lstData.clear();
	foreach (li, liFeatures)
		lstData << li.toPlainText();
	mData.updateProperty(eFeatures, lstData);

	mpDBManager->insertData(&mData, mDbTableName);

	if (!gotoNextDataLink())
		showFinishMsg(QLatin1String("Finished extracting all profiles"));
}

////////////////////////////////////////////////////////////////////////

WebEngine_Main(liveryfinder_co_uk)
