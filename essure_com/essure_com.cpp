#include "essure_com.h"

#define BASEURL "http://www.essure.com/find-a-doctor-landing/find-a-doctor"

essure_com::essure_com(QWidget *apParent)
	: BaseWebEngine(apParent)
{
	setWindowTitle("essure_com");
	setBaseURL(BASEURL);

	if(mpDBLogger != NULL)
		mpDBLogger->setLogPath("essure_com.sql");

	mpLog->setFilePath("essure_com.txt");
///////////////////////////////////////////////////////////
	setWebLoadTimeout(15000);
}

enum enuDatabase
{
	eName = 0,
	eInfo, eAddress, eTel,
	eDatabaseMax
};

void essure_com::initDatabase()
{
	mDbLstColName << "Name"
				  << "Info" << "Address" << "Tel";
	mDbLstKey << eName << eTel;
	mDbTableName = "essure_com";
}

void essure_com::doStart()
{
	meState = E_STATE_INIT;
	miCurState = 2;

	loadPage(BASEURL);
}

void essure_com::doWebLoadFinished()
{
	switch(meState)
	{
	case E_STATE_INIT:
		initSearch();
		break;

	case E_STATE_GET_MAIN_CATEGORY:
		selectAllResult();
		break;

	case E_STATE_GET_DETAIL_INFO:
		getDetailData();
		break;

	default:
		qDebug() << "default";
		break;
	}
}

void essure_com::initSearch()
{
	QWebElement lElement, leTmp;
	QWebElementCollection lCol;

	do
	{
		// Update distance
		lElement = mpWebView->getElementById("C003_Distance");
		DEF_CHECK_BREAK(lElement, "Can not find C003_Distance");
		lCol = lElement.findAll("option");
		foreach (leTmp, lCol)
			leTmp.removeAttribute("selected");
		lCol[lCol.count() - 1].setAttribute("selected", "selected");

		// Update state
		lElement = mpWebView->getElementById("C003_StateDPL");
		DEF_CHECK_BREAK(lElement, "Can not find C003_StateDPL");
		lCol = lElement.findAll("option");
		foreach (leTmp, lCol)
			leTmp.removeAttribute("selected");
		if (miCurState >= lCol.count())
		{
			showFinishMsg("Finished");
			break;
		}
		lCol[miCurState].setAttribute("selected", "selected");
		lElement.evaluateJavaScript("SetStateHiddenField()");

		lElement = mpWebView->getElementById("C003_hd_State");
		DEF_CHECK_BREAK(lElement, "Can not find C003_hd_State");
		lElement.setAttribute("value", lCol[miCurState].toPlainText());
		qDebug() << lCol[miCurState].toPlainText();
		setStatus(QString("Current state index: %1").arg(miCurState));

		// do click
		lElement = mpWebView->getElementById("C003_Search");
		DEF_CHECK_BREAK(lElement, "Can not find C003_Search");

		meState = E_STATE_GET_MAIN_CATEGORY;
		mpWebView->doClick(&lElement);
	} while (false);

	qDebug() << "initSearch";
}

void essure_com::selectAllResult()
{
	QWebElement lElement, leTmp;

	do
	{
		leTmp = mpWebView->getElementById("C003_allNext");
		qDebug() << leTmp.tagName();
		qDebug() << leTmp.toPlainText();

		lElement = mpWebView->getElementById("aspnetForm");
		DEF_CHECK_BREAK(lElement, "Can not find aspnetForm");

		leTmp = mpWebView->getElementById("__EVENTTARGET");
		if (leTmp.isNull())
			lElement.appendInside("<input type=\"hidden\" name=\"__EVENTTARGET\" id=\"__EVENTTARGET\" value=\"\">");
		leTmp = mpWebView->getElementById("__EVENTTARGET");
		DEF_CHECK_BREAK(leTmp, "Can not find __EVENTTARGET");
		leTmp.setAttribute("value", "C003$lnkViewAll");

		leTmp = mpWebView->getElementById("__EVENTARGUMENT");
		if (leTmp.isNull())
			lElement.appendInside("<input type=\"hidden\" name=\"__EVENTARGUMENT\" id=\"__EVENTARGUMENT\" value=\"\">");
		leTmp = mpWebView->getElementById("__EVENTARGUMENT");
		DEF_CHECK_BREAK(leTmp, "Can not find __EVENTARGUMENT");
		leTmp.setAttribute("value", "");

		setWebLoadTimeout(20000);
		meState = E_STATE_GET_DETAIL_INFO;
		lElement.evaluateJavaScript("this.submit();");
	} while (false);
	qDebug() << "Finished selectAllResult";
}

void essure_com::getDetailData()
{
	QWebElement lElement, leTmp1;
	QWebElementCollection lCol;

	QString lstrTmp, lstrTmp2;

	DataEntry ldata(mDbLstColName);

	do
	{
		lCol = mpWebView->findAll("div[class=\"foundPhysiciansModule\"]");
		qDebug() << "Found items: " << lCol.count();
		foreach (lElement, lCol)
		{
			ldata.clearData();

			leTmp1 = lElement.findFirst("span[class=\"bodyBoldBlue\"]");
			lstrTmp = "";
			if (leTmp1.isNull() == false)
				lstrTmp = leTmp1.toPlainText();
			ldata.updateProperty(mDbLstColName[eName], lstrTmp);

			leTmp1 = lElement.findFirst("span[class=\"bodycopy\"]");
			lstrTmp = "Tel:" + leTmp1.toPlainText() + "\n";

			Utilities::extractAndUpdate(&ldata, mDbLstColName[eTel], lstrTmp, "Tel:");
			Utilities::extractAndUpdate(&ldata, mDbLstColName[eInfo], lstrTmp, ldata.getProperty(mDbLstColName[eTel]) + "\n");

			lstrTmp.replace("Tel:" + ldata.getProperty(mDbLstColName[eTel]) + "\n", "");
			lstrTmp.replace(ldata.getProperty(mDbLstColName[eInfo]) + "\n", "");
			lstrTmp2 = Utilities::extractValue(lstrTmp, "distance ", "\n");
			lstrTmp.replace("\ndistance " + lstrTmp2 + "\n", "");
			lstrTmp.replace("\n", ", ");

			ldata.updateProperty(mDbLstColName[eAddress], lstrTmp);

			mlstData << ldata;
		}

		if (mlstData.count() > 0)
		{
			mpDBLogger->logDataList(mlstData);
			DEF_LOG(QString("State[%1]\tSave: %2").arg(miCurState).arg(mlstData.count()));
			mlstData.clear();
		}
	} while (false);

	// goto next state
	miCurState++;
	setWebLoadTimeout(15000);
	meState = E_STATE_INIT;
	loadPage(BASEURL);
//	mpWebView->reload();
}

WebEngine_Main(essure_com)
