#include "thekennelclub_org_uk.h"

#define BASEURL "http://www.thekennelclub.org.uk/services/public/findaclub/area/Default.aspx"

thekennelclub_org_uk::thekennelclub_org_uk(QWidget *apParent)
	: BaseWebEngine(apParent) {
	// for using custom control, call this function then implement initExtWidget()
	initCustomControl();

	setWindowTitle(QLatin1String("thekennelclub_org_uk"));
	setBaseURL(BASEURL);

	if(mpDBLogger != NULL)
		mpDBLogger->setLogPath(QLatin1String("thekennelclub_org_uk.sql"));

	mpLog->setFilePath(QLatin1String("thekennelclub_org_uk.txt"));
	///////////////////////////////////////////////////////////
}

enum enuDatabase {
	eName = 0,
	eClub, eBreeds,
	eWebsite,
	eContactName, eContactEmail, eContactPhone, eContactMobile,
	eDatabaseMax
};

void thekennelclub_org_uk::initDatabase() {
	mDbLstColName << QLatin1String("Club Name")
				  << QLatin1String("Club")
				  << QLatin1String("Breeds")
				  << QLatin1String("Website")
				  << QLatin1String("Contact Name")
				  << QLatin1String("Contact Email")
				  << QLatin1String("Contact Phone")
				  << QLatin1String("Contact Mobile");
	mDbLstKey << eName << eWebsite;
	mDbTableName = QLatin1String("thekennelclub_org_uk");
}

void thekennelclub_org_uk::doStart() {
	do {
		// load data from path setting first
		QString path = mpExtFileLoading->getLoadPath();
		if (!path.isEmpty()) {
			mlstDataLink = Utilities::loadTextData(path);
			if (mlstDataLink.isEmpty()) {
				showFinishMsg(QLatin1String("Cannot load file in path") + path);
				break;
			}

			if (!gotoNextDataLink()) {
				showFinishMsg(QLatin1String("Data list is empty!"));
				break;
			}

			break;
		}

		meState = E_STATE_INIT;
		loadPage(BASEURL);
	} while (false);
}

void thekennelclub_org_uk::doWebLoadFinished() {
	switch(meState) {
	case E_STATE_INIT:
		getCountyList();
		break;

	case E_STATE_GET_SUB_CATEGORY:
		getClubInCounty();
		break;

	case E_STATE_GET_DETAIL_INFO:
		getClubDetail();
		break;

	default:
		qDebug() << "default";
		break;
	}
}

void thekennelclub_org_uk::getCountyList() {
	QWebElement	element;
	QWebElementCollection	eCol;

	do {
		QString link;
		QWebElement aTag;

		eCol = mpWebView->findAll(QLatin1String("*.p_ds, *.p_mw, *.bg_white"));

		mlstCategoryLink.clear();
		foreach (element, eCol) {
			aTag = element.findFirst(QLatin1String("a"));

			link = mpWebView->getHrefURL(&aTag);

			mlstCategoryLink << link;
		}

		qDebug() << QLatin1String("Extracted County: ") << mlstCategoryLink.size();

		if (!gotoNextCategoryLink())
			qDebug() << QLatin1String("Cannot go to next County link ...");
	} while (0);
}

void thekennelclub_org_uk::getClubInCounty() {
	QWebElementCollection eCol;
	QWebElement element;
	QWebElement aTag;

	do {
		eCol = mpWebView->findAll(QLatin1String("*.lightrow"));
		eCol += mpWebView->findAll(QLatin1String("*.darkrow"));

		qDebug() << QLatin1String("Club in county: ") << eCol.count();

		foreach (element, eCol) {
			aTag = element.findFirst(QLatin1String("a"));

			mlstDataLink << mpWebView->getHrefURL(&aTag);
		}

		DEF_LOG_LIST(mlstDataLink, "");
		mlstDataLink.clear();

		if (gotoNextCategoryLink())
			break;

		qDebug() << QLatin1String("End of County list");

		mlstDataLink = Utilities::loadTextData(mpLog->getFilePath());
		mlstDataLink.removeDuplicates();

		qDebug() << QLatin1String("Extracted: ") << mlstDataLink.count();
		qDebug() << QLatin1String("Removed duplicated: ") << mlstDataLink.removeDuplicates();

		mpLog->logStringList(mlstDataLink, QLatin1String("Club list in County"), false);
//		DEF_LOG_LIST(mlstDataLink, "Club list in County");

		showFinishMsg(QLatin1String("Save all clubs into file ..."));
	} while (0);
}

void thekennelclub_org_uk::getClubDetail() {
	QWebElement element;
	DataEntry data(mDbLstColName);
	QString tmp;
	QString clubInfo;

	// club name
	updateData(&data, eName, QLatin1String("#MainContent_headClubName"));

	// club info
	clubInfo = mpWebView->findFirst(QLatin1String("#MainContent_DivAcai > div[class=\"txtL\"]"))
			   .toPlainText();
	tmp = mpWebView->findFirst(QLatin1String("#MainContent_PanelNational")).toPlainText();
	if (!clubInfo.isEmpty()) {
		clubInfo += QLatin1String(", ") + tmp;
		if (clubInfo.endsWith(QLatin1String(", ")))
			clubInfo.chop(2);
	} else {
		clubInfo = tmp;
	}
	if (!clubInfo.isEmpty())
		data.updatePropertyString(eClub, clubInfo);

	// breeds info
	updateData(&data, eBreeds, QLatin1String("#MainContent_PlaceBreeds"));

	// website
	element = mpWebView->getElementById(QLatin1String("MainContent_HyperWebsite"));
	tmp = mpWebView->getHrefURL(&element);
	if (!tmp.isEmpty() && tmp != QLatin1String("http://none"))
		data.updatePropertyString(eWebsite, tmp);

	// contact name
	updateData(&data, eContactName, QLatin1String("#MainContent_DivName"));

	// contact email
	mpWebView->findFirst(QLatin1String("#MainContent_PanelEmail > p > span"))
			.removeFromDocument();
	updateData(&data, eContactEmail, QLatin1String("#MainContent_PanelEmail"));

	// contact phone
	updateData(&data, eContactPhone, QLatin1String("#MainContent_LabelPhoneDay"));

	// contact mobile
	updateData(&data, eContactMobile, QLatin1String("#MainContent_LabelPhoneEvening"));

	mlstData << data;
	if (mlstData.count() >= 1) {
		saveData(&mlstData);
		mlstData.clear();
	}

	if (!gotoNextDataLink()) {
		if (!mlstData.isEmpty())
			saveData(&mlstData);

		showFinishMsg(QLatin1String("Finished for all Club ..."));
	}
}

WebEngine_Main(thekennelclub_org_uk)
