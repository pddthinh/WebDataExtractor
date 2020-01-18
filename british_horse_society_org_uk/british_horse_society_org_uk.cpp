#include "british_horse_society_org_uk.h"

/*
 * CSS reference
 * 		http://www.w3schools.com/cssref/css_selectors.asp
 */

british_horse_society_org_uk::british_horse_society_org_uk(QWidget *apParent)
	: BaseWebEngine(apParent) {
	// for using custom control, call this function then implement initExtWidget()
	initCustomControl();

	if(mpDBLogger != NULL)
		mpDBLogger->setLogPath(QLatin1String("british_horse_society_org_uk.sql"));

	mpLog->setFilePath(QLatin1String("british_horse_society_org_uk.txt"));

#if 0
	qDebug() << QLatin1String("Open local DB:") << mpDBManager->open()
			 << QLatin1String("--> path: ") << mpDBManager->getDBPath();
	qDebug() << QLatin1String("Create temp table:") << mpDBManager->createTmpTable();
#endif

	//	connect(this, SIGNAL(finished(int)), this, SLOT(stopped()));
}

void british_horse_society_org_uk::stopped() {
	////	qDebug() << "Comming here ...";
	//	if (mpDBManager != NULL)
	//		mpDBManager->close();
}

void british_horse_society_org_uk::initConfiguration() {
	do {
		if (mpWndInfo == NULL) {
			qDebug() << QLatin1String("Window info is NULL, ignore Window initializing!");
			break;
		}

		setWindowTitle(mpWndInfo->name);
		setBaseURL(mpWndInfo->url);

		qDebug() << QLatin1String("Database is opened: ") << mpDBManager->isOpen();
		if (!mpDBManager->isOpen()) {
			mpDBManager->open();
			qDebug() << QLatin1String("Database open success: ") << mpDBManager->isOpen();
		}
	} while (0);
}

enum enuDatabase {
	eName = 0,
	eAddress,
	eTelephone,
	eEmail,
	eWebSite,
	eDescription,
	eDatabaseMax
};

void british_horse_society_org_uk::initDatabase() {
	mDbLstColName << QLatin1String("Name")
				  << QLatin1String("Address")
				  << QLatin1String("Telephone")
				  << QLatin1String("Email")
				  << QLatin1String("Website")
				  << QLatin1String("Description")
					 ;
	mDbLstKey << eName << eTelephone << eWebSite;
	mDbTableName = mpWndInfo->tableName;

	mpDBManager->createDataTable(mDbTableName,
								 mDbLstColName,
								 mDbLstKey);

	mTmpTableName = QString("tmp-%1")
					.arg(mpWndInfo->tableName);
	mpDBManager->createTmpTable(mTmpTableName);
}

void british_horse_society_org_uk::doStart() {
	do {
		if (mpExtFileLoading->getExtractLink()) {
			meState = E_STATE_INIT;
			loadPage(mpWndInfo->url);
			break;
		}

		mlstDataLink = mpDBManager->getAllFromTmpTable(mTmpTableName);
		qDebug() << QLatin1String("Filtering skipped item result: ") << mpExtFileLoading->filterSkippedItem(&mlstDataLink);

		if (!gotoNextDataLink())
			showFinishMsg(QLatin1String("Something is wrong - Data list is empty!"));
	} while (false);
}

void british_horse_society_org_uk::doWebLoadFinished() {
	switch(meState)
	{
	case E_STATE_INIT:
		getSearchClubLink();
		break;

	case E_STATE_GET_DETAIL_INFO:
		getClubDetail();
		break;

	default:
		break;
	}
}

////////////////////////////////////////////////////////////////////////

void british_horse_society_org_uk::getSearchClubLink() {
	QWebElement resultList = mpWebView->findFirst(QLatin1String("div.results-list.lookup-list"));
	QWebElementCollection linkCollection;

	linkCollection = resultList.findAll(QLatin1String("div.result.cfx h3 a"));

	QWebElement link;
	foreach (link, linkCollection) {
		mpDBManager->insertTmpTable(mpWebView->getHrefURL(&link),
									mTmpTableName);
	}

	// go to next search result
	link = mpWebView->findFirst(QLatin1String("div.pager.cfx"))
		   .findFirst(QLatin1String("li.next a"));
	QString tmp = mpWebView->getHrefURL(&link);
	if (!tmp.endsWith("#")) {
		mpWebView->doClick(&link);
	} else {
		showFinishMsg(QLatin1String("Finished extracting all Club links ..."));
	}
}

void british_horse_society_org_uk::getClubDetail() {
	QWebElement eleTitle = mpWebView->findFirst(QLatin1String("div.extra div.title h1"));
	if (eleTitle.isNull())
		eleTitle = mpWebView->findFirst(QLatin1String("div.col.main-col h1.title"));
	updateData(&mData, eName, &eleTitle);

	QWebElement eDivContainer = mpWebView->findFirst(QLatin1String("div#TwoColSB.col.sidebar div.fb div.inr"));
	QStringList txtAddress = eDivContainer.findFirst(QLatin1String("address")).toPlainText().split("\n");
	mData.updateProperty(eAddress, txtAddress);

	QString text;

	text = eDivContainer.findFirst(QLatin1String("p.tel"))
		   .findFirst(QLatin1String("span.value")).toPlainText();
	mData.updatePropertyString(eTelephone, text);

	text = eDivContainer.findFirst(QLatin1String("p.email a")).toPlainText();
	mData.updatePropertyString(eEmail, text);

	QWebElement link = eDivContainer.findFirst(QLatin1String("p.website a"));
	if (!link.isNull()) {
		text = mpWebView->getHrefURL(&link);
		mData.updatePropertyString(eWebSite, text);
	}

	text = mpWebView->findFirst(QLatin1String("div#content-columns div.col.main-col div.cont"))
		   .toPlainText();
	if (!text.isEmpty())
		mData.updatePropertyString(eDescription, text);

	mpDBManager->insertData(&mData, mDbTableName);
	mData.clearData();

	// next data
	if (!gotoNextDataLink())
		showFinishMsg(QLatin1String("Finish extracting all data!"));
}

////////////////////////////////////////////////////////////////////////
