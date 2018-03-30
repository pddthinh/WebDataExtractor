#include "inc_com.h"

#define BASEURL "http://www.inc.com/inc5000/search/2011/10000000-500000000/x/x/x/6-11-13-14-15-16-17-19-23-24-26-27-29-30-31-32/ND-SD-NE-KS-MN-IA-MO-IL-IN-MI-OH-WI-AZ-NM-TX-OK-LA-AR-MS-AL-TN-GA-FL-KY-SC-NC-DC-VA-WV-MD-DE-PA-VT-NH-RI-MA-CT-ME-NJ-NY/x/state/"

inc_com::inc_com(QWidget *apParent)
	: BaseWebEngine(apParent)
{
	setWindowTitle("inc_com");
	setBaseURL(BASEURL);

	if(mpDBLogger != NULL)
		mpDBLogger->setLogPath("inc_com.sql");

	mpLog->setFilePath("inc_com.txt");
///////////////////////////////////////////////////////////
	setWebLoadTimeout(15000);
}

enum enuDatabase
{
	eName = 0,
	eCityState, eFounded, eIndustry,
	e2010Rev, e2007Rev,
	eEmployee, eBModel,
	eDatabaseMax
};

void inc_com::initDatabase()
{
	mDbLstColName << "Company Name"
				  << "CityState" << "Founded" << "Industry"
				  << "2010 Revenue" << "2007 Revenue"
				  << "Employee" << "Business Model";
	mDbLstKey << eName << eFounded << eCityState;
	mDbTableName = "inc_com";
}

void inc_com::doStart()
{
	meState = E_STATE_INIT;

	loadPage(BASEURL);
}

void inc_com::doWebLoadFinished()
{
	switch(meState)
	{
	case E_STATE_INIT:
		getAllCompanyList();
		break;

	case E_STATE_GET_DETAIL_INFO:
		getDetailCompanyInfo();
		break;

	default:
		qDebug() << "default";
		break;
	}
}

void inc_com::getAllCompanyList()
{
	QWebElement lElement;
	QWebElementCollection lCol;
	QString lstrTemp;

	do
	{
		lElement = mpWebView->getElementById("fulltable");
		DEF_CHECK_BREAK(lElement, "can not get fulltable");

		lCol = lElement.findAll("a");
		foreach (lElement, lCol)
		{
			lstrTemp = mpWebView->getHrefURL(&lElement);
			if (lstrTemp.indexOf("/inc5000/profile/") < 0)
				continue;

			mlstDataLink << lstrTemp;
		}

		lstrTemp = QString("Colected link: %1").arg(mlstDataLink.count());
		qDebug() << lstrTemp;
		DEF_LOG(lstrTemp);
	} while (false);

//	if (gotoNextResultPage() == false)
	{
		// Save data link
		DEF_LOG_LIST(mlstDataLink, "DataLink:");

		if (gotoNextDataLink() == false)
			showFinishMsg("Finished");
	}
}

bool inc_com::gotoNextResultPage()
{
	bool lblRet = false;
	QWebElement lElement;
	QWebElementCollection lCol;

	QString lstrTmp;

	do
	{
		lElement = mpWebView->findFirst("div[class=\"total_pg\"]");
		DEF_CHECK_BREAK(lElement, "can not get total_pg");

		lCol = lElement.findAll("a");
		foreach (lElement, lCol)
		{
			if (lElement.toPlainText() != "Next")
				continue;

			lstrTmp = mpWebView->getHrefURL(&lElement);
			loadPage(lstrTmp);
			lblRet = true;
			break;
		}
	} while (false);

	return lblRet;
}

void inc_com::getDetailCompanyInfo()
{
	QWebElement lElement;
	QString lstrTmp;

	DataEntry ldata(mDbLstColName);

	do
	{
		lElement = mpWebView->findFirst("h1[class=\"company_name\"]");
		lstrTmp = "";
		if (lElement.isNull() == false)
			lstrTmp = lElement.toPlainText();
		ldata.updateProperty(mDbLstColName[eName], lstrTmp);

		lElement = mpWebView->findFirst("small[class=\"local\"]");
		lstrTmp = "";
		if (lElement.isNull() == false)
			lstrTmp = lElement.toPlainText();
		ldata.updateProperty(mDbLstColName[eCityState], lstrTmp);

		lElement = mpWebView->getElementById("company_blurb");
		lstrTmp = "";
		if (lElement.isNull() == false)
			lstrTmp = lElement.toPlainText();
		ldata.updateProperty(mDbLstColName[eBModel], lstrTmp);

		lElement = mpWebView->getElementById("company_tbl");
		lstrTmp = lElement.toPlainText() + "\n";

		Utilities::extractAndUpdate(&ldata, mDbLstColName[e2010Rev], lstrTmp, "2010 Revenue:", "million");
		Utilities::extractAndUpdate(&ldata, mDbLstColName[e2007Rev], lstrTmp, "2007 Revenue:", "million");
		Utilities::extractAndUpdate(&ldata, mDbLstColName[eEmployee], lstrTmp, "Employees:");
		Utilities::extractAndUpdate(&ldata, mDbLstColName[eFounded], lstrTmp, "Founded:");
		Utilities::extractAndUpdate(&ldata, mDbLstColName[eIndustry], lstrTmp, "Industry:");

		mlstData << ldata;
	} while (false);

	if (mlstData.count() >= 10)
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
		}

		showFinishMsg("Finished");
	}
}

WebEngine_Main(inc_com)
