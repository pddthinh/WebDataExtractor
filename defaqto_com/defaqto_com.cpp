#include "defaqto_com.h"

#define BASEURL "http://defaqto.com/"

/**
  * NOTE:
  *		For getting the URL data list, follow these steps:
  *			1. http://defaqto.com/insurance-brokers/star-ratings/ViewStarRatings/
  *			2. Select [Home Insurance] on the right hand side
  *			3. Get the iframe URL: http://defaqto.com/insurance-brokers/star-ratings/general-insurance/home-insurance/
  *			4. Select 4 & 5 star rating
  *			5. (Chrome) Select [Scrape Similar ...]
  *			6. Scrape properties:
  *				- Selector: [jQuery] --> ul[class="result-list-item"] > li
  *				- Columns:
  *					+ [Name] --> concat(./div[2]/h2, '-', ./div[2]/h3)
  *					+ [Rating] --> ./div[@class="star-rating"]/img/@alt
  *					+ [URL] --> concat('http://defaqto.com', ./div[@class="list-actions"]/ul/li[1]/a/@href)
  *			7. Select [Copy to clipboard]
  *			8. Repeat steps 4..7 for all content categories (Buildings/Content/High Net Worth)
  *			9. Combine all output into format: (separator = {tab})
  *				+ Cagegories
  *				+ URL
  **/

defaqto_com::defaqto_com(QWidget *apParent)
	: BaseWebEngine(apParent) {
// for using custom control, call this function then implement initExtWidget()
	initCustomControl();

	setWindowTitle("defaqto_com");
	setBaseURL(BASEURL);

	if(mpDBLogger != NULL)
		mpDBLogger->setLogPath("defaqto_com.sql");

	mpLog->setFilePath("defaqto_com.log");
///////////////////////////////////////////////////////////
}

enum enuDatabase {
	eCat = 0, eName,
	eRating,
	eADCB,
	eAAC,
	eGBC,
	eADUS,
	eCBSP,
	eTALP,
	eLK,
	eHEC,
	eDatabaseMax
};

void defaqto_com::initDatabase() {
	mDbLstColName << "Category" << "Name"
				  << "Rating"
//				  << "Accidental damage cover - buildings"
//				  << "Alternative accommodation cover"
//				  << "Glass breakage cover"
//				  << "Accidental damage to underground services"
//				  << "Cover for blocked sewerage pipes"
//				  << "Tracing and accessing leaking pipes"
//				  << "Loss of keys"
//				  << "Home emergency cover"
					 ;
	mDbLstKey << eName;
	mDbTableName = "defaqto_com";
}

void defaqto_com::doStart() {
	do {
		mURLData.clear();
		mURLData = Utilities::loadCSVData(mpExtFileLoading->getLoadPath(),
										  true,
										  QLatin1String("\t"));
		if (mURLData.isEmpty() == true) {
			showFinishMsg(QLatin1String("Failed to load data list!"));
			break;
		}

		if (gotoNextUrlData() == false) {
			showFinishMsg(QLatin1String("End of data list"));
			break;
		}

		meState = E_STATE_GET_DETAIL_INFO;
		loadPage(mURLItem.takeLast());
	} while (0);
}

void defaqto_com::doWebLoadFinished() {
	switch(meState) {
	case E_STATE_GET_DETAIL_INFO:
		getDetailData();
		break;

	default:
		break;
	}
}

bool defaqto_com::gotoNextUrlData() {
	bool blRet = false;

	do {
		if (mURLData.isEmpty() == true) {
			qDebug() << QLatin1String("Go to end of list URL data");
			break;
		}

		mURLItem = mURLData.takeFirst();
		blRet = true;
	} while (0);

	return blRet;
}

void defaqto_com::getDetailData() {
	QString strTmp;
	QString strValue;
	QWebElement eleBase;
	QWebElement eleTmp;
	QWebElementCollection eleCols;

	do {
		mData.clearTable();
		mData.initColName(mDbLstColName);

		// category
		mData.updateProperty(eCat, mURLItem[0]);

		eleBase = mpWebView->findFirstDiv(QLatin1String("class"),
										  QLatin1String("content-text landing-page product-detail"));
		DEF_CHECK_BREAK(eleBase, QLatin1String("Failed to get based element!"));

		// name
		eleTmp = mpWebView->findFirstElement(QLatin1String("li"),
											 QLatin1String("class"),
											 QLatin1String("current"),
											 &eleBase);
		updateData(&mData, eName, &eleTmp);

		// Rating
		eleTmp = eleBase.findFirst(QLatin1String("div[class=\"productDetailHeader\"] > img"));
		strTmp = eleTmp.attribute(QLatin1String("src"));
		strValue = DEF_STR_EMPTY;
		if (strTmp.endsWith(QLatin1String("sr_5_large.png")) == true)
			strValue = QLatin1String("5 Stars");
		else if (strTmp.endsWith(QLatin1String("sr_4_large.png")) == true)
			strValue = QLatin1String("4 Stars");
		mData.updateProperty(eRating, strValue);

		// Other properties
		eleBase = mpWebView->findFirstDiv(QLatin1String("class"),
										  QLatin1String("product-detail-features no-watermark"),
										  &eleBase);
		eleCols = eleBase.findAll(QLatin1String("li"));
		foreach (eleBase, eleCols) {
			strTmp = eleBase.findFirst(QLatin1String("h2")).toPlainText().trimmed();
			if (strTmp.isEmpty() == true)
				continue;

			strValue = eleBase.findFirst(QLatin1String("p")).toPlainText().trimmed();
			mData.addProperty(strTmp, strValue);
		}

		mlstData << mData;
		if (mlstData.size() >= 10)
			logDataList();
	} while (0);

	do {
		if (gotoNextUrlData() == false) {
			showFinishMsg(QLatin1String("End of data list"));
			if (mlstData.size() > 0)
				logDataList();
			break;
		}

		loadPage(mURLItem.takeLast());
	} while (0);
}

WebEngine_Main(defaqto_com)
