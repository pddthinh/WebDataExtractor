#include "niceic_com.h"

#define BASEURL "http://niceic.com/proximitysearch/mainform.aspx"
#define DETAIL_ELEMENT_START_KW		"javascript:__doPostBack('gvContractors$ctl"

#define ITEM_POST_ID_NAME			"lkbtnName"
#define ITEM_POST_ID_PT				"lblPostTown"
#define ITEM_POST_ID_PC				"lblPostCode"
#define ITEM_POST_ID_TEL			"lblTelephone"
#define ITEM_POST_ID_DIS			"imgDIS"
#define ITEM_POST_ID_AC				"imgAC"
#define ITEM_POST_ID_PAT			"imgPAT"
#define ITEM_POST_ID_MCS			"imgMCS"
#define ITEM_POST_ID_TM				"imgTM"

Niceic::Niceic(QWidget *apParent)
	: BaseWebEngine(apParent)
{
	setWindowTitle("Niceic.com");
	setBaseURL(BASEURL);

	if(mpDBLogger != NULL)
		mpDBLogger->setLogPath("niceic.sql");
	mpLog->setFilePath("niceic.txt");
	///////////////////////////////////////////////////////////

	setWebLoadTimeout();
	initAJAXMonitor();

	initCustomControl();
}

void Niceic::initExtWidget()
{
	mpExtFileLoading = new ExtFileLoading(this);
}

enum enuDatabase
{
	eName = 0,
	eDIS, eAC,
	ePAT, eMCS, eTM,
	eAddress, ePostTown, ePostCode,
	eTel, eEmail, eWebsite,
	eID,
	eDatabaseMax
};

void Niceic::initDatabase()
{
	mDbLstColName << "Name"
				  << "DIS" << "AC"
				  << "PAT" << "MCS" << "TM"
				  << "Address" << "PostTown" << "PostCode"
				  << "Tel" << "Email" << "Website"
				  << "ID";
	mDbLstKey << eName << ePostTown << ePostCode;
	mDbTableName = "niceic_com";

	// for mysql connection
	mDBName = "niceic";
	mDBHost = "localhost";
	mDBUsername = "root";
	mDBPassword = "123456";

	foreach (int idx, mDbLstKey)
		mlstDBKey << mDbLstColName[idx];
}

void Niceic::doStart()
{
	meState = E_STATE_INIT;
	prepareStarting();

	loadPage(BASEURL);
}

void Niceic::doWebLoadFinished()
{
	switch(meState)
	{
	case E_STATE_INIT:
		initSearching();
		break;

	case E_STATE_GET_DETAIL_INFO:
		getDetailData();
		break;

	case E_STATE_SUB1:
	{
		if (mblTimeout == true)
		{
			// skip current data entry
			miCurrentEntry ++;
			requestDetailEx();
			break;
		}

		extractDetailEx();
		break;
	}

	default:
		qDebug() << "default";
		break;
	}
}

void Niceic::initAJAXMonitor()
{
	mpAJAXMonitor = new QTimer(this);
	mpAJAXMonitor->setInterval(AJAX_MONITOR_INTERVAL);
	mpAJAXMonitor->setSingleShot(true);

	connect(mpAJAXMonitor, SIGNAL(timeout()), this, SLOT(sltAJAXTimeout()));
	miNumTry = 0;
}

void Niceic::startMonitorAJAX(const QString &astrElementId)
{
	mstrMonitorElementId = astrElementId;

	// Remove the monitoring element
	QWebElement lElement = mpWebView->getElementById(mstrMonitorElementId);
	if (lElement.isNull() == false)
	{
		lElement = lElement.findFirst("table");
		if (lElement.isNull() == false)
			lElement.removeFromDocument();
	}

	mblTimeout = false;
	miNumTry = 0;
	mpAJAXMonitor->start();
}

void Niceic::stopMonitorAJAX()
{
	mpAJAXMonitor->stop();
}

