#include "thekennelclub_org_uk3.h"

/*
 * CSS reference
 * 		http://www.w3schools.com/cssref/css_selectors.asp
 */

#define BASEURL "https://www.thekennelclub.org.uk/services/public/acbr/Default.aspx"

#define BREEDER_TABLE "breeders"

thekennelclub_org_uk3::thekennelclub_org_uk3(QWidget *apParent)
	: BaseWebEngine(apParent) {
// for using custom control, call this function then implement initExtWidget()
	initCustomControl();

	setWindowTitle(QLatin1String("thekennelclub_org_uk3"));
	setBaseURL(BASEURL);

	mpLog->setFilePath(QLatin1String("thekennelclub_org_uk3.txt"));
///////////////////////////////////////////////////////////
	QString homeDir(::getenv("HOME"));
	mpDBManager = new DBManager(homeDir + "/tmp/thekennelclub_org_uk3.db");
	qDebug() << QLatin1String("Open local DB:") << mpDBManager->open()
			 << QLatin1String("--> path: ") << mpDBManager->getDBPath();
	qDebug() << QLatin1String("Create temp table:") << mpDBManager->createTmpTable();
}

enum enuDatabase {
	eName = 0,
	eAddress,
	ePhone,
	eBreeds,
	eDatabaseMax
};

void thekennelclub_org_uk3::initDatabase() {
	mDbLstColName << QLatin1String("Name")
				  << QLatin1String("Address")
				  << QLatin1String("Phone")
				  << QLatin1String("Breeds")
					 ;
	mDbLstKey << eName << eAddress << ePhone;
	mDbTableName = QLatin1String("thekennelclub_org_uk3");

	mpDBManager->createDataTable(mDbTableName,
								 mDbLstColName,
								 mDbLstKey);
}

void thekennelclub_org_uk3::doStart() {
	do {
		if (mpExtFileLoading->getExtractLink()) {
			meState = E_STATE_INIT;
			loadPage(BASEURL);
			break;
		}

		meState = E_STATE_GET_DETAIL_INFO;
		mlstDataLink = mpDBManager->getAllFromTmpTable(BREEDER_TABLE);
		if (!gotoNextDataLink())
			showFinishMsg(QLatin1String("Unknown error - Data list is empty!"));
	} while (0);
}

void thekennelclub_org_uk3::doWebLoadFinished() {
	switch(meState)
	{
	case E_STATE_INIT:
		getBreederList();
		break;

	case E_STATE_GET_SUB_CATEGORY:
		getBreederInAllArea();
		break;

	case E_STATE_SUB1:
		getBreederListItemsInAllArea();
		break;

	case E_STATE_GET_DETAIL_INFO:
		getBreederDetailData();
		break;

	default:
		qDebug() << "default";
		break;
	}
}

////////////////////////////////////////////////////////////////////////
void thekennelclub_org_uk3::getBreederList() {
	QWebElement	element;
	QWebElementCollection	eCol;

	do {
		QString link;

		eCol = mpWebView->findAll(QLatin1String("div.p.group.p_ds.p_mw.bg_white.pad_s a"));
		foreach(element, eCol) {
			link = mpWebView->getHrefURL(&element);
			qDebug() << "Link: " << link;
			mlstCategoryLink << link;
		}

		if (!gotoNextCategoryLink())
			showFinishMsg(QLatin1String("Unknown error, there is no category link!"));
	} while (0);
}

void thekennelclub_org_uk3::getBreederInAllArea() {
	QWebElement element;
	QWebElementCollection eCol;

	element = mpWebView->getElementById(QLatin1String("MainContent_PanelCountyList"));
	eCol = element.findAll(QLatin1String("div.p.group.p_ds.p_mw.bg_white a"));

	QString link;
	foreach (element, eCol) {
		link = mpWebView->getHrefURL(&element);
		mpDBManager->insertTmpTable(link);
	}

	if (!gotoNextCategoryLink()) {
		// NEXT: get breeder list
		mlstCategoryLink = mpDBManager->getAllFromTmpTable();
		mpDBManager->createTmpTable(BREEDER_TABLE);

		if (!gotoNextCategoryLink(E_STATE_SUB1)) {
			showFinishMsg(QLatin1String("Unknown error, there is no link in temp table!"));
		}
	}
}

void thekennelclub_org_uk3::getBreederListItemsInAllArea() {
	QWebElement element;
	QWebElementCollection eLinks;
	QString link;

	element = mpWebView->findFirst(QLatin1String("table.data2.cellTop tbody"));
	eLinks = element.findAll(QLatin1String("td a"));
	foreach(element, eLinks) {
		link = mpWebView->getHrefURL(&element);
		mpDBManager->insertTmpTable(link, BREEDER_TABLE);
	}

	if (!gotoNextCategoryLink(E_STATE_SUB1))
		showFinishMsg(QLatin1String("Extracted all breeder links in all area into database"));
}

void thekennelclub_org_uk3::getBreederDetailData() {
	mData.clearData();

	QWebElement element;

	updateData(&mData, eName, QLatin1String("#MainContent_DivAB h1"));
	updateData(&mData, eAddress, QLatin1String("#MainContent_LabelLocation"));
	updateData(&mData, ePhone, QLatin1String("#MainContent_LabelPhone"));

	element = mpWebView->getElementById(QLatin1String("MainContent_DivAB"))
			  .nextSibling();
	QStringList breeds;
	QWebElementCollection eCol = element.findAll(QLatin1String("a.underline"));
	foreach(element, eCol)
		breeds << element.toPlainText();
	mData.updateProperty(eBreeds, breeds);

	mpDBManager->insertData(&mData, mDbTableName);
	mData.clearData();

	if (!gotoNextDataLink())
		showFinishMsg(QLatin1String("Finished extracting all data!"));
}
////////////////////////////////////////////////////////////////////////

WebEngine_Main(thekennelclub_org_uk3)
