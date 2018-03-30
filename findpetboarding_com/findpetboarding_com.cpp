#include "findpetboarding_com.h"

/*
 * CSS reference
 * 		http://www.w3schools.com/cssref/css_selectors.asp
 */

#define BASEURL "http://www.findpetboarding.com"

/*
 * Step1: get all links belongs to all category
 * Step2: get detail data
 */

findpetboarding_com::findpetboarding_com(QWidget *apParent)
	: BaseWebEngine(apParent) {
	// for using custom control, call this function then implement initExtWidget()
	initCustomControl();

	setWindowTitle(QLatin1String("findpetboarding_com"));
	setBaseURL(BASEURL);

	if(mpDBLogger != NULL)
		mpDBLogger->setLogPath(QLatin1String("findpetboarding_com.sql"));

	mpXMLLogger = new XMLLogger(mpLog, QLatin1String("findpetboarding_com.xml"));

	mpLog->setFilePath(QLatin1String("findpetboarding_com.txt"));
	///////////////////////////////////////////////////////////

//	// request initial
//	mCategoryIDs << QLatin1String("boardingkennels") // kennel
//				 << QLatin1String("homeboarding")
//				 << QLatin1String("boardingcatteries") // catteries
//				 << QLatin1String("dogdaycare");
}

enum enuDatabase {
	eName = 0, eCategory,
	eAdd, eTown, eCounty, ePostcode, eFullAdd,
	eTel, eEmail, eWeb,
	eDatabaseMax
};

void findpetboarding_com::initDatabase() {
	mDbLstColName << QLatin1String("Name") << QLatin1String("Category")
				  << QLatin1String("Address") << QLatin1String("Town") << QLatin1String("County") << QLatin1String("Postcode") << QLatin1String("FullAddress")
				  << QLatin1String("Tel")
				  << QLatin1String("Email")
				  << QLatin1String("Web");

	mDbLstKey << eName << ePostcode;
	mDbTableName = QLatin1String("findpetboarding_com");
}

void findpetboarding_com::doStart() {
	do {
		QString path = mpExtFileLoading->getLoadPath();
		if (path.isEmpty()) {
			meState = E_STATE_INIT;
			loadPage(BASEURL);
			break;
		}

		mlstDataLink = Utilities::loadTextData(path);
		if (!gotoNextDataLink()) {
			showFinishMsg(QLatin1String("Cannot go to next data link!"));
			break;
		}
	} while(false);
}

void findpetboarding_com::doWebLoadFinished() {
	switch(meState) {
	case E_STATE_INIT:
//		searchCategory();
		getBoardingCagetogyList();
		break;

	case E_STATE_GET_MAIN_CATEGORY:
		getCategoryLink();
		break;

	case E_STATE_GET_DETAIL_INFO:
		getDetailInfo1();
		break;

	case E_STATE_SUB1:
		getDetailInfo2();
		break;

	default:
		break;
	}
}

////////////////////////////////////////////////////////////////////////

void findpetboarding_com::getBoardingCagetogyList() {
	do {
		QWebElement menuElement = mpWebView->getElementById(QLatin1String("menu"));
		DEF_CHECK_BREAK(menuElement, QLatin1String("Menu element is null!"));

		QWebElementCollection links = menuElement.findAll(QLatin1String("a"));
		mCategoryIDs.clear();
		QWebElement link;
		QString href;
		foreach (link, links) {
			href = mpWebView->getHrefURL(&link);
			if (href.contains(QLatin1String("/advertise/")))
				continue;

			mCategoryIDs.append(href);
		}

		if (mCategoryIDs.isEmpty()) {
			showFinishMsg(QLatin1String("Category menu link is empty!"));
			break;
		}

		qDebug() << QLatin1String("Total category links: ") << mCategoryIDs.count();

		searchCategory();
	} while(false);
}


void findpetboarding_com::searchCategory() {
	QString id;

	do {
		do {
			if (mCategoryIDs.isEmpty()) {
				qDebug() << QLatin1String("Category IDs list is empty!");
				break;
			}

			id = mCategoryIDs.takeFirst();
		} while(false);
		if (id.isEmpty()) {
			if (!mlstCategoryLink.isEmpty())
				DEF_LOG_LIST(mlstCategoryLink, "");

			showFinishMsg(QLatin1String("Finished extract category links."));
			break;
		}

		meState = E_STATE_GET_MAIN_CATEGORY;
//		loadPage(QString("%1/%2/")
//				 .arg(BASEURL)
//				 .arg(id));

		loadPage(id);
	} while(false);
}

