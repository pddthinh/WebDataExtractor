#include "Century21.h"

#define BASEURL "http://www.century21.com.au/c21/agents/agentsearch.cfm"
QString gstrMeetTheTeamFormat = "%1real-estate-agents/";

Century21::Century21(QWidget *apParent)
	: BaseWebEngine(apParent)
{
	setWindowTitle("Century21");
	setBaseURL(BASEURL);

	if(mpDBLogger != NULL)
		mpDBLogger->setLogPath("Century21.sql");

	mpLog->setFilePath("Century21.txt");
}

enum enuDatabase
{
	eName = 0,
	eTitle,
	ePhone, eFax, eAddress, eEmail,
	eDatabaseMax
};

void Century21::initDatabase()
{
	mDbLstColName << "Name"
				  << "Job Title"
				  << "Phone" << "Fax" << "Address" << "Email";
	mDbLstKey << eName << eEmail;
	mDbTableName = "Century21";
}

void Century21::doStart()
{
	meState = E_STATE_INIT;

	loadPage(BASEURL);
}

void Century21::doWebLoadFinished()
{
	switch(meState)
	{
	case E_STATE_INIT:
		getListState();
		break;

	case E_STATE_GET_MAIN_CATEGORY:
		getMainDataList();
		break;

	case E_STATE_GET_DETAIL_INFO:
		getData();
		break;

	default:
		qDebug() << "default";
		break;
	}
}

void Century21::getListState()
{
	mlstDataLink.clear();

	QWebElement	lele;
	QWebElementCollection	leCol;
	QString lstrLink;

	do
	{
		lele = mpWebView->findFirst(QLatin1String("div.c21-footer-2nd-level.clearfix"));
		DEF_CHECK_BREAK(lele, "Can not get [c21-footer-2nd-level clearfix]");

		leCol = lele.findAll("a");
		foreach(lele, leCol)
		{
			lstrLink = lele.attribute("href");
			if (lstrLink.startsWith("http://www.century21.com.au/sitemap/real-estate-agents/"))
				mlstCategoryLink << lstrLink;
		}

		qDebug() << "Save: " << mlstCategoryLink.count();
		DEF_LOG(QString("Save %1 state").arg(mlstCategoryLink.count()));

		if (gotoNextCategoryLink(E_STATE_GET_MAIN_CATEGORY) == false)
		{
			qDebug() << "End of main category or error occurred";
			DEF_LOG("End of main category or error occurred");

			if (gotoNextDataLink() == false)
				showFinishMsg();
		}
	} while(false);
}

void Century21::getMainDataList()
{
	QWebElement	lele;
	QWebElementCollection leCol;
	QString lstrTmp;

	do
	{
		lele = mpWebView->getElementById("leftList_SM");
		DEF_CHECK_BREAK(lele, "Can not get Left List");

		leCol = lele.findAll("a");
		foreach(lele, leCol)
		{
			// Save the [Meet The Team] link
			lstrTmp = lele.attribute("href");
			mlstDataLink << gstrMeetTheTeamFormat.arg(lstrTmp);
		}

		qDebug() << "DataLink count = " << mlstDataLink.count();
		DEF_LOG(QString("DataLink count = %1").arg(mlstDataLink.count()));
	} while(false);

	if (gotoNextCategoryLink(E_STATE_GET_MAIN_CATEGORY) == false)
	{
		qDebug() << "End of main category or error occurred";
		DEF_LOG("End of main category or error occurred");

		if (gotoNextDataLink() == false)
			showFinishMsg();
	}
}

void Century21::getData()
{
	QWebElement lele, leSub;
	QWebElementCollection	leCol;
	QString		lstrTmp;

	do
	{
		mData.clearData();

		qDebug() << mpWebView->url();

		// Get the group information
		lele = mpWebView->findFirst(QLatin1String("div.c21-right-panel-contact-box"));
		if (lele.isNull() == false)
		{
			// Name
			lstrTmp = "";
			leSub = lele.findFirst("h3[class=\"fn org\"]");
			if (leSub.isNull() == false)
				lstrTmp = leSub.toPlainText();
			mData.updateProperty(mDbLstColName[eName], lstrTmp);

			leCol = lele.findAll("span[class=\"tel\"]");

			// Phone
			lstrTmp = "";
			if (leCol.count() >= 1)
				lstrTmp = leCol[0].toPlainText() + "\n";
			Utilities::extractAndUpdate(&mData, mDbLstColName[ePhone],
										lstrTmp, ":");

			// Fax
			lstrTmp = "";
			if (leCol.count() >= 2)
				lstrTmp = leCol[1].toPlainText() + "\n";
			Utilities::extractAndUpdate(&mData, mDbLstColName[eFax],
										lstrTmp, ":");

			// Address
			lstrTmp = "";
			leSub = lele.findFirst("p[class=\"adr\"]");
			if (leSub.isNull() == false)
				lstrTmp = leSub.toPlainText();
			mData.updateProperty(mDbLstColName[eAddress], lstrTmp);

			// Email
			lstrTmp = "";
			leCol = lele.findAll("a.email");
			if (leCol.count() > 0)
				lstrTmp = leCol[0].attribute(QLatin1String("href"));
			mData.updateProperty(mDbLstColName[eEmail], lstrTmp);

			mlstData << mData;
			mData.clearData();
		}

		// Get the team information
		lele = mpWebView->findFirst(QLatin1String("div.c21-content-section3"));
		if (lele.isNull())
			break;

		leCol = lele.findAll(QLatin1String("a"));
		foreach (leSub, leCol)
		{
			lstrTmp = leSub.attribute(QLatin1String("href"));
			if (lstrTmp != QLatin1String("javascript:void(0);"))
				continue;

			mpWebView->doClick(&leSub);
		}

		sleep(2);

		leCol = lele.findAll(QLatin1String("div.clearfix"));
		foreach(leSub, leCol)
		{
			lstrTmp = QLatin1String("Name:") + leSub.toPlainText() + QLatin1String("\n");

			// Name
			Utilities::extractAndUpdate(&mData, mDbLstColName[eName],
										lstrTmp, "Name:");

			// Job Title
			Utilities::extractAndUpdate(&mData, mDbLstColName[eTitle],
										lstrTmp, mData.getProperty(mDbLstColName[eName]) + "\n");

			// Contact
			Utilities::extractAndUpdate(&mData, mDbLstColName[ePhone],
										lstrTmp, mData.getProperty(mDbLstColName[eTitle]) + "\n",
										QLatin1String("\n"), QLatin1String("\n"), true);

			// Email
			qDebug() << leSub.findFirst(QLatin1String("")).toInnerXml();
			lstrTmp = leSub.findFirst(QLatin1String("p.emailHideAfter")).toPlainText();
			Utilities::extractAndUpdate(&mData, mDbLstColName[ePhone],
										lstrTmp, "\n");

			mlstData << mData;
			mData.clearData();
		}

		if (mlstData.count() >= 10)
			logDataList();
	} while(false);

	if (gotoNextDataLink() == false)
		showFinishMsg();
}

WebEngine_Main(Century21)
