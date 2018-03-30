#include "ncua_gov.h"

#define BASEURL "http://researchcu.ncua.gov/Views/FindCreditUnions.aspx"

ncua_gov::ncua_gov(QWidget *apParent)
	: BaseWebEngine(apParent)
{
	setWindowTitle("ncua_gov");
	setBaseURL(BASEURL);

	if(mpDBLogger != NULL)
		mpDBLogger->setLogPath("ncua_gov.sql");

	mpLog->setFilePath("ncua_gov.txt");
	///////////////////////////////////////////////////////////
}

enum enuDatabase
{
	eName = 0,
	eAdd, eCity, eZip,
	eCountry, eCounty,
	ePhone, eWeb,
	eChaterNum,
	eDatabaseMax
};

void ncua_gov::initDatabase()
{
	mDbLstColName << "Name"
				  << "Add" << "City" << "Zip"
				  << "Country" << "County"
				  << "Phone" << "Web"
				  << "CharterNum";
	mDbLstKey << eChaterNum;
	mDbTableName = "ncua_gov";
}

void ncua_gov::doStart()
{
	meState = E_STATE_INIT;

	loadPage(BASEURL);
}

void ncua_gov::doWebLoadFinished()
{
	switch(meState)
	{
	case E_STATE_INIT:
		doSearch();
		break;

	case E_STATE_GET_MAIN_CATEGORY:
		getCharterNumber();
		break;

	case E_STATE_SUB1:
		startParseDetail();
		break;

	case E_STATE_GET_DETAIL_INFO:
		parseDetailInfo();
		break;

	default:
		qDebug() << "default";
		break;
	}
}

void ncua_gov::doSearch()
{
	QWebElement lElement;

	do
	{
		lElement = mpWebView->getElementById("ctl00_MainContent_ButtonFind");
		DEF_CHECK_BREAK(lElement, "Can not get ctl00_MainContent_ButtonFind");

		mpWebView->doClick(&lElement);
	} while (false);
}

void ncua_gov::getCharterNumber()
{
	QWebElement lElement, leTmp1;
	QWebElementCollection lCol;

	do
	{
		lElement = mpWebView->getElementById("ctl00_MainContent_GridViewResults");
		DEF_CHECK_BREAK(lElement, "Can not get ctl00_MainContent_GridViewResults");

		lCol = lElement.findAll("tr[class=\"RowType_1\"]");
		foreach (leTmp1, lCol)
			mlstCharterNum << leTmp1.firstChild().toPlainText();

		lCol = lElement.findAll("tr[class=\"RowType_2\"]");
		foreach (leTmp1, lCol)
			mlstCharterNum << leTmp1.firstChild().toPlainText();

		DEF_LOG_LIST(mlstCharterNum, "CharterNumber:");
		qDebug() << "Found: " << mlstCharterNum.count();

		if (mlstCharterNum.count() >= 50)
		{
			// start parse detail
			qDebug() << "Parse detail...";
			meState = E_STATE_SUB1;
			loadPage(BASEURL);
			break;
		}

		// goto next page
		lElement = mpWebView->getElementById("ctl00_MainContent_GridViewResults_ctl28_LinkButtonNext");
		DEF_CHECK_BREAK(lElement, "Can not get ctl00_MainContent_GridViewResults_ctl28_LinkButtonNext");

		if (lElement.attribute("href").indexOf("javascript") < 0)
		{
			// start parse detail
			showFinishMsg("Finsihed");
			break;
		}

		mpWebView->doClick(&lElement);
	} while (false);
}

void ncua_gov::startParseDetail()
{
	QString lstrCharterNum = "";
	QWebElement lElement;

	do
	{
		if (mlstCharterNum.count() == 0)
		{
			showFinishMsg("Finished all charter num");
			break;
		}

		setStatus(QString("Remain: %1").arg(mlstCharterNum.count()));
		lstrCharterNum = mlstCharterNum.takeFirst();

		lElement = mpWebView->getElementById("ctl00_MainContent_txtCharterNumber");
		DEF_CHECK_BREAK(lElement, "Can not get ctl00_MainContent_txtCharterNumber");

		lElement.setAttribute("value", lstrCharterNum);

		lElement = mpWebView->getElementById("ctl00_MainContent_ButtonFindQuick");
		DEF_CHECK_BREAK(lElement, "Can not get ctl00_MainContent_ButtonFindQuick");

		meState = E_STATE_GET_DETAIL_INFO;
		mpWebView->doClick(&lElement);
	} while (false);
}

void ncua_gov::parseDetailInfo()
{
	QWebElement lElement;
	QString lstrTmp;

	DataEntry ldata(mDbLstColName);

	do
	{
		lElement = mpWebView->findFirst("table[class=\"TableInner\"]");
		DEF_CHECK_BREAK(lElement, "Can not get TableInner");

		lstrTmp = lElement.toPlainText() + "\n";

		Utilities::extractAndUpdate(&ldata, mDbLstColName[eName], lstrTmp, "Manager/CEO:");
		Utilities::extractAndUpdate(&ldata, mDbLstColName[eAdd], lstrTmp, "Address:");
		Utilities::extractAndUpdate(&ldata, mDbLstColName[eCity], lstrTmp, "City, State Zip code:", ",");
		Utilities::extractAndUpdate(&ldata, mDbLstColName[eZip], lstrTmp, ldata.getProperty(mDbLstColName[eCity]) + ",");
		Utilities::extractAndUpdate(&ldata, mDbLstColName[eCountry], lstrTmp, "Country:");
		Utilities::extractAndUpdate(&ldata, mDbLstColName[eCounty], lstrTmp, "County:");
		Utilities::extractAndUpdate(&ldata, mDbLstColName[ePhone], lstrTmp, "Phone:");
		Utilities::extractAndUpdate(&ldata, mDbLstColName[eWeb], lstrTmp, "Site URL:");
		Utilities::extractAndUpdate(&ldata, mDbLstColName[eChaterNum], lstrTmp, "Charter Number:");

		mlstData << ldata;
	} while (false);

	if (mlstData.count() == 10)
	{
		mpDBLogger->logDataList(mlstData);
		DEF_LOG(QString("Save %1").arg(mlstData.count()));
		mlstData.clear();
	}

	// goto next charter number
	meState = E_STATE_SUB1;
	loadPage(BASEURL);
}

WebEngine_Main(ncua_gov)
