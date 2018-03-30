#include "gdorb_decc_gov_uk.h"

//TODO: have to define one of these type
//#define EXE_INSTALLERS_ONLY
#define EXE_PROVIDERS_ONLY
//#define EXE_ASSESSORS_ONLY

#define BASEURL "http://gdorb.decc.gov.uk/find-a-green-deal-supplier?DECC=true"

gdorb_decc_gov_uk::gdorb_decc_gov_uk(QWidget *apParent)
	: BaseWebEngine(apParent)
{
	// for using custom control, call this function then implement initExtWidget()
	initCustomControl();

	setWindowTitle("gdorb_decc_gov_uk");
	setBaseURL(BASEURL);

	if(mpDBLogger != NULL)
		mpDBLogger->setLogPath("gdorb.sql");

	mpLog->setFilePath("gdorb.txt");
	///////////////////////////////////////////////////////////
}

enum enuDatabase
{
	eName = 0, eCerID, eCerDate,
	eTel, eWeb, eEmail,
	eServices,
	eAdd, eTown, eCounty, eCountry, ePostcode, eFullAdd,
	eDatabaseMax
};

void gdorb_decc_gov_uk::initDatabase()
{
	mDbLstColName << "Name" << "CerID" << "DateCer"
				  << "Tel" << "Web" << "Email"
				  << "Services"
				  << "Address" << "Town" << "County" << "Country" << "Postcode" << "FullAddress";
	mDbLstKey << eName << eCerID;
	mDbTableName = "gdorb";
}

void gdorb_decc_gov_uk::doStart()
{
	do
	{
		if (loadSavedData() == true)
		{
			// start from saved data
			if (gotoNextDataLink() == false)
			{
				showFinishMsg(QLatin1String("Unknown error!\nFail to goto next data link."));
				break;
			}

			break;
		}

		meState = E_STATE_INIT;
		loadPage(BASEURL);
	} while(0);
}

void gdorb_decc_gov_uk::doWebLoadFinished()
{
	switch(meState)
	{
	case E_STATE_INIT:
		getMainList();
		break;

	case E_STATE_GET_MAIN_CATEGORY:
		updatePageLimit();
		break;

	case E_STATE_GET_SUB_CATEGORY:
		getCompanyList();
		break;

	case E_STATE_GET_DETAIL_INFO:
		parseDetailData();
		break;

	default:
		qDebug() << "default";
		break;
	}
}

bool gdorb_decc_gov_uk::loadSavedData()
{
	bool blRet = false;

	do
	{
		if (mpExtFileLoading->isLoadFromFile() == false)
			break;

		mlstDataLink = mpExtFileLoading->loadData();
		if (mlstDataLink.isEmpty() == true)
		{
			qDebug() << QLatin1String("Loaded empty list data!");
			break;
		}

		blRet = true;
	} while(0);

	return blRet;
}

void gdorb_decc_gov_uk::getMainList()
{
	QWebElement element;

	do
	{
#ifdef EXE_INSTALLERS_ONLY
		// set company type = [Installer only]
		element = mpWebView->getElementById(QLatin1String("company-type"));
		mpWebView->changeCmbSelectedElement(&element, 3);
#endif

#ifdef EXE_PROVIDERS_ONLY
		// company type = [Providers only]
		element = mpWebView->getElementById(QLatin1String("company-type"));
		mpWebView->changeCmbSelectedElement(&element, 2);

		// domestic = [Home]
		element = mpWebView->getElementById(QLatin1String("domestic"));
		mpWebView->changeCmbSelectedElement(&element, 1);
#endif

#ifdef EXE_ASSESSORS_ONLY
		// company type = [Assessors only]
		element = mpWebView->getElementById(QLatin1String("company-type"));
		mpWebView->changeCmbSelectedElement(&element, 1);

		// domestic = [Home]
		element = mpWebView->getElementById(QLatin1String("domestic"));
		mpWebView->changeCmbSelectedElement(&element, 1);
#endif

		// do click
		meState = E_STATE_GET_MAIN_CATEGORY;
		element = mpWebView->getElementById(QLatin1String("search_button"));
		mpWebView->doClick(&element);
	}
	while (0);
}

