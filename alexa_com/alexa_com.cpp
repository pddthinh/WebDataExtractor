#include "alexa_com.h"

#define BASEURL ""

#define MAX_SEARCH_PAGE		20		// alexa only return 20 result pages
#define URL_FORMAT "http://www.alexa.com/topsites/category;%1/Top/%2"
// %1: result page, ex 0, 1, ... 20
// %2: category, ex Shopping/Clothing

#define LIMIT_DATA_LINK	100
// -1 mean no limit

#define STR_INPUT_FILE_LINK "alexa_com_link.txt"

alexa_com::alexa_com(QWidget *apParent)
	: BaseWebEngine(apParent)
{
	initCustomControl();

	setWindowTitle("alexa_com");
	setBaseURL(BASEURL);

	if(mpDBLogger != NULL)
		mpDBLogger->setLogPath("alexa_com.sql");

	mpLog->setFilePath("alexa_com.txt");
	///////////////////////////////////////////////////////////
	initSearchCategory();
	setWebLoadTimeout(15000);
}

enum enuDatabase
{
	eName = 0, eAddress,
	eSiteUrl, eEmail,
	eAlexaRank, eAlexaCategory,
	eItemURL,
	eDatabaseMax
};

void alexa_com::initDatabase()
{
	mDbLstColName << "Name" << "Address"
				  << "Site URL" << "Email"
				  << "Alexa Rank" << "Alexa Category"
				  << "ItemURL";
	mDbLstKey << eName;
	mDbTableName = "alexa_com";
}

void alexa_com::doStart()
{
	do
	{
		if (mpExtFileLoading->getLoadPath().isEmpty() == true)
		{
			// Normal run
			meState = E_STATE_INIT;
			miCurPage = 0;

			browseResultUrl();
			break;
		}

		// Load data from file
		getDataLinkFromFile();
		if (gotoNextDataLink() == false)
		{
			showFinishMsg("Finished");
			break;
		}
	} while (false);
}

void alexa_com::doWebLoadFinished()
{
	switch(meState)
	{
	case E_STATE_INIT:
		getResultWebList();
		break;

	case E_STATE_GET_DETAIL_INFO:
		parseDetailData();
		break;

	default:
		qDebug() << "default";
		break;
	}
}

void alexa_com::initExtWidget()
{
	mpExtFileLoading = new ExtFileLoading(this);
}

void alexa_com::initSearchCategory()
{
	mlstSearchCategory.clear();

	mlstSearchCategory << "Shopping/Clothing";

	// Have to initial = -1
//	miCurCategory = -1;
	miCurCategory = 0;
}

bool alexa_com::gotoNextSearchCategory()
{
	bool lblRet = false;

	do
	{
		miCurCategory++;
		if (miCurCategory >= mlstSearchCategory.count())
		{
			qDebug() << "At end of search category";
			DEF_LOG("At end of search category");
			break;
		}

		lblRet = true;
	} while (false);

	return lblRet;
}

void alexa_com::browseResultUrl()
{
	QString lstrCategory;
	QString lstrUrl;

	do
	{
		if (miCurPage > MAX_SEARCH_PAGE)
		{
			qDebug() << "Alexa not support over " << MAX_SEARCH_PAGE;
			break;
		}

//		if (gotoNextSearchCategory() == false)
//		{
//			//TODO: process at end of search category
//			showFinishMsg("Finished");
//			break;
//		}

		lstrCategory = mlstSearchCategory.at(miCurCategory);
		DEF_LOG(QString("At category: %1\t\tPage: %2")
				.arg(lstrCategory)
				.arg(miCurPage));

		lstrUrl = QString(URL_FORMAT)
				  .arg(miCurPage)
				  .arg(lstrCategory);
		miCurPage++;

		loadPage(lstrUrl);
	} while (false);
}

