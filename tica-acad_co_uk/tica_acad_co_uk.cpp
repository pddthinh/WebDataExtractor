#include "tica_acad_co_uk.h"

#define BASEURL		"http://www.tica-acad.co.uk/members?sort=tica"
#define BASE_SITE	"http://www.tica-acad.co.uk"

#define TIMER_WAIT      4000

tica_acad_co_uk::tica_acad_co_uk(QWidget *apParent)
	: BaseWebEngine(apParent)
{
	setWindowTitle("tica_acad_co_uk");
	setBaseURL(BASEURL);

	if(mpDBLogger != NULL)
		mpDBLogger->setLogPath("tica_acad_co_uk.sql");

	mpLog->setFilePath("tica_acad_co_uk.txt");
	///////////////////////////////////////////////////////////

	mpTimer = new QTimer();
	mpTimer->setInterval(TIMER_WAIT);
	mpTimer->setSingleShot(true);
	connect(mpTimer, SIGNAL(timeout()), this, SLOT(sltWaitTimeout()));
}

enum enuDatabase
{
	eComName = 0,
	eContact,
	eAdd, eTown, eCounty, eCountry, ePostcode, eFullAdd,
	eTel, eFax, eWeb, eEmail,
	eMemInfo,
	eDes,
	eDatabaseMax
};

void tica_acad_co_uk::initDatabase()
{
	mDbLstColName << "Name"
				  << "Contact"
				  << "Address" << "Town" << "County" << "Country" << "Postcode" << "FullAddress"
				  << "Tel" << "Fax" << "Website" << "Email"
				  << "MemInfo"
				  << "Description";
	mDbLstKey << eComName << eTel << ePostcode;
	mDbTableName = "tica_acad_co_uk";
}

void tica_acad_co_uk::doStart()
{
	meState = E_STATE_INIT;

	loadPage(BASEURL);
}

void tica_acad_co_uk::doWebLoadFinished()
{
	switch(meState)
	{
	case E_STATE_INIT:
		qDebug() << "E_STATE_INIT";
		mpTimer->start();
		break;

	case E_STATE_GET_DETAIL_INFO:
		getDetailData();
		break;

	default:
		qDebug() << "default";
		break;
	}
}

void tica_acad_co_uk::sltWaitTimeout()
{
	qDebug() << "Timeout";
	mpTimer->stop();

	getMainList();
}

void tica_acad_co_uk::getMainList()
{
	QWebElement lElement, leTmp;
	QWebElementCollection lCol;
	QString lstrLink;

	do
	{
		lCol = mpWebView->findAll("div[class=\"member-search-result\"]");

		mlstDataLink.clear();
		foreach (leTmp, lCol)
		{
			lElement = leTmp.findFirst("a");
			if (lElement.isNull() == true)
				continue;

			lstrLink = lElement.attribute("title");
			if (lstrLink.isEmpty())
				continue;

			lstrLink = lElement.attribute("href");
			if (lstrLink.isEmpty())
				continue;

			lstrLink = BASE_SITE + lstrLink;
			mlstDataLink << lstrLink;
		}

		qDebug() << "total items: " << mlstDataLink.count();
		DEF_LOG(QString("Total items: %1").arg(mlstDataLink.count()));

		if (gotoNextDataLink() == false)
			showFinishMsg("Finished!");
	} while (false);
}