void Niceic::sltAJAXTimeout()
{
	do
	{
		if (miNumTry == AJAX_MAX_TRY)
		{
			mblTimeout = true;

			qDebug() << "Timeout";
			DEF_LOG(QString("Can not get data of [%1]").arg(mstrPostcode));
			miNumTry = 0;
			mpAJAXMonitor->stop();

			// continue other phase
			doWebLoadFinished();
			break;
		}

		QWebElement lElement;
		QWebElement lContainer;

		lElement = mpWebView->getElementById(mstrMonitorElementId);
		DEF_CHECK_BREAK(lElement, "sltAJAXMonitor, can not get data container element");
		lContainer = lElement.findFirst("table");

		if (lContainer.isNull() == true)
		{
			miNumTry ++;
			mpAJAXMonitor->start();
			QString message = QString("[%1] ---> monitor: %2").arg(mstrPostcode).arg(miNumTry);
			DEF_LOG(message);
			qDebug() << message;
			break;
		}

		qDebug() << "Got data";
		miNumTry = 0;
		doWebLoadFinished();
	} while (false);
}

/*
 * Load setting from user
 */
void Niceic::prepareStarting()
{
	do
	{
		mlstPostcode.clear();

		QString lStrLoadPath = mpExtFileLoading->getLoadPath();
		if (lStrLoadPath.isEmpty() == true)
		{
			qDebug() << "Load file path is empty";
			break;
		}

		mlstPostcode = Utilities::loadTextData(lStrLoadPath);
		if (mlstPostcode.size() == 0)
		{
			DEF_LOG(QString("Can not load postcode data, file path: %1").arg(lStrLoadPath));
			break;
		}

		int liSkipItems = mpExtFileLoading->getNumberOfSkipItem();
		int liCnt = 0;

		while (liSkipItems > 0 && mlstPostcode.size() > 0)
		{
			liCnt++;
			liSkipItems --;
			mlstPostcode.removeFirst();
		}

		DEF_LOG(QString("Total item skip: %1").arg(liCnt));
		DEF_LOG(QString("Total postcode: %1").arg(mlstPostcode.size()));
	} while (false);
}

bool Niceic::gotoNextPostcode()
{
	bool lblRet = false;

	do
	{
		if (mlstPostcode.isEmpty())
		{
			DEF_LOG("Postcode list is empty");
			break;
		}

		setStatus(QString("Remain items: %1").arg(mlstPostcode.size()));
		mstrPostcode = mlstPostcode.takeFirst();
		lblRet = true;
	} while (false);

	return lblRet;
}

void Niceic::initSearching()
{
	QWebElement lElement;
	QWebElementCollection lOptions;

	do
	{
		if (gotoNextPostcode() == false)
		{
			showFinishMsg("Finished");
			break;
		}

		mpWebView->doClick("*#cbMCS");
		mpWebView->doClick("*#cbTrustMark");
		mpWebView->doClick("*#cbPIR");

		lElement = mpWebView->getElementById("ddlRadius");
		if (lElement.isNull() == false)
		{
			lOptions = lElement.findAll("option");

			lOptions.at(0).removeAttribute("selected");
//			lOptions.at(lOptions.count() - 1).setAttribute("selected", "selected");

			// search arround 10miles
			lOptions.at(1).setAttribute("selected", "selected");
		}

		lElement = mpWebView->getElementById("txtPostCode");
		DEF_CHECK_BREAK(lElement, "Can not get postcode element");
		lElement.setAttribute("value", mstrPostcode);

		// click search button
		mpWebView->doClick("input[id=\"btnPostcodeSearch\"]");
		meState = E_STATE_GET_DETAIL_INFO;

		miCurrentPage = 1;

		// start monitor
		startMonitorAJAX("UpdatePanel1");
	} while (false);
}

