#include "Manta_com.h"

#define URL_SEARCH_ALL_CITY "http://www.manta.com/mb_41_ALL_%1/%2?show_all_cities=1&search=%3"
// %1 = state index
// %2 = state name (full, replace space = _)
// %3 = search keyword (replace space = +)

#define MAX_DATA			100
#define MAX_WAIT_TIME		35000


manta_com::manta_com(QWidget *apParent)
	: BaseWebEngine(apParent)
{
	setWindowTitle("manta_com");

	if(mpDBLogger != NULL)
		mpDBLogger->setLogPath("manta_com.sql");

	mpLog->setFilePath("manta_com.txt");
	///////////////////////////////////////////////////////////
	loadStateList();
	loadKeywordList();

	mpTimer = new QTimer(this);
	mpTimer->setSingleShot(true);
	mpTimer->setInterval(MAX_WAIT_TIME);

	connect(mpTimer, SIGNAL(timeout()), this, SLOT(sltWaitTimeout()));
}

enum enuDatabase
{
	eName = 0, eOwner,
	ePhone,
	eAdd, eCity, eState, eZip,
	eNAICSCode, eNAICSFull,
	eLink,
	eDatabaseMax
};

void manta_com::initDatabase()
{
	mDbLstColName << "Name" << "Owner name"
				  << "Phone"
				  << "Address" << "City" << "State" << "Zip"
				  << "NAICS code" << "NAICS full"
				  << "Link";
	mDbLstKey << eName << eZip << eNAICSCode;
	mDbTableName = "manta_com";
}

void manta_com::doStart()
{
	meState = E_STATE_INIT;

	miCurState = 0;
	miCurKeyword = 0;

	buildSearchLink();
}

void manta_com::doWebLoadFinished()
{
	switch(meState)
	{
	case E_STATE_GET_MAIN_CATEGORY:
		getCityLinkList();
		break;

	case E_STATE_GET_SUB_CATEGORY:
		parseCityInfo();
		break;

	case E_STATE_GET_DETAIL_INFO:
		parseDetailData();
		break;

	default:
		qDebug() << "default";
		break;
	}
}

void manta_com::sltWaitTimeout()
{
	qDebug() << "Wait timeout";

	switch (meState)
	{
	case E_STATE_GET_MAIN_CATEGORY:
	{
		if (gotoNextCategoryLink() == false)
			showFinishMsg("Finished");
		break;
	}

	case E_STATE_GET_SUB_CATEGORY:
		if (mlstDataLink.count() >= MAX_DATA)
		{
			qDebug() << "Parse detail";

			DEF_LOG_LIST(mlstDataLink, "DataLink");

			if (gotoNextDataLink() == false)
				showFinishMsg("Finished");

			break;
		}

		if (gotoNextCategoryLink() == false)
		{
			qDebug() << "Finished all cities in state, goto datalink";
			DEF_LOG_LIST(mlstDataLink, "DataLink: ");
			DEF_LOG("End of data link");

			if (gotoNextDataLink() == false)
				showFinishMsg("Finished all data link");
		}
		break;

	default:
		break;
	}
}

bool manta_com::loadStateList()
{
	bool lblRet = false;

	mlstState.clear();
	mlstState = Utilities::loadTextData(":/us-state-name-full.txt");

	lblRet = !(mlstState.isEmpty());

	return lblRet;
}

bool manta_com::loadKeywordList()
{
	bool lblRet = false;

	mlstKeyword.clear();
	mlstKeyword << "Linen Supply Service";

	lblRet = !(mlstKeyword.isEmpty());

	return lblRet;
}

void manta_com::buildSearchLink()
{
	QString lstrStateIdx, lstrStateName, lstrKeyword;
	QString lstrURL;

	do
	{
		if (miCurState >= mlstState.count())
		{
			DEF_LOG("End of state!!!");
			break;
		}

		if (miCurKeyword >= mlstKeyword.count())
		{
			DEF_LOG("End of keyword list");
			break;
		}

		lstrStateIdx = QString("%1").arg(miCurState + 1);
		if (lstrStateIdx.length() == 1)
			lstrStateIdx.insert(0, "0");

		lstrStateName = mlstState.at(miCurState).trimmed().toLower();
		lstrStateName.replace(" ", "_");

		lstrKeyword = mlstKeyword.at(miCurKeyword).trimmed();
		lstrKeyword.replace(" ", "+");

		meState = E_STATE_GET_MAIN_CATEGORY;
		lstrURL = QString(URL_SEARCH_ALL_CITY)
				  .arg(lstrStateIdx)
				  .arg(lstrStateName)
				  .arg(lstrKeyword);
		loadPage(lstrURL);
	} while (false);
}

bool manta_com::gotoNextState()
{
	bool lblRet = false;

	do
	{
		if (miCurState >= mlstState.count())
			break;

		miCurState++;
		lblRet = true;
	} while (false);

	return lblRet;
}

