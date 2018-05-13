#include "dogsandpuppies.h"

/*
 * CSS reference
 * 		http://www.w3schools.com/cssref/css_selectors.asp
 */

#define BASEURL "https://www.dogsandpuppies.co.uk/business-list/Breeder/%1?page=%2"

dogsandpuppies::dogsandpuppies(QWidget *apParent)
	: BaseWebEngine(apParent) {
// for using custom control, call this function then implement initExtWidget()
	initCustomControl();

	setWindowTitle(QLatin1String("dogsandpuppies"));
	setBaseURL(BASEURL);

	if(mpDBLogger != NULL)
		mpDBLogger->setLogPath(QLatin1String("dogsandpuppies.sql"));

	mpXMLLogger = new XMLLogger(mpLog, QLatin1String("dogsandpuppies.xml"));

	mpLog->setFilePath(QLatin1String("dogsandpuppies.txt"));

	QString homeDir(::getenv("HOME"));
	mpDBManager = new DBManager(homeDir + "/tmp/database.db");
///////////////////////////////////////////////////////////
	miCatalog = 0; // start from 'A'

	qDebug() << QLatin1String("Open local DB:") << mpDBManager->open()
			 << QLatin1String("--> path: ") << mpDBManager->getDBPath();
	qDebug() << QLatin1String("Create temp table:") << mpDBManager->createTmpTable();
}

enum enuDatabase {
	eName = 0,
	eAdd, eTown, eCounty, ePostcode, eFullAdd,
	eTel, eMobile, eEmail, eWeb,
	eDescription,
	eDatabaseMax
};

void dogsandpuppies::initDatabase() {
	mDbLstColName << QLatin1String("Name")
				  << QLatin1String("Address") << QLatin1String("Town") << QLatin1String("County") << QLatin1String("Postcode") << QLatin1String("FullAddress")
				  << QLatin1String("Tel") << QLatin1String("Mobile")
				  << QLatin1String("Email")
				  << QLatin1String("Web")
				  << QLatin1String("Description");

	mDbLstKey << eName << eTel << eMobile << eWeb;
	mDbTableName = QLatin1String("dogsandpuppies");
}

void dogsandpuppies::doStart() {
	do {
		if (mpExtFileLoading->getExtractLink()) {
			meState = E_STATE_INIT;

			QString startCategory = mpExtFileLoading->getParamCategory();
			int startIdx = mpExtFileLoading->getParamPageIndex();

			if (startCategory.length() == 1)
				miCatalog = startCategory.at(0).toUpper().toLatin1() - 'A';
			if (startIdx <= 0)
				startIdx = 1;

			QString url = QString(BASEURL)
						  .arg((char)(miCatalog + 'A'))
						  .arg(startIdx)
						  ;
			qDebug() << QLatin1String("URL: ") << url;
			loadPage(url);
			break;
		}

		if (!mpExtFileLoading->getLoadPath().isEmpty()) {
			int skipped = mpExtFileLoading->getNumberOfSkipItem();
			mlstDataLink = Utilities::loadTextData(mpExtFileLoading->getLoadPath());
			while (skipped > 0)
				mlstDataLink.takeFirst();

			if (!gotoNextDataLink(E_STATE_GET_DETAIL_INFO)) {
				showFinishMsg(QLatin1String("Error! Can not go to next data link!"));
				break;
			}
		}
	} while(false);
}

void dogsandpuppies::doWebLoadFinished() {
	switch(meState)
	{
	case E_STATE_INIT:
		extractBreeders();
		break;

	case E_STATE_GET_DETAIL_INFO:
		extractDetail();
		break;

	default:
		qDebug() << "default";
		break;
	}
}

////////////////////////////////////////////////////////////////////////

bool dogsandpuppies::isEndCategory() {
	return (miCatalog > ('A' - 'A'));
}