void gdorb_decc_gov_uk::updatePageLimit()
{
	QWebElement element;

	meState = E_STATE_GET_SUB_CATEGORY;

	// set number result = 500
	element = mpWebView->findFirstElement(QLatin1String("select"),
										  QLatin1String("name"),
										  QLatin1String("squiloople-pagination-limit"));
	mpWebView->changeCmbSelectedElement(&element, "500");
	mpWebView->load(QUrl(BASEURL + QLatin1String("&amp;page=1&amp;limit=500")));
}

void gdorb_decc_gov_uk::getCompanyList()
{
	QWebElement mainTable;
	QWebElement element;
	QWebElementCollection eCol;

	do
	{
		mainTable = mpWebView->findFirstElement(QLatin1String("table"),
												QLatin1String("class"),
												QLatin1String("mcsResultsTable"));
		DEF_CHECK_BREAK(mainTable, QLatin1String("Fail to get main search result table!"));

		eCol = mainTable.findAll(QLatin1String("a"));
		foreach (element, eCol)
		{
			if (element.toPlainText().startsWith(QLatin1String("More info")) == false)
				continue;

			mlstDataLink << mpWebView->getHrefURL(&element);
		}

		if (gotoNextResultPage() == true)
			break;

		DEF_LOG_LIST(mlstDataLink, QLatin1String("Company list:"));
		qDebug() << QString("Saved link: %1").arg(mlstDataLink.size());

		if (mpExtFileLoading->getExtractLink() == true)
		{
			showFinishMsg(QLatin1String("Finished extract company link list."));
		}
	}
	while (0);
}

bool gdorb_decc_gov_uk::gotoNextResultPage()
{
	bool blRet = false;
	bool found = false;
	QWebElement container;
	QWebElement element;
	QWebElementCollection eCol;

	QString tmp;

	do
	{
		container = mpWebView->findFirstElement(QLatin1String("ul"),
												QLatin1String("class"),
												QLatin1String("squiloople-pagination"));
		DEF_CHECK_BREAK(container, QLatin1String("Fail to get pagination container!"));

		eCol = container.findAll(QLatin1String("a"));
		foreach (element, eCol)
		{
			tmp =element.toPlainText();
			qDebug() << "Text: " << tmp;
			if (element.toPlainText().compare("›") != 0)
				continue;

			found = true;
			break;
		}
		if (found == false)
			break;

		blRet = true;
		mpWebView->doClick(&element);
	}
	while(0);

	return blRet;
}

