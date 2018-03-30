#include "nia_org_uk.h"
#include <QWebFrame>

#define BASEURL "http://www.nationalinsulationassociation.org.uk/householder/index.php?page=directory"

nia_org_uk::nia_org_uk(QWidget *apParent)
	: BaseWebEngine(apParent)
{
	setWindowTitle("nia_org_uk");
	setBaseURL(BASEURL);

	if(mpDBLogger != NULL)
		mpDBLogger->setLogPath("nia_org_uk.sql");

	mpLog->setFilePath("nia_org_uk.txt");
	///////////////////////////////////////////////////////////
}

enum enuDatabase
{
	eName = 0,
	eAdd, eTown, eCounty, eCountry, ePostcode, eFullAdd,
	eTel, eFax, eWeb, eEmail,
	eOther,
	eDatabaseMax
};

void nia_org_uk::initDatabase()
{
	mDbLstColName << "Name"
				  << "Address" << "Town" << "County" << "Country" << "Postcode" << "FullAddress"
				  << "Tel" << "Fax" << "Website" << "Email"
				  << "Other";
	mDbLstKey << eName << eTel << ePostcode;
	mDbTableName = "nia_org_uk";
}

void nia_org_uk::doStart()
{
	meState = E_STATE_INIT;

	loadPage(BASEURL);
}

void nia_org_uk::doWebLoadFinished()
{
	switch(meState)
	{
	case E_STATE_INIT:
		getDataLink();
		break;

	case E_STATE_GET_DETAIL_INFO:
		getDetailData();
		break;

	default:
		qDebug() << "default";
		break;
	}
}

void nia_org_uk::getDataLink()
{
	QWebElement lElement;
	QWebElementCollection lCol;
	QString lstrTmp;

	do
	{
		QList<QWebFrame*> lpFrames = mpWebView->page()->mainFrame()->childFrames();

		if (lpFrames.count() == 0)
		{
			DEF_LOG("Can not find data frame");
			break;
		}

		lCol = lpFrames.at(0)->findAllElements("a[class=\"purpleLink\"]");
		foreach (lElement, lCol)
		{
			lstrTmp = mpWebView->getHrefURL(&lElement);

			if (lstrTmp.indexOf("&member=") > 0)
				mlstDataLink << lstrTmp;
		}
	} while (false);

	if (gotoNextDataLink() == false)
		showFinishMsg("Finished!");
}

void nia_org_uk::getDetailData()
{
	QWebElement lElement, leTmp, leEmail;
	QString lstrTmp;

	DataEntry ldata(mDbLstColName);
	QList<QWebFrame*> lFrames;
	QWebFrame *lpFrame;

	TAG_ADDRESS ltAdd;

	do
	{
		lFrames = mpWebView->page()->mainFrame()->childFrames();
		if (lFrames.count() == 0)
		{
			DEF_LOG("Can not find data frame");
			break;
		}

		lpFrame = lFrames.at(0);
		lElement = lpFrame->findFirstElement("div[style=\"width:320px;\"]");
		DEF_CHECK_BREAK(lElement, "Can not get div data element");

		leTmp = lElement.findFirst("font");
		lstrTmp = "";
		if (leTmp.isNull() == false)
			lstrTmp = leTmp.toPlainText();
		ldata.updateProperty(mDbLstColName[eName], lstrTmp);

		leTmp = leTmp.nextSibling();
		DEF_CHECK_BREAK(leTmp, "Can not get contact element");

		leEmail = leTmp.findFirst("a");
		lstrTmp = leEmail.attribute("href");
		Utilities::extractAndUpdate(&ldata, mDbLstColName[eEmail], lstrTmp, "email=", "&company=");

		lstrTmp = leTmp.toPlainText() + "\n";
		Utilities::extractAndUpdate(&ldata, mDbLstColName[eTel], lstrTmp, "Tel:");
		Utilities::extractAndUpdate(&ldata, mDbLstColName[eFax], lstrTmp, "Fax:");
		Utilities::extractAndUpdate(&ldata, mDbLstColName[eWeb], lstrTmp, "Web:");

		// Get Address
		lstrTmp.replace("E-mail: Click here", "");
		lstrTmp.replace("Tel: " + ldata.getProperty(mDbLstColName[eTel]), "");
		lstrTmp.replace("Fax: " + ldata.getProperty(mDbLstColName[eFax]), "");
		lstrTmp.replace("Web: " + ldata.getProperty(mDbLstColName[eWeb]), "");

		while (lstrTmp.endsWith("\n"))
			lstrTmp.chop(1);

		ltAdd = Utilities::parseUK_Add(lstrTmp, false);
		ldata.updateProperty(mDbLstColName[eFullAdd], ltAdd.strFullAddress);
		ldata.updateProperty(mDbLstColName[eAdd], ltAdd.strAddress);
		ldata.updateProperty(mDbLstColName[eTown], ltAdd.strTown);
		ldata.updateProperty(mDbLstColName[eCounty], ltAdd.strCounty);
		ldata.updateProperty(mDbLstColName[eCountry], ltAdd.strCountry);
		ldata.updateProperty(mDbLstColName[ePostcode], ltAdd.strPostcode);

		// Get other info
		leTmp = lElement.parent().parent().nextSibling();
		lstrTmp = "";
		if (leTmp.isNull() == false && leTmp.tagName().toLower() == "tr")
			lstrTmp = leTmp.toPlainText();
		ldata.updateProperty(mDbLstColName[eOther], lstrTmp);

		mlstData << ldata;
	} while (false);

	if (mlstData.count() == 20)
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
		}

		showFinishMsg("Finished!");
	}
}

WebEngine_Main(nia_org_uk)
