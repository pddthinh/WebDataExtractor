#include "inca-ltd_org_uk.h"

#define BASEURL "http://www.inca-ltd.org.uk/find-a-member.asp"

#define BASE_TIME_WAIT		5000

inca_ltd_org_uk::inca_ltd_org_uk(QWidget *apParent)
	: BaseWebEngine(apParent)
{
	setWindowTitle("inca_ltd_org_uk");
	setBaseURL(BASEURL);

	if(mpDBLogger != NULL)
		mpDBLogger->setLogPath("inca_ltd_org_uk.sql");

	mpLog->setFilePath("inca_ltd_org_uk.txt");
	///////////////////////////////////////////////////////////

	mpTimer = new QTimer();
	mpTimer->setInterval(BASE_TIME_WAIT);
	mpTimer->setSingleShot(true);

	connect(mpTimer, SIGNAL(timeout()), this, SLOT(sltWaitTimeout()));

	miMaxProject = -1;
	miMaxRegion = -1;
}

enum enuDatabase
{
	eName = 0,
	eWeb,
	eTown, ePostcode,
	eTel,
	eEmail,
	eDatabaseMax
};

void inca_ltd_org_uk::initDatabase()
{
	mDbLstColName << "Name"
				  << "Website"
				  << "Town-County" << "Postcode"
				  << "Tel" << "Email";
	mDbLstKey << eName << ePostcode << eEmail;
	mDbTableName = "inca_ltd_org_uk";
}

void inca_ltd_org_uk::doStart()
{
	meState = E_STATE_INIT;

	// skip the first (All types / All regions)
	miCurProject = 1;
	miCurRegion = 1;

	loadPage(BASEURL);
}

void inca_ltd_org_uk::doWebLoadFinished()
{
	switch(meState)
	{
	case E_STATE_INIT:
		initSearching();
		break;

	case E_STATE_GET_DETAIL_INFO:
		qDebug() << "E_STATE_GET_DETAIL_INFO";
		parseDetail();
		break;

	default:
		qDebug() << "default";
		break;
	}
}

void inca_ltd_org_uk::sltWaitTimeout()
{
	mpTimer->stop();
	doWebLoadFinished();
}

bool inca_ltd_org_uk::initSearching()
{
	bool lblRet = false;
	QWebElement lElement;
	QWebElementCollection   lCol;
	int i;

	do
	{
		lElement = mpWebView->getElementById("project");
		lCol = lElement.findAll("option");
		if (lCol.count() == 0)
		{
			DEF_LOG("Can not get the project selector");
			break;
		}

		miMaxProject = lCol.count();

		// check the region index
		if (miMaxRegion > 0 && miCurRegion >= miMaxRegion)
		{
			miCurProject ++;
			miCurRegion = 1;

			if (miCurProject >= miMaxProject)
			{
				showFinishMsg("Finished");
				break;
			}
		}

		for (i = 0; i < miMaxProject; i++)
		{
			if (i == miCurProject)
			{
				lCol.at(i).setAttribute("selected", "selected");
				break;
			}

			lCol.at(i).removeAttribute("selected");
		}

		// Update region
		lElement = mpWebView->getElementById("region");
		lCol = lElement.findAll("option");
		if (lCol.count() == 0)
		{
			DEF_LOG("Can not get the region selector");
			break;
		}

		miMaxRegion = lCol.count();

		for (i = 0; i < miMaxRegion; i++)
		{
			if (i == miCurRegion)
			{
				lCol.at(i).setAttribute("selected", "selected");
				break;
			}

			lCol.at(i).removeAttribute("selected");
		}

		// start search
		meState = E_STATE_GET_DETAIL_INFO;
		mpTimer->start();
		mpWebView->doClick("*#start-search");

		lblRet = true;
	} while (false);

	return lblRet;
}

void inca_ltd_org_uk::parseDetail()
{
	QWebElement lElement;
	QWebElementCollection lCol;

	int i, size;

	DataEntry ldata(mDbLstColName);
	QString lstrTmp;

	do
	{
		lElement = mpWebView->getElementById("contractor-tbl");
		DEF_CHECK_BREAK(lElement, "Can not find contractor-tbl");

		lCol = lElement.findAll("tr");
		size = lCol.count();

		// skip the first entry
		for (i = 1; i < size; i++)
		{
			lElement = lCol.at(i);

			// Name
			lstrTmp = lElement.findFirst("td[id=\"name\"]").toPlainText();
			ldata.updateProperty(mDbLstColName[eName], lstrTmp);

			// Website
			lstrTmp = lElement.findFirst("td[id=\"name\"]>a").attribute("href");
			ldata.updateProperty(mDbLstColName[eWeb], lstrTmp);

			// Town
			lstrTmp = lElement.findFirst("td[id=\"town\"]").toPlainText();
			ldata.updateProperty(mDbLstColName[eTown], lstrTmp);

			// Postcode
			lstrTmp = lElement.findFirst("td[id=\"postcode\"]").toPlainText();
			ldata.updateProperty(mDbLstColName[ePostcode], lstrTmp);

			// Tel
			lstrTmp = lElement.findFirst("td[id=\"telephone\"]").toPlainText();
			ldata.updateProperty(mDbLstColName[eTel], lstrTmp);

			// Email
			lstrTmp = lElement.findFirst("td[id=\"email\"]").toPlainText();
			ldata.updateProperty(mDbLstColName[eEmail], lstrTmp);

			mlstData << ldata;
		}

		mpDBLogger->logDataList(mlstData);
		DEF_LOG(mpWebView->findFirst("h3").toPlainText());
		DEF_LOG(QString("Saved data: %1").arg(mlstData.count()));
		mlstData.clear();
	} while (false);

	// goto next search
	miCurRegion ++;

	meState = E_STATE_INIT;
	loadPage(BASEURL);
}

WebEngine_Main(inca_ltd_org_uk)
