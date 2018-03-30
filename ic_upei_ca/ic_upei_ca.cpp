#include "ic_upei_ca.h"

/*
 * CSS reference
 * 		http://www.w3schools.com/cssref/css_selectors.asp
 */

#define BASEURL "http://ic.upei.ca/cidd/breeds/overview"

ic_upei_ca::ic_upei_ca(QWidget *apParent)
	: BaseWebEngine(apParent) {
	// for using custom control, call this function then implement initExtWidget()
	initCustomControl();

	setWindowTitle(QLatin1String("ic_upei_ca"));
	setBaseURL(BASEURL);

	if (mpDBLogger != NULL)
		mpDBLogger->setLogPath(QLatin1String("ic_upei_ca.sql"));

	mpXMLLogger = new XMLLogger(mpLog, QLatin1String("ic_upei_ca.xml"));

	mpLog->setFilePath(QLatin1String("ic_upei_ca.txt"));
	///////////////////////////////////////////////////////////
}

enum enuDatabase {
	eName = 0,
	eImportant, eIncreased, eConfirmation, eOther,
	eDatabaseMax
};

void ic_upei_ca::initDatabase() {
	mDbLstColName << QLatin1String("Breed")
				  << QLatin1String("Important")
				  << QLatin1String("Increased")
				  << QLatin1String("Confirmation")
				  << QLatin1String("Other")
					 ;
	mDbLstKey << eName;
	mDbTableName = QLatin1String("ic_upei_ca");
}

void ic_upei_ca::doStart() {
	meState = E_STATE_INIT;
	loadPage(BASEURL);
}

void ic_upei_ca::doWebLoadFinished() {
	switch(meState)
	{
	case E_STATE_INIT:
		getBreedsList();
		break;

	case E_STATE_GET_DETAIL_INFO:
		parseBreedDetail();
		break;

	default:
		break;
	}
}

////////////////////////////////////////////////////////////////////////

void ic_upei_ca::getBreedsList() {
	QWebElementCollection eCol;
	QWebElement eTmp;

	mlstDataLink.clear();

	eCol = mpWebView->findAll(QLatin1String("div.item-list span.field-content a"));
	foreach (eTmp, eCol)
		mlstDataLink << mpWebView->getHrefURL(&eTmp);

	mpXMLLogger->writeComment(QString("item: %1").arg(mlstDataLink.size()));

	if (!gotoNextDataLink())
		qDebug() << QLatin1String("Cannot goto first data link!");
}

void ic_upei_ca::parseBreedDetail() {
	DataEntry data(mDbLstColName);

	// get breed name
	updateData(&data, eName, QLatin1String("h1.title"));

	// get disorders detail
	QWebElementCollection eCol = mpWebView->findAll(QLatin1String("div.disorder-label"));
	QWebElementCollection eLinks;
	QWebElement eTmp;
	QString tmp;
	int type;
	QStringList disothers;

	foreach(eTmp, eCol) {
		tmp = eTmp.toPlainText();

		if (tmp == "For more information about this breed")
			continue;

		eLinks = eTmp.nextSibling().findAll(QLatin1String("a"));
		if (eLinks.count() == 0)
			continue;

		type = -1;
		do {
			if (tmp.contains(QLatin1String("Important"))) {
				type = eImportant;
				break;
			}

			if (tmp.contains(QLatin1String("have an increased"))) {
				type = eIncreased;
				break;
			}

			if (tmp.contains(QLatin1String("with conformation"))) {
				type = eConfirmation;
				break;
			}

			if (tmp.contains(QLatin1String("Other disorders"))) {
				type = eOther;
				break;
			}
		} while (0);
		if (type == -1)
			continue;

		disothers.clear();
		foreach (eTmp, eLinks)
			disothers << eTmp.toPlainText();

		data.updateProperty(type, disothers);
	}

	mlstData << data;
	if (mlstData.size() >= 5)
		saveXMLData(&mlstData);

	if (!gotoNextDataLink()) {
		saveXMLData(&mlstData);

		showFinishMsg(QLatin1String("All extracted"));
	}
}

////////////////////////////////////////////////////////////////////////

WebEngine_Main(ic_upei_ca)
