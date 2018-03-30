#include "ciga_co_uk.h"

#define BASEURL "http://www.ciga.co.uk/installers.html"

ciga_co_uk::ciga_co_uk(QWidget *apParent)
	: BaseWebEngine(apParent)
{
	setWindowTitle("ciga_co_uk");
	setBaseURL(BASEURL);

	if(mpDBLogger != NULL)
		mpDBLogger->setLogPath("ciga_co_uk.sql");

	mpLog->setFilePath("ciga_co_uk.txt");
	///////////////////////////////////////////////////////////
}

enum enuDatabase
{
	eName = 0,
	eAdd, eTown, eCounty, eCountry, ePostcode, eFullAdd,
	eTel, eFax, eWeb, eEmail,
	eDatabaseMax
};

void ciga_co_uk::initDatabase()
{
	mDbLstColName << "Name"
				  << "Address" << "Town" << "County" << "Country" << "Postcode" << "FullAddress"
				  << "Tel" << "Fax" << "Website" << "Email";
	mDbLstKey << eName << ePostcode << eTel;
	mDbTableName = "ciga_co_uk";
}

void ciga_co_uk::doStart()
{
	meState = E_STATE_INIT;

	loadPage(BASEURL);
}

void ciga_co_uk::doWebLoadFinished()
{
	switch(meState)
	{
	case E_STATE_INIT:
		getRegionLink();
		break;

	case E_STATE_GET_DETAIL_INFO:
		getDetailData();
		break;

	default:
		qDebug() << "default";
		break;
	}
}

void ciga_co_uk::getRegionLink()
{
	QWebElement lElement;
	QWebElementCollection lCol;
	QString lstrTmp;

	do
	{
		lElement = mpWebView->findFirst("td[align=\"left\"][valign=\"top\"][class=\"navContainer\"]");
		DEF_CHECK_BREAK(lElement, "Can not find navigator element");

		lCol = lElement.findAll("a");
		foreach (lElement, lCol)
		{
			lstrTmp = mpWebView->getHrefURL(&lElement);
			qDebug() << "link: " << lstrTmp;
			if (lstrTmp.indexOf("/installers/") > 0)
				mlstDataLink << lstrTmp;
		}

		qDebug() << "Found: " << mlstDataLink.count();
	} while (false);

	if (gotoNextDataLink() == false)
		showFinishMsg("Finished!");
}

void ciga_co_uk::getDetailData()
{
	QWebElement lElement;
	QString lstrTmp, lstrData, lstrTmp1;
	QStringList llstData;
	int i, count;

	DataEntry ldata(mDbLstColName);
	TAG_ADDRESS ltAdd;

	do
	{
		lElement = mpWebView->findFirst("td[class=\"mT\"][valign=\"top\"]");
		DEF_CHECK_BREAK(lElement, "Can not find td data element");

		lstrTmp = lElement.toPlainText().trimmed();
		lstrTmp.replace("\240", " ");
		lstrTmp.replace("  ", " ");
		llstData = lstrTmp.split("\n");

		lstrData = "Name:";
		foreach(lstrTmp, llstData)
		{
			lstrTmp = lstrTmp.trimmed();
			if (lstrTmp.isEmpty())
				continue;

			if (lstrTmp.startsWith("Email") == false)
			{
				lstrData += lstrTmp + "\n";
				continue;
			}

			lstrData += lstrTmp + "\n";

			// start parsing
			lstrData.replace("Tel", "\nTel");
			lstrData.replace("Fax", "\nFax");
			lstrData.replace("Email", "\nEmail");
			lstrData.replace("Web", "\nWeb");

			lstrTmp = Utilities::extractValue(lstrData, "Name:", "\n");

			// separate the postcode
			i = lstrTmp.length();
			count = 0;
			while ((i = lstrTmp.lastIndexOf(" ", (-1) * (lstrTmp.length() - i + 1))) > 0)
			{
				count++;
				if (count == 2)
					break;
			}

			lstrTmp.insert(i, ",");
			lstrTmp.replace(",,", ",");

			lstrTmp1 = "Name:" + lstrTmp;
			lstrTmp1.replace(",", "\n");
			lstrTmp1.replace(".", "\n");

			// Name
			Utilities::extractAndUpdate(&ldata, mDbLstColName[eName], lstrTmp1, "Name:");
			lstrTmp.replace(ldata.getProperty(mDbLstColName[eName]), "");
			if (lstrTmp.startsWith(","))
				lstrTmp = lstrTmp.right(lstrTmp.length() - 2);

			// Address
			ltAdd = Utilities::parseUK_Add(lstrTmp, false);
			ldata.updateProperty(mDbLstColName[eFullAdd], ltAdd.strFullAddress);
			ldata.updateProperty(mDbLstColName[eAdd], ltAdd.strAddress);
			ldata.updateProperty(mDbLstColName[eTown], ltAdd.strTown);
			ldata.updateProperty(mDbLstColName[eCounty], ltAdd.strCounty);
			ldata.updateProperty(mDbLstColName[eCountry], ltAdd.strCountry);
			ldata.updateProperty(mDbLstColName[ePostcode], ltAdd.strPostcode);

			Utilities::extractAndUpdate(&ldata, mDbLstColName[eTel], lstrData, "Tel:");
			Utilities::extractAndUpdate(&ldata, mDbLstColName[eFax], lstrData, "Fax:");
			Utilities::extractAndUpdate(&ldata, mDbLstColName[eEmail], lstrData, "Email:");
			Utilities::extractAndUpdate(&ldata, mDbLstColName[eWeb], lstrData, "Web:");

			lstrData = "Name:";

			mlstData << ldata;
			ldata.clearData();
		}

		mpDBLogger->logDataList(mlstData);
		DEF_LOG(QString("Save: %1").arg(mlstData.count()));
		mlstData.clear();
	} while (false);

	if (gotoNextDataLink() == false)
		showFinishMsg("Finished!");
}

WebEngine_Main(ciga_co_uk)