void Niceic::getDetailData()
{
	QWebElement lElement;
	QWebElementCollection lCol;

	QWebElement lETmp;
	QString lstrTmp;

	DataEntry ldata(mDbLstColName);

	do
	{
		lElement = mpWebView->getElementById("UpdatePanel1");
		if (lElement.isNull())
		{
			DEF_LOG("Can not find data table");
			goto _next_postcode;
		}

		lElement = lElement.findFirst("tbody");
		if (lElement.isNull() && miNumTry >= AJAX_MAX_TRY)
		{
			DEF_LOG("Can not find data body");
			goto _next_postcode;
		}

		// Find and extract base info
		lCol = lElement.findAll("tr");
		foreach (lElement , lCol)
		{
			lETmp = lElement.findFirst("a");
			if (lETmp.isNull())
				continue;

			lstrTmp = lETmp.attribute("href");
			if (lstrTmp.startsWith(DETAIL_ELEMENT_START_KW) == false)
				continue;

			// Found the correct row
			// extract base data
			extract1Row(&lElement, &ldata);

			// check previous database
			if (mpMySQLHandler->checkExisted(&ldata, mDbTableName, mlstDBKey) == true)
			{
				qDebug() << "Found existed entry, skip ...";
				DEF_LOG(QString("\tExisted: %1").arg(ldata.getDataString()));

				// save log data list
				mLogDataList << ldata;

				continue;
			}

			mlstData << ldata;
		}

		miCurrentEntry = 0;

		// Next, extract extend info
		requestDetailEx();
	} while (false);

	return;

_next_postcode:
	meState = E_STATE_INIT;
	loadPage(BASEURL);
}

void Niceic::extract1Row(QWebElement *apElement, DataEntry *apData)
{
	QWebElement lElement;
	QString lstrTmp;
	QString lstrIdPre;
	QString lstrID;

	do
	{
		if (apElement == NULL || apData == NULL)
		{
			qDebug() << "Input is null";
			break;
		}

		lElement = apElement->findFirst("a");
		if (lElement.isNull())
		{
			DEF_LOG("Invalid row data");
			break;
		}

		lstrTmp = lElement.attribute("id");
		if (lstrTmp.isEmpty())
		{
			DEF_LOG("Invalid row data");
			break;
		}

		int li = lstrTmp.lastIndexOf("_");
		if (li < 0)
		{
			DEF_LOG("Invalid row data");
			break;
		}

		lstrIdPre = "*#" + lstrTmp.left(li) + "_%1";

		apData->clearData();

		// Name
		lstrTmp = "";
		lstrID = lstrIdPre.arg(ITEM_POST_ID_NAME);
		lElement = apElement->findFirst(lstrID);
		if (lElement.isNull() == false)
			lstrTmp = lElement.toPlainText();
		apData->updateProperty(mDbLstColName[eName], lstrTmp);
		apData->updateProperty(mDbLstColName[eID], lstrID.remove("*#"));

		// PostTown
		lstrTmp = "";
		lstrID = lstrIdPre.arg(ITEM_POST_ID_PT);
		lElement = apElement->findFirst(lstrID);
		if (lElement.isNull() == false)
			lstrTmp = lElement.toPlainText();
		apData->updateProperty(mDbLstColName[ePostTown], lstrTmp);

		// PostCode
		lstrTmp = "";
		lstrID = lstrIdPre.arg(ITEM_POST_ID_PC);
		lElement = apElement->findFirst(lstrID);
		if (lElement.isNull() == false)
			lstrTmp = lElement.toPlainText();
		apData->updateProperty(mDbLstColName[ePostCode], lstrTmp);

		// Telephone
		lstrTmp = "";
		lstrID = lstrIdPre.arg(ITEM_POST_ID_TEL);
		lElement = apElement->findFirst(lstrID);
		if (lElement.isNull() == false)
			lstrTmp = lElement.toPlainText();
		apData->updateProperty(mDbLstColName[eTel], lstrTmp);

		// DIS
		lstrTmp = "";
		lstrID = lstrIdPre.arg(ITEM_POST_ID_DIS);
		lElement = apElement->findFirst(lstrID);
		if (lElement.isNull() == false)
			lstrTmp = "T";
		apData->updateProperty(mDbLstColName[eDIS], lstrTmp);

		// AC
		lstrTmp = "";
		lstrID = lstrIdPre.arg(ITEM_POST_ID_AC);
		lElement = apElement->findFirst(lstrID);
		if (lElement.isNull() == false)
			lstrTmp = "T";
		apData->updateProperty(mDbLstColName[eAC], lstrTmp);

		// PAT
		lstrTmp = "";
		lstrID = lstrIdPre.arg(ITEM_POST_ID_PAT);
		lElement = apElement->findFirst(lstrID);
		if (lElement.isNull() == false)
			lstrTmp = "T";
		apData->updateProperty(mDbLstColName[ePAT], lstrTmp);

		// MCS
		lstrTmp = "";
		lstrID = lstrIdPre.arg(ITEM_POST_ID_MCS);
		lElement = apElement->findFirst(lstrID);
		if (lElement.isNull() == false)
			lstrTmp = "T";
		apData->updateProperty(mDbLstColName[eMCS], lstrTmp);

		// TM
		lstrTmp = "";
		lstrID = lstrIdPre.arg(ITEM_POST_ID_TM);
		lElement = apElement->findFirst(lstrID);
		if (lElement.isNull() == false)
			lstrTmp = "T";
		apData->updateProperty(mDbLstColName[eTM], lstrTmp);
	} while (false);
}

