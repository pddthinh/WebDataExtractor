#include "icatcare_org.h"

/*
 * CSS reference
 * 		http://www.w3schools.com/cssref/css_selectors.asp
 */

#define BASEURL "http://icatcare.org/advice/cat-breeds"

icatcare_org::icatcare_org(QWidget *apParent)
	: BaseWebEngine(apParent) {
	// for using custom control, call this function then implement initExtWidget()
	initCustomControl();

	setWindowTitle(QLatin1String("icatcare_org"));
	setBaseURL(BASEURL);

	if(mpDBLogger != NULL)
		mpDBLogger->setLogPath(QLatin1String("icatcare_org.sql"));

	mpXMLLogger = new XMLLogger(mpLog, QLatin1String("icatcare_org.xml"));

	mpLog->setFilePath(QLatin1String("icatcare_org.txt"));
	///////////////////////////////////////////////////////////
}

enum enuDatabase {
	eName = 0,
	eDisorders,
	eDatabaseMax
};

void icatcare_org::initDatabase() {
	mDbLstColName << QLatin1String("Name")
				  << QLatin1String("Disorders")
					 ;
	mDbLstKey << eName;
	mDbTableName = QLatin1String("icatcare_org");
}

void icatcare_org::doStart() {
	meState = E_STATE_INIT;
	loadPage(BASEURL);
}

void icatcare_org::doWebLoadFinished() {
	switch(meState)
	{
	case E_STATE_INIT:
		getCatBreeds();
		break;

	case E_STATE_GET_DETAIL_INFO:
		getCatDetail();
		break;

	default:
		qDebug() << "default";
		break;
	}
}

////////////////////////////////////////////////////////////////////////

void icatcare_org::getCatBreeds() {
	QWebElementCollection eCol;
	QWebElement eTmp;

	eCol = mpWebView->findAll(QLatin1String("div.a-z_content div.wrappedAreas a"));
	foreach (eTmp, eCol)
		mlstDataLink << mpWebView->getHrefURL(&eTmp);

	qDebug() << QString("Extracted link: %1").arg(mlstDataLink.size());
	mEmptyDataLinks.clear();

	mpXMLLogger->writeComment(QString("item: %1").arg(mlstDataLink.size()));

	if (!gotoNextDataLink())
		showFinishMsg(QLatin1String("Cannot go to data link!"));
}

void icatcare_org::getCatDetail() {
	DataEntry dt(mDbLstColName);
	QWebElement eContainer;
	QWebElement eLast, eFirst, eTmp;
	QWebElementCollection eCol;

	do {
		if (!mNameSaved.isEmpty()) {
			dt.updatePropertyString(eName, mNameSaved);
			mNameSaved.clear();
		} else {
			updateData(&dt, eName, QLatin1String("#page-title"));
		}

		eContainer = mpWebView->findFirstDiv(QLatin1String("class"),
											 QLatin1String("field-item even"));
		eFirst = eContainer.findFirst(QLatin1String("h2"));
		eLast = eContainer.findFirst(QLatin1String("h2.boxout"));

		if (eFirst.isNull() && eLast.isNull()) {
			// there is case:
			//		http://icatcare.org/advice/cat-breeds/anatolian-turkish-shorthair

			// special item
			QString text = eContainer.toPlainText().trimmed();
			if (text.startsWith("See ")) {
				eTmp = eContainer.findFirst(QLatin1String("a"));

				mNameSaved = dt.getPropertyString(mDbLstColName[eName]);
				DEF_LOG(QString("Redirected for [%1]").arg(mNameSaved));

				loadPage(mpWebView->getHrefURL(&eTmp));
				break;
			}
		}

		QStringList disorders;
		QString tmp;
		while (!(eTmp = eFirst.nextSibling()).isNull() && eTmp != eLast ) {
			eFirst = eTmp;

			eCol = eTmp.findAll(QLatin1String("a"));
			foreach (eTmp, eCol) {
				tmp = eTmp.toPlainText();
				if (tmp.startsWith(QLatin1String("click here for more information")))
					continue;

				disorders << tmp;
			}
		}
		dt.updateProperty(eDisorders, disorders);

		// check to save empty data item
		do {
			if (!eLast.isNull())
				break;

			if (disorders.isEmpty())
				mEmptyDataLinks << mpWebView->url().toString();
		} while (0);

		mlstData << dt;
		if (mlstData.size() >= 5)
			saveXMLData(&mlstData);

		if (!gotoNextDataLink()) {
			saveXMLData(&mlstData);

			showFinishMsg();
			mpLog->logStringList(mEmptyDataLinks);
		}
	} while (0);
}

////////////////////////////////////////////////////////////////////////

WebEngine_Main(icatcare_org)