void alexa_com::getResultWebList()
{
	QWebElement lElement;
	QWebElementCollection lCol;
	QString lstrTmp;

	do
	{
		lCol = mpWebView->findAll("li[class=\"site-listing\"]");
		foreach (lElement, lCol)
		{
			lElement = lElement.findFirst("a");
			lstrTmp = mpWebView->getHrefURL(&lElement);

			if (lstrTmp.indexOf("/siteinfo/") < 0)
				continue;

			mlstDataLink << lstrTmp;
		}

		// check limitation
		if (LIMIT_DATA_LINK > 0 &&
			mlstDataLink.count() >= LIMIT_DATA_LINK)
		{
			// Save link data
			DEF_LOG_LIST(mlstDataLink, "SearchResult:");

			// start parse detail result
			if (gotoNextDataLink() == false)
				showFinishMsg("Finished all link");

			break;
		}

		// goto next result page
		browseResultUrl();
	} while (false);
}

void alexa_com::parseDetailData()
{
	QWebElement lElement;
	QString lstrTmp, lstrAdd;
	QStringList llstTmp;

	DataEntry ldata(mDbLstColName);

	do
	{
		lElement = mpWebView->findFirst("a[class=\"url offsite\"]");
		lstrTmp.clear();
		if (lElement.isNull() == false)
			lstrTmp = lElement.attribute("href");
		ldata.updateProperty(mDbLstColName[eSiteUrl], lstrTmp);

		lElement = mpWebView->findFirst("td[class=\"start clear-border\"]");
		lstrTmp.clear();
		if (lElement.isNull() == false)
		{
			lstrTmp = lElement.toPlainText();
			lstrTmp.replace("Global Rank", "").trimmed();
		}
		ldata.updateProperty(mDbLstColName[eAlexaRank], lstrTmp);

		ldata.updateProperty(mDbLstColName[eAlexaCategory], "Clothing");

		lElement = mpWebView->getElementById("contactinfo_div");
		DEF_CHECK_BREAK(lElement, "can not get contactinfo_div");

		lElement = lElement.findFirst("div[class=\"column span-2\"]");
		DEF_CHECK_BREAK(lElement, "can not find [column span-2]");

		lstrTmp = lElement.toPlainText();
		llstTmp = lstrTmp.split("\n", QString::SkipEmptyParts);
		if (llstTmp.count() > 0)
			ldata.updateProperty(mDbLstColName[eName], llstTmp.takeFirst());

		if (llstTmp.count() > 0 &&
				llstTmp.last().indexOf(" [at] ") > 0)
		{
			lstrTmp = llstTmp.takeLast().replace(" [at] ", "@");
			ldata.updateProperty(mDbLstColName[eEmail], lstrTmp);
		}

		lstrAdd = "";
		foreach (lstrTmp, llstTmp)
			lstrAdd += lstrTmp.trimmed() + ", ";
		if (lstrAdd.endsWith(", "))
			lstrAdd.chop(2);
		ldata.updateProperty(mDbLstColName[eAddress], lstrAdd);

		ldata.updateProperty(mDbLstColName[eItemURL], mpWebView->url().toString());

		mlstData << ldata;
	} while (false);

	if (mlstData.count() >= 20)
	{
		mpDBLogger->logDataList(mlstData);
		DEF_LOG(QString("Save %1").arg(mlstData.count()));
		mlstData.clear();
	}

	if (gotoNextDataLink() == false)
	{
		if (mlstData.count() > 0)
		{
			mpDBLogger->logDataList(mlstData);
			DEF_LOG(QString("Save %1").arg(mlstData.count()));
			mlstData.clear();
		}

		showFinishMsg("Finished all link");
	}
}

void alexa_com::getDataLinkFromFile()
{
	QString lstrMsg;
	int liCount = 0;

	do
	{
		mlstDataLink.clear();
		mlstDataLink = Utilities::loadTextData(mpExtFileLoading->getLoadPath());

		while (liCount < mpExtFileLoading->getNumberOfSkipItem())
		{
			liCount ++;
			mlstDataLink.takeFirst();
		}

		if (mlstDataLink.count() == 0)
			lstrMsg = "Warning: load data link from file but size is 0";
		else
			lstrMsg = QString("Load datalink from file: %1").arg(mlstDataLink.count());
		DEF_LOG(lstrMsg);
	} while (false);
}

WebEngine_Main(alexa_com)
