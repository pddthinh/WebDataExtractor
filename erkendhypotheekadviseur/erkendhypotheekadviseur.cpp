#include "erkendhypotheekadviseur.h"

#define BASEURL "http://www.erkendhypotheekadviseur.nl/zoek-een-erkend-hypotheekadviseur"

#define MAX_ZIP		17
QString gstrZip[] = {
	"8211", "8212", "8219", "8222", "8223",
	"8224", "8225", "8226", "8231", "8232",
	"8233", "8239", "8241", "8242", "8243",
	"8244", "8245",
	""
};
int miCurZip = -1;

erkendhypotheekadviseur::erkendhypotheekadviseur(QWidget *apParent)
	: BaseWebEngine(apParent)
{
	setWindowTitle("erkendhypotheekadviseur");
	setBaseURL(BASEURL);

	if(mpDBLogger != NULL)
		mpDBLogger->setLogPath("erkendhypotheekadviseur.sql");

	mpLog->setFilePath("erkendhypotheekadviseur.txt");
}

enum enuDatabase
{
	eName = 0,
	eCompany,
	eAddress, eZipCode,
	ePhone,
	eWebsite,
	eDatabaseMax
};

void erkendhypotheekadviseur::initDatabase()
{
	mDbLstColName << "Name"
				  << "Company Name"
				  << "Address" << "ZipCode"
				  << "Phone"
				  << "Website";
	mDbLstKey << eName << eCompany << eZipCode;
	mDbTableName = "erkendhypotheekadviseur";
}

void erkendhypotheekadviseur::doStart()
{
	meState = E_STATE_INIT;

	loadPage(BASEURL);
}

void erkendhypotheekadviseur::doStop()
{}

void erkendhypotheekadviseur::doWebLoadFinished()
{
	switch(meState)
	{
	case E_STATE_INIT:
		inputZipCode();
		break;

	case E_STATE_GET_MAIN_CATEGORY:
		getMainCategory();
		break;

	case E_STATE_GET_SUB_CATEGORY:
		qDebug() << "E_STATE_GET_SUB_CATEGORY";
		break;

	case E_STATE_GET_DETAIL_INFO:
		qDebug() << "E_STATE_GET_DETAIL_INFO";
		break;

	default:
		qDebug() << "default";
		break;
	}
}

bool erkendhypotheekadviseur::gotoNextZipCode()
{
	bool lblRet = false;

	do
	{
		if(miCurZip >= MAX_ZIP)
			break;

		miCurZip ++;
		lblRet = true;
	}while(false);

	return lblRet;
}

void erkendhypotheekadviseur::inputZipCode()
{
	QWebElement		lelement;

	do
	{
		if(gotoNextZipCode() == false)
		{
			DEF_LOG("FINISHED.");
			meState = E_STATE_MAX;
			showFinishMsg();
			break;
		}

		DEF_LOG(QString("->>>>>>>>>>>>>>Current zip: %1").arg(gstrZip[miCurZip]));
		qDebug() << QString("->>>>>>>>>>>>>>Current zip: %1").arg(gstrZip[miCurZip]);

		lelement = mpWebView->document().findFirst("input[class=\"inputtext\"][name=\"keyword\"]");
		DEF_CHECK_BREAK(lelement, "Can not find Keyword element");
		lelement.setAttribute("value", gstrZip[miCurZip]);

		meState = E_STATE_GET_MAIN_CATEGORY;
		miCurPage = 0;
		mpWebView->doClick("input[class=\"submit\"][type=\"submit\"]");
	}while(false);
}

