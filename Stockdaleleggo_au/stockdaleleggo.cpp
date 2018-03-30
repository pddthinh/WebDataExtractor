#include "stockdaleleggo.h"

#define BASEURL "http://stockdaleleggo.com.au/"

stockdaleleggo::stockdaleleggo(QWidget *apParent)
	: BaseWebEngine(apParent)
{
	setWindowTitle("stockdaleleggo");
	setBaseURL(BASEURL);

	if(mpDBLogger != NULL)
		mpDBLogger->setLogPath("stockdaleleggo.sql");

	mpLog->setFilePath("stockdaleleggo.txt");
}

enum enuDatabase
{
	eName = 0,
	ePhone, eEmail,
	eContactName, eJobTitle,
	eDatabaseMax
};

void stockdaleleggo::initDatabase()
{
	mDbLstColName << "Name"
				  << "Phone" << "Email"
				  << "Contact name" << "Job Title";
	mDbLstKey << eName << eEmail;
	mDbTableName = "stockdaleleggo";
}

void stockdaleleggo::doStart()
{
	meState = E_STATE_GET_MAIN_CATEGORY;

	loadPage(BASEURL);
}

void stockdaleleggo::doWebLoadFinished()
{
	switch(meState)
	{
	case E_STATE_GET_MAIN_CATEGORY:
		getMainCategoryList();
		break;

	case E_STATE_GET_SUB_CATEGORY:
		getTeamInfo();
		break;

	case E_STATE_GET_DETAIL_INFO:
		getDetailInfo();
		break;

	default:
		qDebug() << "default";
		break;
	}
}

void stockdaleleggo::getMainCategoryList()
{
	QWebElement	lele;
	QWebElementCollection	leColSection, leCol;

	do
	{
		leColSection = mpWebView->document().findAll("section[class=\"browse-real-estate\"]");
		foreach(lele, leColSection)
		{
			leCol = lele.findAll("a");
			foreach(lele, leCol)
				mlstCategoryLink << lele.attribute("href");
		}

		if(gotoNextCategoryLink(E_STATE_GET_SUB_CATEGORY) == false)
		{
			qDebug() << "End of category link list";
			DEF_LOG("End of category link list");

			showFinishMsg();
		}
	} while(false);
}

void stockdaleleggo::getTeamInfo()
{
	QWebElement	lele;
	QString		lstrUrl;

	do
	{
		lele = mpWebView->getElementById("team");
		if (lele.isNull() == true)
		{
			qDebug() << "Can not get team element";
			DEF_LOG("Can not get team element");

			if(gotoNextCategoryLink(E_STATE_GET_SUB_CATEGORY) == false)
			{
				qDebug() << "End of category link list";
				DEF_LOG("End of category link list");

				showFinishMsg();
			}

			break;
		}

		lstrUrl = getURL();
		lstrUrl += lele.attribute("href");
		qDebug() << "URL: " << lstrUrl;

		meState = E_STATE_GET_DETAIL_INFO;
		loadPage(lstrUrl);
	} while(false);
}

void stockdaleleggo::getDetailInfo()
{
	QWebElement	lele, leSub;
	QString lstrTmp, lstrName;
	QStringList	llstData;
	QWebElementCollection leCol;

	lstrName = "";
	lele = mpWebView->getElementById("officeHeader");
	if (lele.isNull() == false)
	{
		lstrTmp = lele.toPlainText();
		llstData = lstrTmp.split("\n");
		if (llstData.size() == 3)
		{
			lstrName = llstData[0];
			mData.updateProperty(mDbLstColName[eName], lstrName);

			lstrTmp = llstData[1];
			lstrTmp = lstrTmp.replace("Tel:", "");
			mData.updateProperty(mDbLstColName[ePhone], lstrTmp);

			mData.updateProperty(mDbLstColName[eEmail], llstData[2]);

			mlstData << mData;
			mData.clearData();
		}
	}

	leCol = mpWebView->document().findAll("div[class=\"agent-result2\"]");
	foreach (lele, leCol)
	{
		mData.updateProperty(mDbLstColName[eName], lstrName);

		leSub = lele.findFirst("div[class=\"result-header\"]");

		lstrTmp = "";
		if (leSub.isNull() == false)
			lstrTmp = leSub.toPlainText();
		mData.updateProperty(mDbLstColName[eContactName], lstrTmp);

		leSub = lele.findFirst("p[class=\"description\"]");
		if (leSub.isNull() == false)
		{
			lstrTmp = "Title:" + leSub.toPlainText() + "\n";

			Utilities::extractAndUpdate(&mData, mDbLstColName[eJobTitle], lstrTmp, "Title:");
			Utilities::extractAndUpdate(&mData, mDbLstColName[ePhone], lstrTmp, "Mobile:");
		}

		leSub = lele.findFirst("a[class=\"email\"]");
		lstrTmp = "";
		if (leSub.isNull() == false)
			lstrTmp = leSub.toPlainText();
		mData.updateProperty(mDbLstColName[eEmail], lstrTmp);

		mlstData << mData;
		mData.clearData();
	}

	logDataList();

	if(gotoNextCategoryLink(E_STATE_GET_SUB_CATEGORY) == false)
	{
		qDebug() << "End of category link list";
		DEF_LOG("End of category link list");

		showFinishMsg();
	}
}

WebEngine_Main(stockdaleleggo)