void tica_acad_co_uk::getDetailData()
{
	QString lstrTmp;
	QWebElement lElement, lMemDetail, leTmp1;
	QWebElementCollection lCol;

	DataEntry ldata(mDbLstColName);
	TAG_ADDRESS ltAdd;

	do
	{
		lMemDetail = mpWebView->getElementById("member-detail");
		DEF_CHECK_BREAK(lMemDetail, "Can not find member detail");

		// Name
		lstrTmp = "";
		lElement = lMemDetail.findFirst("h1");
		if (lElement.isNull() == false)
			lstrTmp = lElement.toPlainText();
		ldata.updateProperty(mDbLstColName[eComName], lstrTmp);

		// Contact + Address
		{
			lElement = lMemDetail.findFirst("div[class=\"col1\"]");
			DEF_CHECK_BREAK(lElement, "Can not find col1");
			lstrTmp = "Name: " + lElement.toPlainText() + "\nEnd";
			Utilities::extractAndUpdate(&ldata, mDbLstColName[eContact], lstrTmp, "Name:");

			lstrTmp = lElement.findFirst("p").toPlainText();
			lstrTmp = lstrTmp.replace("\n", ",");

			ltAdd = Utilities::parseUK_Add(lstrTmp);
			ldata.updateProperty(mDbLstColName[eFullAdd], ltAdd.strFullAddress);
			ldata.updateProperty(mDbLstColName[eAdd], ltAdd.strAddress);
			ldata.updateProperty(mDbLstColName[eTown], ltAdd.strTown);
			ldata.updateProperty(mDbLstColName[eCounty], ltAdd.strCounty);
			ldata.updateProperty(mDbLstColName[eCountry], ltAdd.strCountry);
			ldata.updateProperty(mDbLstColName[ePostcode], ltAdd.strPostcode);
		}

		// Contact info
		{
			lElement = lMemDetail.findFirst("div[class=\"col2\"]");
			DEF_CHECK_BREAK(lElement, "Can not find col2");

			// Email
			lstrTmp = "";
			leTmp1 = lElement.findFirst("img[alt=\"Email:\"]").nextSibling();
			if (leTmp1.tagName().toLower() == "a")
				lstrTmp = leTmp1.attribute("href").replace("mailto:", "");
			ldata.updateProperty(mDbLstColName[eEmail], lstrTmp);

			// Website
			lstrTmp = "";
			leTmp1 = lElement.findFirst("img[alt=\"Website:\"]").nextSibling();
			if (leTmp1.tagName().toLower() == "a")
				lstrTmp = leTmp1.attribute("href");
			ldata.updateProperty(mDbLstColName[eWeb], lstrTmp);

			// MemInfo
			lstrTmp = "";
			lCol = lElement.findAll("p[class=\"member_info\"]");
			foreach (leTmp1, lCol)
			{
				if (leTmp1.toPlainText().isEmpty() == false)
					lstrTmp += leTmp1.toPlainText() + ", ";
				leTmp1.removeFromDocument();
			}
			if (lstrTmp.endsWith(", "))
				lstrTmp.chop(2);
			ldata.updateProperty(mDbLstColName[eMemInfo], lstrTmp);

			lstrTmp = lElement.toPlainText().replace(ldata.getProperty(mDbLstColName[eWeb]), "");
			lstrTmp = lstrTmp.replace(ldata.getProperty(mDbLstColName[eEmail]), "");
			if (lstrTmp.contains("No website"))
				lstrTmp = lstrTmp.replace("No website", "");

			// Tel
			lstrTmp = "Phone: " + lstrTmp + "\n";
			Utilities::extractAndUpdate(&ldata, mDbLstColName[eTel], lstrTmp, "Phone:");

			// Fax
			Utilities::extractAndUpdate(&ldata, mDbLstColName[eFax], lstrTmp, ldata.getProperty(mDbLstColName[eTel]) + "\n");
		}

		// Description
		{
			lCol = lMemDetail.findAll("div[class=\"member-box\"]");
			if (lCol.count() != 2)
				break;

			lstrTmp = lCol.at(1).toPlainText();
			ldata.updateProperty(mDbLstColName[eDes], lstrTmp);
		}
	} while (false);

	//goto next data entry
	mlstData << ldata;

	if (mlstData.count() == 10)
	{
		mpDBLogger->logDataList(mlstData);
		DEF_LOG(QString("Saved data: %1").arg(mlstData.count()));

		mlstData.clear();
	}

	if (gotoNextDataLink() == false)
		showFinishMsg("Finished");
}

WebEngine_Main(tica_acad_co_uk)