bool Niceic::requestDetailEx()
{
	bool lblRet = false;
	DataEntry ldata;
	QWebElement lElement;

	do
	{
		if (mlstData.size() == 0)
		{
			qDebug() << "Data list is empty";
			break;
		}

		// At the last entry in current list
		if (miCurrentEntry >= mlstData.size())
			break;

		ldata = mlstData[miCurrentEntry];
		lElement = mpWebView->getElementById(ldata.getProperty(mDbLstColName[eID]));
		DEF_CHECK_BREAK(lElement, "Can not find element Name");

		// do the click
		meState = E_STATE_SUB1;
		mpWebView->doClick(&lElement);

		// monitor AJAX
		startMonitorAJAX("updPnlContractorDetail");
		lblRet = true;
	} while (false);

	// check request result
	if (lblRet == false)
	{
		if (mlstData.size() > 0)
		{
			// Can not extract more info, same them
			// Insert to database
			mpMySQLHandler->updateDatabase(mlstData, mDbTableName);
			mpDBLogger->logDataList(mlstData);
			mlstData.clear();
		}

		mpDBLogger->logDataList(mLogDataList);
		mLogDataList.clear();

		// goto next page
		lblRet = requestNextPage();
	}

	return lblRet;
}

void Niceic::extractDetailEx()
{
	QWebElement lElement;
	DataEntry ldata;

	QString lstrData;

	do
	{
		lElement = mpWebView->getElementById("dvContractorDetail");
		DEF_CHECK_BREAK(lElement, "Can not find contractor detail element");

		ldata = mlstData[miCurrentEntry];

		lstrData = lElement.toPlainText() + "\n";

		// Address
		Utilities::extractAndUpdate(&ldata, mDbLstColName[eAddress], lstrData, "Address");

		// Email
		Utilities::extractAndUpdate(&ldata, mDbLstColName[eEmail], lstrData, "Email");

		// Website
		Utilities::extractAndUpdate(&ldata, mDbLstColName[eWebsite], lstrData, "Web Site");

		// save entry back to list
		mlstData[miCurrentEntry] = ldata;
	} while (false);

	// goto next entry in list
	miCurrentEntry ++;
	requestDetailEx();
}

bool Niceic::requestNextPage()
{
	bool lblRet = false;
	QWebElement lElement;
	QWebElementCollection lCol;

	QString lstrTmp;
	int liNextPage;

	int liCount, liSize;

	do
	{
		lElement = mpWebView->getElementById("UpdatePanel1");
		DEF_CHECK_BREAK(lElement, "Can not find main table");

		lElement = lElement.findFirst("td");
		DEF_CHECK_BREAK(lElement, "Can not find navigator panel");

		liNextPage = miCurrentPage + 1;
		lCol = lElement.findAll("a");
		// find next page
		liSize = lCol.count();
		for (liCount = 0; liCount < liSize; liCount++)
		{
			lElement = lCol[liCount];
			lstrTmp = lElement.toPlainText();
			if ((lstrTmp == QString("%1").arg(liNextPage)) ||
					(lstrTmp == "..." && liCount == liSize - 1))
			{
				DEF_LOG(QString("[%1] --> goto page: %2").arg(mstrPostcode).arg(liNextPage));
				qDebug() << "goto page: " << liNextPage;

				miCurrentPage = liNextPage;
				meState = E_STATE_GET_DETAIL_INFO;
				mpWebView->doClick(&lElement);

				// start monitor
				startMonitorAJAX("UpdatePanel1");

				lblRet = true;
				break;
			}
		}
	} while (false);

	// Goto next postcode
	if (lblRet == false)
	{
		meState = E_STATE_INIT;
		loadPage(BASEURL);
	}

	return lblRet;
}

WebEngine_Main(Niceic)
