#include "thekennelclub_org_uk2.h"

#define BASEURL "https://www.thekennelclub.org.uk/services/public/findapuppy/Default.aspx"

#define BREEDER_TABLE "breeders"

thekennelclub_org_uk::thekennelclub_org_uk(QWidget *apParent)
	: BaseWebEngine(apParent) {
	// for using custom control, call this function then implement initExtWidget()
	initCustomControl();

	setWindowTitle(QLatin1String("thekennelclub_org_uk_2"));
	setBaseURL(BASEURL);

	if(mpDBLogger != NULL)
		mpDBLogger->setLogPath(QLatin1String("thekennelclub_org_uk2.sql"));

	mpLog->setFilePath(QLatin1String("thekennelclub_org_uk2.txt"));
	///////////////////////////////////////////////////////////

	QString homeDir(::getenv("HOME"));
	mpDBManager = new DBManager(homeDir + "/tmp/thekennelclub_org_uk2.db");

	qDebug() << QLatin1String("Open local DB:") << mpDBManager->open()
			 << QLatin1String("--> path: ") << mpDBManager->getDBPath();
	qDebug() << QLatin1String("Create temp table:") << mpDBManager->createTmpTable();
}

enum enuDatabase {
	eName = 0,
	eContactPhone, eAddress,
	eDateOfBirth, eDateOfReg,
	eNoBitches, eNoDogs,
	eAds,
	eDatabaseMax
};

void thekennelclub_org_uk::initDatabase() {
	mDbLstColName << QLatin1String("Breeder Name")
				  << QLatin1String("Contact Phone")
				  << QLatin1String("Contact Address")
				  << QLatin1String("Date-Of-Birth")
				  << QLatin1String("Date-Of-Registration")
				  << QLatin1String("No. Bitches")
				  << QLatin1String("No. Dogs")
				  << QLatin1String("Ads")
					 ;
	mDbLstKey << eName << eContactPhone;
	mDbTableName = QLatin1String("thekennelclub_org_uk_findapuppy");
}

void thekennelclub_org_uk::doStart() {
	do {
		qDebug() << QLatin1String("Create data table: ")
				 << mpDBManager->createDataTable(mDbTableName,
												 mDbLstColName,
												 mDbLstKey);
		if (!mpExtFileLoading->getExtractLink()) {
			meState = E_STATE_GET_DETAIL_INFO;
			mlstDataLink = mpDBManager->getAllFromTmpTable(BREEDER_TABLE);

			if (!gotoNextDataLink())
				showFinishMsg(QLatin1String("Unknown error - Data list is empty"));

			break;
		}

#if 1
		meState = E_STATE_INIT;
		loadPage(BASEURL);
#else
		meState = E_STATE_SUB1;
		mlstCategoryLink = mpDBManager->getAllFromTmpTable();
		mpDBManager->createTmpTable(BREEDER_TABLE);
		loadPage(mlstCategoryLink.takeFirst());
#endif
	} while (false);
}

void thekennelclub_org_uk::doWebLoadFinished() {
	switch(meState) {
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
		getBreederDetailInfo();
		break;

	default:
		break;
	}
}

void thekennelclub_org_uk::getBreederList() {
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

void thekennelclub_org_uk::getBreederInAllArea() {
	QWebElement element;

	element = mpWebView->getElementById(QLatin1String("MainContent_HyperAllList"));
	QString link = mpWebView->getHrefURL(&element);

	mpDBManager->insertTmpTable(link);

//	mlstDataLink << link;
//	if (mlstDataLink.count() >= 5) {
//		DEF_LOG_LIST(mlstDataLink, "");
//		qDebug() << QLatin1String("Saved item: ") << mlstDataLink.count();
//		mlstDataLink.clear();
//	}

	if (!gotoNextCategoryLink()) {
//		if (mlstDataLink.count() > 0)
//			DEF_LOG_LIST(mlstDataLink, "");

		// NEXT: get breeder list
		mlstCategoryLink = mpDBManager->getAllFromTmpTable();
		mpDBManager->createTmpTable(BREEDER_TABLE);

		if (!gotoNextCategoryLink(E_STATE_SUB1)) {
			showFinishMsg(QLatin1String("Unknown error, there is no link in temp table!"));
		}
	}
}

void thekennelclub_org_uk::getBreederListItemsInAllArea() {
	QWebElement element;
	QWebElementCollection eLinks;
	QString link;

	element = mpWebView->findFirst(QLatin1String("table.data2 tbody"));
	eLinks = element.findAll(QLatin1String("a.b"));
	foreach(element, eLinks) {
		link = mpWebView->getHrefURL(&element);
		mpDBManager->insertTmpTable(link, BREEDER_TABLE);
	}

	if (!gotoNextCategoryLink(E_STATE_SUB1))
		showFinishMsg(QLatin1String("Extracted all breeder links in all area into database"));
}

void thekennelclub_org_uk::getBreederDetailInfo() {
	QWebElement element;
	DataEntry data(mDbLstColName);
	QString tmp;

	element = mpWebView->findFirst(QLatin1String("#MainContent_LabelBreeder span.b"));
	QStringList lstTmp = element.toPlainText().split("\n");
	if (lstTmp.size() > 0) {
		data.updatePropertyString(eName, lstTmp.at(0));

		tmp = element.toPlainText().replace(data.getPropertyString(eName) + "\n", "")
			  .replace(QLatin1String("\n"), QLatin1String(", "));
		if (tmp.endsWith(", "))
			tmp.chop(2);

		data.updatePropertyString(eAddress, tmp);
	}

	updateData(&data, eContactPhone, QLatin1String("#MainContent_LabelPhone"));
	updateData(&data, eDateOfBirth, QLatin1String("#MainContent_LabelDateBirth"));
	updateData(&data, eDateOfReg, QLatin1String("#MainContent_LabelDateReg"));
	updateData(&data, eNoBitches, QLatin1String("#MainContent_LabelBitchCount"));
	updateData(&data, eNoDogs, QLatin1String("#MainContent_LabelDogCount"));

	element = mpWebView->findFirst(QLatin1String("#MainContent_DivText div.p.group.p_md.bg_white"));
	tmp = element.toPlainText().replace(QLatin1String("Breeder Advertisement"), QLatin1String(""));
	data.updatePropertyString(eAds, tmp);

	mpDBManager->insertData(&data, mDbTableName);

	if (!gotoNextDataLink())
		showFinishMsg(QLatin1String("Finished extracting all breeders info ..."));
}

WebEngine_Main(thekennelclub_org_uk)