void manta_com::getCityLinkList()
{
	QWebElement lElement, leLink;
	QWebElementCollection lCol;

	QString lstrTmp;

	lCol = mpWebView->findAll("li[class=\"sub_region\"]");
	foreach (lElement, lCol)
	{
		leLink = lElement.findFirst("a");
		if (leLink.isNull() == true)
			continue;

		lstrTmp = mpWebView->getHrefURL(&leLink);
		qDebug() << "Link: " << lstrTmp;

		mlstCategoryLink << lstrTmp;
	}

	qDebug() << "Found cities: " << mlstCategoryLink.count();

	//TODO: goto next state here

	{
		// Save data
		if (mlstCategoryLink.count() > 0)
		{
			DEF_LOG_LIST(mlstCategoryLink, "Found cities: ");
			DEF_LOG("End of cities =================================");
		}
	}

	{
		setStatus("Wait a little ...");
		mpTimer->start();
	}
}

void manta_com::parseCityInfo()
{
	QWebElement lElement, leLink;
	QWebElementCollection lCol;

	QString lstrTmp;

	lCol = mpWebView->findAll("h2[itemprop=\"name\"]");
	qDebug() << "Found company: " << lCol.count();
	foreach (lElement, lCol)
	{
		leLink = lElement.findFirst("a");
		if (leLink.isNull() == true)
			continue;

		lstrTmp = mpWebView->getHrefURL(&leLink);
		qDebug() << "Company: " << lstrTmp;

		mlstDataLink << lstrTmp;
	}

	if (mlstDataLink.count() > 0)
	{
		DEF_LOG_LIST(mlstDataLink, "Company list: ");
		DEF_LOG("End of company =================================");
	}

	setStatus("Wait a little...");
	mpTimer->start();
}

void manta_com::parseDetailData()
{
	QWebElement lElement;

	QString lstrTmp;

	DataEntry ldata(mDbLstColName);

	do
	{
		lElement = mpWebView->findFirst("h1[class=\"profile-company_name\"]");
		lstrTmp = "";
		if (lElement.isNull() == false)
			lstrTmp = lElement.toPlainText();
		ldata.updateProperty(mDbLstColName[eName], lstrTmp);

		lElement = mpWebView->findFirst("div[itemprop=\"streetAddress\"]");
		lstrTmp = "";
		if (lElement.isNull() == false)
			lstrTmp = lElement.toPlainText();
		ldata.updateProperty(mDbLstColName[eAdd], lstrTmp);

		lElement = mpWebView->findFirst("span[itemprop=\"addressLocality\"]");
		lstrTmp = "";
		if (lElement.isNull() == false)
			lstrTmp = lElement.toPlainText();
		ldata.updateProperty(mDbLstColName[eCity], lstrTmp);

		lElement = mpWebView->findFirst("span[itemprop=\"addressRegion\"]");
		lstrTmp = "";
		if (lElement.isNull() == false)
			lstrTmp = lElement.toPlainText();
		ldata.updateProperty(mDbLstColName[eState], lstrTmp);

		lElement = mpWebView->findFirst("span[itemprop=\"postalCode\"]");
		lstrTmp = "";
		if (lElement.isNull() == false)
			lstrTmp = lElement.toPlainText();
		ldata.updateProperty(mDbLstColName[eZip], lstrTmp);

		lElement = mpWebView->findFirst("dd[itemprop=\"telephone\"]");
		lstrTmp = "";
		if (lElement.isNull() == false)
			lstrTmp = lElement.toPlainText();
		ldata.updateProperty(mDbLstColName[ePhone], lstrTmp);

		// Owner name
		lElement = mpWebView->getElementById("profile-contacts");
		lstrTmp = parseOwnerName(&lElement);
		ldata.updateProperty(mDbLstColName[eOwner], lstrTmp);

		// NAICS code
		lstrTmp = mpWebView->getElementById("o-information").toPlainText() + "\n";
		Utilities::extractAndUpdate(&ldata, mDbLstColName[eNAICSCode], lstrTmp, "NAICS Code", ",");
		Utilities::extractAndUpdate(&ldata, mDbLstColName[eNAICSFull], lstrTmp, "NAICS Code");

		ldata.updateProperty(mDbLstColName[eLink], mpWebView->url().toString());

		mlstData << ldata;
	} while (false);

	if (mlstData.count() == 10)
	{
		mpDBLogger->logDataList(mlstData);
		DEF_LOG(QString("Save: %1").arg(mlstData.count()));
		mlstData.clear();
	}

	if (gotoNextDataLink() == false)
	{
		if (mlstData.count() > 0)
		{
			mpDBLogger->logDataList(mlstData);
			DEF_LOG(QString("Save: %1").arg(mlstData.count()));
			mlstData.clear();
		}

		showFinishMsg("Finished");

	}
}

QString manta_com::parseOwnerName(QWebElement *apProfile)
{
	QString lstrOwner = "";
	QWebElement lElement;
	QWebElementCollection lCol;

	do
	{
		if (apProfile == NULL)
			break;
		if (apProfile->isNull() == true)
			break;

		lCol = apProfile->findAll("span[itemprop=\"name\"]");
		foreach (lElement, lCol)
		{
			if (lElement.isNull() == false)
				lstrOwner += lElement.toPlainText() + "; ";
		}

		if (lstrOwner.endsWith("; ") == true)
			lstrOwner.chop(2);
	} while (false);

	return lstrOwner;
}

WebEngine_Main(manta_com)