void erkendhypotheekadviseur::getMainCategory()
{
	QWebElement		lelement;
	QWebElementCollection	leCol;
	int liCnt, liTotal;

	DataEntry		ldata(mDbLstColName);
	QString			lstrTmp;

	do
	{
		lelement = mpWebView->document().findFirst("table[class=\"zoekresultaten\"]");
		if(lelement.isNull())
		{
			// Can not find any result
			qDebug() << "Can not find any result";
			DEF_LOG("Can not find any result()");

			// Goto next zip
			doStart();
			break;
		}
		leCol = lelement.findAll("tr");

		qDebug() << "Total tr element: " << leCol.count();
		if(leCol.count() % 3 != 0)
		{
			qDebug() << "Invalid size";
			DEF_LOG("Invalid size");

			if(gotoNextResultLink() == false)
			{
				DEF_LOG("FINISHED!");
				showFinishMsg();
				break;
			}
		}

		liTotal = leCol.count();
		for(liCnt = 0; liCnt < liTotal; liCnt += 3)
		{
			ldata.clearData();

			// Tr[n0]
			lstrTmp = "";
			lelement = leCol[liCnt].findFirst("a[class=\"website-link\"]");
			if(lelement.isNull() == false)
				lstrTmp = lelement.attribute("href");
			ldata.updateProperty(mDbLstColName[eWebsite], lstrTmp);

			lstrTmp = "";
			lelement = leCol[liCnt].findFirst("span[class=\"telefoonnr\"]");
			if(lelement.isNull() == false)
				lstrTmp = lelement.toPlainText();
			ldata.updateProperty(mDbLstColName[ePhone], lstrTmp);

			// Tr[n0 + 1]
			lstrTmp = "";
			lelement = leCol[liCnt + 1].findFirst("a[class=\"no-link\"]");
			if(lelement.isNull() == false)
				lstrTmp = lelement.toPlainText();
			ldata.updateProperty(mDbLstColName[eName], lstrTmp);

			lstrTmp = "";
			lelement = leCol[liCnt + 1].findAll("td")[1];
			if(lelement.isNull() == false)
				lstrTmp = lelement.toPlainText();
			ldata.updateProperty(mDbLstColName[eAddress], lstrTmp);

			// Tr[n0 + 2]
			lstrTmp = "";
			lelement = leCol[liCnt + 2].findAll("td")[0];
			if(lelement.isNull() == false)
				lstrTmp = lelement.toPlainText();
			ldata.updateProperty(mDbLstColName[eCompany], lstrTmp);

			lstrTmp = "";
			lelement = leCol[liCnt + 2].findAll("td")[1];
			if(lelement.isNull() == false)
				lstrTmp = lelement.toPlainText();
			ldata.updateProperty(mDbLstColName[eZipCode], lstrTmp);

			mlstData << ldata;
			if(mlstData.count() == 20)
			{
				mpDBLogger->logDataList(mlstData);
				DEF_LOG(QString("Save %1 dataentry").arg(mlstData.count()));

				mlstData.clear();
			}
		}

		if(mlstData.count() > 0 )
		{
			mpDBLogger->logDataList(mlstData);
			DEF_LOG(QString("Save %1 dataentry").arg(mlstData.count()));

			mlstData.clear();
		}

		if(gotoNextResultLink() == false)
		{
			// Goto next zip
			qDebug() << "Goto next zip code";
			DEF_LOG("Goto next zip code");
			doStart();
			break;
		}
	}while(false);
}

bool erkendhypotheekadviseur::gotoNextResultLink()
{
	bool lblRet = false;
	QWebElement		lelement;
	QWebElementCollection	leCol;
	int	liCnt;

	do
	{
//		miCurPage++;
		lelement = mpWebView->document().findFirst("div[class=\"pages\"]");
		DEF_CHECK_BREAK(lelement, "Can not find Pages element");
		leCol = lelement.findAll("a");
		liCnt = 0;
		while(liCnt < leCol.count())
		{
			if(leCol[liCnt].attribute("class") == "active_page")
				break;	// found current page

			liCnt++;
		}

		if(liCnt + 1 >= leCol.count())
			break;

		lelement = leCol[liCnt + 1];
		lblRet = true;
		qDebug() << "null: " << lelement.isNull();
		mpWebView->doClick(&lelement);
	}while(false);

	return lblRet;
}

WebEngine_Main(erkendhypotheekadviseur)
