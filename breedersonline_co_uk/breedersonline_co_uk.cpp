#include "breedersonline_co_uk.h"

#include <QNetworkProxy>
#include <QString>

/*
 * CSS reference
 * 		http://www.w3schools.com/cssref/css_selectors.asp
 */

#define BASEURL "https://www.breedersonline.co.uk/pedigree-breeder.php?adtype=DB&breedid=5"

#define BREEDER_SEARCH_TEMPLATE "https://www.breedersonline.co.uk/pedigree-breeder.php?adtype=DB&breedid=%1"


#define DB_BREEDER_TABLE "Breeder"


breedersonline_co_uk::breedersonline_co_uk(QWidget *apParent)
	: BaseWebEngine(apParent) {
// for using custom control, call this function then implement initExtWidget()
	initCustomControl();

	setWindowTitle(QLatin1String("breedersonline_co_uk"));
	setBaseURL(BASEURL);

	if(mpDBLogger != NULL)
		mpDBLogger->setLogPath(QLatin1String("breedersonline_co_uk.sql"));

	mpXMLLogger = new XMLLogger(mpLog, QLatin1String("breedersonline_co_uk.xml"));

	mpLog->setFilePath(QLatin1String("breedersonline_co_uk.txt"));
///////////////////////////////////////////////////////////
	QString homeDir(::getenv("HOME"));
	mpDBManager = new DBManager(homeDir + "/tmp/breedersonline_co_uk.db");
	qDebug() << QLatin1String("Open local DB:") << mpDBManager->open()
			 << QLatin1String("--> path: ") << mpDBManager->getDBPath();
	qDebug() << QLatin1String("Create temp table:") << mpDBManager->createTmpTable();

#if 0
	QNetworkProxy proxy;
	proxy.setType(QNetworkProxy::Socks5Proxy);
	proxy.setType(QNetworkProxy::HttpProxy);
	proxy.setHostName(QLatin1String("sin2.sme.zscalertwo.net"));
	proxy.setPort(80);
	QNetworkProxy::setApplicationProxy(proxy);
#endif
}

enum enuDatabase {
	eName = 0,
	eDatabaseMax
};

void breedersonline_co_uk::initDatabase() {
	mDbLstColName << QLatin1String("Name");
	mDbLstKey << eName;
	mDbTableName = QLatin1String("breedersonline_co_uk");

	mpDBManager->createDataTable(mDbTableName,
								 mDbLstColName,
								 mDbLstKey);
}

void breedersonline_co_uk::doStart() {
	meState = E_STATE_INIT;
	loadPage(BASEURL);
}

void breedersonline_co_uk::doWebLoadFinished() {
	switch(meState)
	{
	case E_STATE_INIT:
//		getBreederIdList();
		getBreederLinks();
		break;

	case E_STATE_GET_MAIN_CATEGORY:
		qDebug() << "E_STATE_GET_MAIN_CATEGORY";
		break;

	case E_STATE_GET_SUB_CATEGORY:
		getBreederLinks();
		break;

	case E_STATE_GET_DETAIL_INFO:
		qDebug() << "E_STATE_GET_DETAIL_INFO";
		break;

	default:
		qDebug() << "default";
		break;
	}
}

////////////////////////////////////////////////////////////////////////
void breedersonline_co_uk::loadList() {
	mpWebView->doScroll(0, 1000);
}

void breedersonline_co_uk::getBreederIdList() {
	mBreederIds.clear();

	QWebElement opt;
	QWebElementCollection options = mpWebView->getElementById(QLatin1String("breedid"))
									.findAll(QLatin1String("option"));

	foreach(opt, options) {
		mBreederIds << opt.attribute(QLatin1String("value"));
	}

	qDebug() << QLatin1String("Total breeders: ") << mBreederIds.count();
	if (!gotoNextBreeder())
		showFinishMsg(QLatin1String("Unknown error - BreederId is empty!"));
}

bool breedersonline_co_uk::gotoNextBreeder(ENU_STATE state) {
	bool blRet = false;

	do {
		if (mBreederIds.isEmpty())
			break;

		QString id = mBreederIds.takeFirst();
		QString url = QString(BREEDER_SEARCH_TEMPLATE).arg(id);

		meState = state;
		loadPage(url);

		blRet = true;
	} while (false);

	return blRet;
}

void breedersonline_co_uk::getBreederLinks() {
	int i = 1;
	QWebElement eEnd;

	do {
		mpWebView->doScroll(0, 1000 * i);
		i++;
		QThread::sleep(5);

		eEnd = mpWebView->findFirst(QLatin1String("button.btn.btn-danger.btn-lg.btn-block"));
	} while (eEnd.isNull());
	qDebug() << QLatin1String("End of breeder list in page ...");

	qDebug() << mpWebView->getElementById("searchresults").isNull();
	qDebug() << "h4: " << mpWebView->findAll("h4 a").count();

	while (mpWebView->getElementById("searchresults").toPlainText().isEmpty()) {
		QThread::sleep(1);
	}

	qDebug() << "------------------------------------";
	qDebug() << mpWebView->getElementById("searchresults").toPlainText();
	qDebug() << "------------------------------------";

	QWebElementCollection colTmp = mpWebView->findAll("h4");
	foreach(eEnd, colTmp)
		qDebug() << eEnd.toPlainText();

	QWebElementCollection rows = mpWebView->getElementById(QLatin1String("searchresults"))
								 .findAll(QLatin1String("div.row.advertrow"));
	rows = mpWebView->findAll(QLatin1String("div.col-sm-2"));
	qDebug() << "rows: " << rows.count();
	QWebElement rElement;
	QWebElement eLink;
	QString link;

	foreach (rElement, rows) {
		eLink = rElement.findFirst(QLatin1String("a"));
		link = mpWebView->getHrefURL(&eLink);

		mpDBManager->insertTmpTable(link, DB_BREEDER_TABLE);
	}

	if (!gotoNextBreeder())
		showFinishMsg(QLatin1String("Finish extracting breeder list"));
}

////////////////////////////////////////////////////////////////////////

WebEngine_Main(breedersonline_co_uk);
