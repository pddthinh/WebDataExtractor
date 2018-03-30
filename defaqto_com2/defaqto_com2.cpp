#include "defaqto_com2.h"

/*
 * CSS reference
 * 		http://www.w3schools.com/cssref/css_selectors.asp
 */

#define BASEURL "https://www.defaqto.com/star-ratings/pet-insurance"

defaqto_com2::defaqto_com2(QWidget *apParent)
	: BaseWebEngine(apParent) {
	// for using custom control, call this function then implement initExtWidget()
	initCustomControl();

	setWindowTitle(QLatin1String("defaqto_com2"));
	setBaseURL(BASEURL);

	if(mpDBLogger != NULL)
		mpDBLogger->setLogPath(QLatin1String("defaqto_com2.sql"));

	mpXMLLogger = new XMLLogger(mpLog, QLatin1String("defaqto_com2.xml"));

	mpLog->setFilePath(QLatin1String("defaqto_com2.txt"));
	///////////////////////////////////////////////////////////
}

enum enuDatabase {
	eName = 0, eCategory, eDescription, eRating,
	eRow1, eRow2, eRow3, eRow4, eRow5, eRow6, eRow7, eRow8,
	eDatabaseMax
};

void defaqto_com2::initDatabase() {
	mDbLstColName << QLatin1String("Name")
				  << QLatin1String("Category")
				  << QLatin1String("Description") << QLatin1String("Rating")
				  << QLatin1String("Row1")
				  << QLatin1String("Row2")
				  << QLatin1String("Row3")
				  << QLatin1String("Row4")
				  << QLatin1String("Row5")
				  << QLatin1String("Row6")
				  << QLatin1String("Row7")
				  << QLatin1String("Row8")

					 ;
	mDbLstKey << eName;
	mDbTableName = QLatin1String("defaqto_com2");
}

void defaqto_com2::doStart() {
	meState = E_STATE_INIT;
	loadPage(BASEURL);
}

void defaqto_com2::doWebLoadFinished() {
	switch(meState)
	{
	case E_STATE_INIT:
		getProductList();
		break;

	case E_STATE_GET_SUB_CATEGORY:
		getCompanyList();
		break;

	case E_STATE_GET_DETAIL_INFO:
		getCompanyDetail();
		break;

	default:
		break;
	}
}

////////////////////////////////////////////////////////////////////////
void defaqto_com2::getProductList() {
	QWebElementCollection eCol = mpWebView->findAll(QLatin1String("div.product-specific a"));
	QWebElement eTmp;

	foreach (eTmp, eCol)
		mlstCategoryLink << mpWebView->getHrefURL(&eTmp);

	if (!gotoNextCategoryLink())
		showFinishMsg(QLatin1String("Cannot load next category"));
}

void defaqto_com2::getCompanyList() {
	QWebElementCollection eCol;
	QWebElement eTmp;

	do {
		eCol = mpWebView->findAll(QLatin1String("ul.result-list-item div.list-actions li > a"));
		foreach (eTmp, eCol) {
			if (!eTmp.toPlainText().startsWith(QLatin1String("more")))
				continue;

			mlstDataLink << mpWebView->getHrefURL(&eTmp);
		}

		if (gotoNextCategoryLink())
			break;

		qDebug() << QString("Extracted links: %1").arg(mlstDataLink.size());
		mpXMLLogger->writeComment(QString("item: %1").arg(mlstDataLink.size()));

//		mpLog->logStringList(mlstDataLink);

		if (!gotoNextDataLink())
			showFinishMsg(QLatin1String("Cannot goto next datalink!"));
	} while (false);
}

void defaqto_com2::getCompanyDetail() {
	DataEntry dt(mDbLstColName);

	QWebElement eTmp;
	QString text, star;

	QWebElementCollection eCol;

	// header
	{
		updateData(&dt, eName, QLatin1String("div.productDetailHeader h2"));

		updateData(&dt, eDescription, QLatin1String("div.productDetailHeader h3"));

		text = mpWebView->findFirst((QLatin1String("div.productDetailHeader img")))
			  .attribute(QLatin1String("src"));
		star = Utilities::extractValue(text, QLatin1String("/sr_"), QLatin1String("_large_16.png"));
		if (!star.isEmpty())
			star += " star";
		dt.updatePropertyString(eRating, star);
	}

	// category
	text = Utilities::extractValue(mpWebView->url().toString(),
								   QLatin1String("=StarRatingsCenter_PetInsurance_"),
								   QLatin1String("_ProductDetails&"));
	dt.updatePropertyString(eCategory, text);

	eCol = mpWebView->findAll(QLatin1String("div.product-detail-features ul li"));
	int idx = eRow1;
	QStringList rows;
	foreach (eTmp, eCol) {
		rows.clear();

		rows << eTmp.findFirst(QLatin1String("h2")).toPlainText();
		rows << eTmp.findFirst(QLatin1String("p")).toPlainText();
		dt.updateProperty(idx, rows);

		idx ++;
	}

	mlstData << dt;
	if (mlstData.size() >= 5)
		saveXMLData(&mlstData);

	if (!gotoNextDataLink()) {
		if (mlstData.size() > 0)
			saveXMLData(&mlstData);

		showFinishMsg();
	}
}

////////////////////////////////////////////////////////////////////////

WebEngine_Main(defaqto_com2)