void gdorb_decc_gov_uk::parseDetailData()
{
	DataEntry data(mDbLstColName);
	QWebElement element, eTmp;
	QWebElement container;
	QString text, strTmp, tmp;
	QWebElementCollection eCol;

	do
	{
		container = mpWebView->findFirstDiv(QLatin1String("class"),
											QLatin1String("mcsColumnsTwoOne"));
		DEF_CHECK_BREAK(container, QLatin1String("Can not get the main column info"));

		// name
		element = mpWebView->findFirst(QLatin1String("h1"));
		data.parseElement(mDbLstColName[eName], element);

		text = container.toPlainText() + "\n";

		// cert ID
#ifndef EXE_PROVIDERS_ONLY
		Utilities::extractAndUpdate(&data, mDbLstColName[eCerID], text,
										QLatin1String("Certification ID:"));
#else
		Utilities::extractAndUpdate(&data, mDbLstColName[eCerID], text,
										QLatin1String("Provider ID:"));
#endif

		// cert date
#ifndef EXE_PROVIDERS_ONLY
		Utilities::extractAndUpdate(&data, mDbLstColName[eCerDate], text,
										QLatin1String("Date Certified:"));
#endif

		// tel
#ifndef EXE_PROVIDERS_ONLY
		Utilities::extractAndUpdate(&data, mDbLstColName[eTel], text,
										QLatin1String("Telephone:"));
#else
		strTmp = "";
		tmp = Utilities::extractValue(text,
									  QLatin1String("Green Deal Enquiries Telephone:"),
									  QLatin1String("\n"));
		if (tmp.isEmpty() == false)
			strTmp += tmp + ", ";

		tmp = Utilities::extractValue(text,
									  QLatin1String("Customer Enquiries Telephone:"),
									  QLatin1String("\n"));
		if (strTmp.contains(tmp) == false)
			strTmp += tmp + ", ";

		tmp = Utilities::extractValue(text,
									  QLatin1String("Complaints Telephone:"),
									  QLatin1String("\n"));
		if (strTmp.contains(tmp) == false)
			strTmp += tmp;

		if (strTmp.endsWith(", ") == true)
			strTmp.chop(2);
		data.updateProperty(mDbLstColName[eTel], strTmp);
#endif

		// website
		Utilities::extractAndUpdate(&data, mDbLstColName[eWeb], text,
										QLatin1String("Website:"));

		// email
#ifndef EXE_PROVIDERS_ONLY
		Utilities::extractAndUpdate(&data, mDbLstColName[eEmail], text,
										QLatin1String("Email:"));
#else
		strTmp = "";
		tmp = Utilities::extractValue(text,
									  QLatin1String("Green Deal Enquiries Email:"),
									  QLatin1String("\n"));
		if (tmp.isEmpty() == false)
			strTmp += tmp + ", ";

		tmp = Utilities::extractValue(text,
									  QLatin1String("Customer Enquiries Email:"),
									  QLatin1String("\n"));
		if (strTmp.contains(tmp) == false)
			strTmp += tmp + ", ";

		tmp = Utilities::extractValue(text,
									  QLatin1String("Complaints Email:"),
									  QLatin1String("\n"));
		if (strTmp.contains(tmp) == false)
			strTmp += tmp;

		if (strTmp.endsWith(", ") == true)
			strTmp.chop(2);
		data.updateProperty(mDbLstColName[eEmail], strTmp);
#endif

		// Address
#ifndef EXE_PROVIDERS_ONLY
		strTmp = Utilities::extractValue(text,
										 QLatin1String("Head Office\n"),
										 QLatin1String("\n"));
#else
		eCol = container.findAll(QLatin1String("p"));
		strTmp = eCol.last().toPlainText();
#endif
		TAG_ADDRESS tAdd = Utilities::parseUK_Add(strTmp);
		data.updateProperty(mDbLstColName[eFullAdd], tAdd.strFullAddress);
		data.updateProperty(mDbLstColName[eAdd], tAdd.strAddress);
		data.updateProperty(mDbLstColName[eTown], tAdd.strTown);
		data.updateProperty(mDbLstColName[eCounty], tAdd.strCounty);
		data.updateProperty(mDbLstColName[eCountry], tAdd.strCountry);
		data.updateProperty(mDbLstColName[ePostcode], tAdd.strPostcode);

		// services
		eTmp = container.findFirst(QLatin1String("ul"));
		eCol = eTmp.findAll(QLatin1String("li"));
		strTmp.clear();
		foreach (eTmp, eCol)
			strTmp += eTmp.toPlainText() + ", ";

		strTmp.remove(strTmp.length() - 2, 2);
		data.updateProperty(mDbLstColName[eServices], strTmp);

		mlstData << data;
	} while(0);

	if (mlstData.length() == 20)
	{
		mpDBLogger->logDataList(mlstData);
		DEF_LOG(QString("Save %1 entry").arg(mlstData.count()));
		mlstData.clear();
	}

	//TODO: goto next link
	if (gotoNextDataLink() == false)
	{
		if (mlstData.length() > 0)
		{
			mpDBLogger->logDataList(mlstData);
			DEF_LOG(QString("Save %1 entry").arg(mlstData.count()));
			mlstData.clear();
		}

		showFinishMsg(QLatin1String("Parsing finished"));
	}
}

WebEngine_Main(gdorb_decc_gov_uk)