void dogsandpuppies::extractBreeders() {
	do {
		QWebElementCollection links;
		QWebElement link;
		QString linkURL;

		meState = E_STATE_MAX;

		links = mpWebView->findAll(QLatin1String("div.business_block div.float_left.business_details > a"));
		if (links.count() == 0) {
			qDebug() << QLatin1String("End of page for category: ") << (char)(miCatalog + 'A');

			// go to next category
			miCatalog ++;

			if (isEndCategory()) {
				// show end of category list
				showFinishMsg(QLatin1String("Finish parsing Breeders category list!"));
				break;
			}

			meState = E_STATE_INIT;
			QString url = QString(BASEURL)
						  .arg((char)(miCatalog + 'A'))
						  .arg(1)
						  ;
			loadPage(url);
			break;
		}

		int processed = 0;

		foreach(link, links) {
			linkURL = mpWebView->getHrefURL(&link);
//			mpDBManager->insertTmpTable(linkURL);
			mlstDataLink << linkURL;
			processed++;
		}
		if (processed > 0) {
			qDebug() << QLatin1String("-->> Processed list: ") << processed;
		}

		if (mlstDataLink.count() > 0) {
			DEF_LOG_LIST(mlstDataLink, "");
			mlstDataLink.clear();
		}

		// go to next page
		link = mpWebView->findFirst(QLatin1String("a.nav_next"));
		if (!link.isNull()) {
			if (mlstDataLink.count() > 0) {
				DEF_LOG_LIST(mlstDataLink, "");
				mlstDataLink.clear();
			}

			meState = E_STATE_INIT;
			mpWebView->doClick(&link);
			break;
		}

		qDebug() << QLatin1String("End of page for category: ") << (char)miCatalog;

		// go to next category
		miCatalog ++;

		if (isEndCategory()) {
			if (mlstDataLink.count() > 0) {
				DEF_LOG_LIST(mlstDataLink, "");
				mlstDataLink.clear();
			}

			// show end of category list
			showFinishMsg(QLatin1String("Finish parsing Breeders category list!"));
			break;
		}

		meState = E_STATE_INIT;
		QString url = QString(BASEURL)
					  .arg((char)(miCatalog + 'A'))
					  .arg(1)
					  ;
		loadPage(url);
	} while (false);
}

void dogsandpuppies::extractDetail() {
	do {
		DataEntry de(mDbLstColName);
		QString strTmp;
		QWebElement eleLink;

		meState = E_STATE_MAX;

		updateData(&de, eName, QLatin1String("div.trader_name"));
		updateData(&de, eDescription, QLatin1String("div.descr"));

		updateData(&de, eTel, QLatin1String("span.item.phone"));
		updateData(&de, eMobile, QLatin1String("span.item.mobile"));

		eleLink = mpWebView->findFirst(QLatin1String("a.item.email"));
		strTmp = eleLink.attribute(QLatin1String("href"));
		if (strTmp != "#mail_trader")
			de.updatePropertyString(eEmail, strTmp);

		eleLink = mpWebView->findFirst(QLatin1String("a.item.website"));
		strTmp = mpWebView->getHrefURL(&eleLink);
		de.updatePropertyString(eWeb, strTmp);

		eleLink = mpWebView->findFirst(QLatin1String("div.location b"));
		strTmp = eleLink.toPlainText().trimmed();
		if (strTmp.endsWith("\n"))
			strTmp.chop(1);
		QString tmp = strTmp;
		de.updatePropertyString(eFullAdd, tmp);

		QStringList locationSegs = strTmp.split("\n");

		int iSegment = locationSegs.count();

		if (!locationSegs.isEmpty())
			de.updatePropertyString(eAdd, locationSegs.at(0));
		if (iSegment > 1) {
			strTmp = locationSegs.at(1);
			if (strTmp.indexOf(',') < 0)
				de.updatePropertyString(ePostcode, strTmp);
			else
				de.updatePropertyString(eTown, strTmp);
		}
		if (iSegment > 2)
			de.updatePropertyString(eTown, locationSegs.at(2));

		mlstData << de;

		if (mlstData.count() >= 5)
			saveData(&mlstData);

		if (!gotoNextDataLink(E_STATE_GET_DETAIL_INFO)) {
			if (mlstData.count() > 0)
				saveData(&mlstData);

			showFinishMsg(QLatin1String("Finish extract all data links"));
			break;
		}
	} while (false);
}
////////////////////////////////////////////////////////////////////////

WebEngine_Main(dogsandpuppies)