void findpetboarding_com::getCategoryLink() {
	QWebElement element;
	QWebElementCollection eCol;

	eCol = mpWebView->findAll(QLatin1String("div.column1 a"));
	if (eCol.count() == 0)
		eCol = mpWebView->findAll(QLatin1String("div.places a"));

	foreach(element, eCol)
		mlstCategoryLink << mpWebView->getHrefURL(&element);

	searchCategory();
}

////////////////////////////////////////////////////////////////////////

void findpetboarding_com::getDetailInfo1() {
	QWebElementCollection eCol;
	QWebElement element;

	// first, save links of sub-data1
	eCol = mpWebView->findAll(QLatin1String("div.listing1 a"));
	foreach(element, eCol)
		mSubData << mpWebView->getHrefURL(&element);

	// next, get the simple data
	do {
		QString category;

		category = parseCategory();

		eCol = mpWebView->findAll(QLatin1String("div.listing2"));
		foreach(element, eCol) {
			DataEntry de(mDbLstColName);
			TAG_ADDRESS tAdd;
			QString strTmp, text;

			de.updatePropertyString(eCategory, category);

			updateData(&de, eName, QLatin1String("h3"), &element);

			text = QString("Add:%1\n")
				   .arg(element.findFirst(QLatin1String("p.address")).toPlainText());

			strTmp = Utilities::extractValue2(text, QLatin1String("Add:"), QLatin1String("\n"));
			tAdd = Utilities::parseUK_Add(strTmp, false);
			de.updatePropertyString(eAdd, tAdd.strAddress);
			de.updatePropertyString(eTown, tAdd.strTown);
			de.updatePropertyString(eCounty, tAdd.strCounty);
			de.updatePropertyString(ePostcode, tAdd.strPostcode.toUpper());
			de.updatePropertyString(eFullAdd, tAdd.strFullAddress);

			Utilities::extractAndUpdate2(&de, mDbLstColName[eTel], text, QLatin1String("Tel:"));
			mlstData << de;
		}
	} while(false);

	if (mlstData.count() >= 5)
		saveData(&mlstData);

	if (!gotoNextDataLink()) {
		DEF_LOG(QLatin1String("Finished all data link, goto sub data link ..."));

		if (!gotoNextSubDataLink()) {
			if (!mlstData.isEmpty())
				saveData(&mlstData);

			showFinishMsg(QLatin1String("Finished all data link!"));
		}
	}
}

bool findpetboarding_com::gotoNextSubDataLink() {
	bool blRet = false;

	do {
		setStatus(QString("Remain link: %1").arg(mSubData.count()));

		if (mSubData.isEmpty()) {
			qDebug() << QLatin1String("Finished all sub-data ...");
			break;
		}

		QString link = mSubData.takeFirst();
		meState = E_STATE_SUB1;
		loadPage(link);

		blRet = true;
	} while (false);

	return blRet;
}

void findpetboarding_com::getDetailInfo2() {
	DataEntry de(mDbLstColName);

	QString category, strTmp, text;
	TAG_ADDRESS tAdd;
	category = parseCategory();

	de.updatePropertyString(eCategory, category);

	updateData(&de, eName, QLatin1String("span[itemprop=\"name\"]"));

	strTmp = mpWebView->findFirst(QLatin1String("p.address"))
			 .toPlainText().replace(QLatin1String("\n"), QLatin1String(","));
	tAdd = Utilities::parseUK_Add(strTmp, false);
	de.updatePropertyString(eAdd, tAdd.strAddress);
	de.updatePropertyString(eTown, tAdd.strTown);
	de.updatePropertyString(eCounty, tAdd.strCounty);
	de.updatePropertyString(ePostcode, tAdd.strPostcode.toUpper());
	de.updatePropertyString(eFullAdd, tAdd.strFullAddress);

	text = mpWebView->findFirst(QLatin1String("div[itemscope]")).toPlainText();

	Utilities::extractAndUpdate2(&de, mDbLstColName[eTel], text, QLatin1String("Tel:"));
	Utilities::extractAndUpdate2(&de, mDbLstColName[eEmail], text, QLatin1String("Email:"));
	Utilities::extractAndUpdate2(&de, mDbLstColName[eWeb], text, QLatin1String("Web:"));

	mlstData << de;
	if (mlstData.count() >= 5)
		saveData(&mlstData);

	if (!gotoNextSubDataLink()) {
		if (!mlstData.isEmpty())
			saveData(&mlstData);

		showFinishMsg(QLatin1String("Finished all data!"));
	}
}

QString findpetboarding_com::parseCategory() {
	QString tmp = Utilities::extractValue2(mpWebView->url().toString(),
										   QLatin1String("www.findpetboarding.com/"),
										   QLatin1String("/"));
	QStringList segments = tmp.split(QLatin1String("/"));

	return (segments.isEmpty() ? QLatin1String("") : segments.at(0));
}

WebEngine_Main(findpetboarding_com)
