#include "ciphe_org_uk.h"
#include <QFile>
#include <QMessageBox>
#include <QtWebKit/QWebElement>
#include <QtWebKit/QWebElementCollection>
#include <QDebug>

#define BASEURL     "http://www.ciphe.org.uk/Consumers/Search_the_Register_of_Plumbers/"
#define BASEURL2    "http://www.ciphe.org.uk/Find-a-Plumber/?pc="
#define TITLE       "Ciphe.org.uk"

ciphe_org_uk::ciphe_org_uk(QWidget *apParent)
	: BaseWebEngine(apParent)
{
	mLogger.setFilePath(DEF_STR_DEFAULT_LOGGER_NAME);
	mpDBLogger = new DBLogger(&mLogger);
	// init PostCode list
	loadPostCode();
//	setWebLoadTimeout(10000);

	setBaseURL(BASEURL2);
	setWindowTitle(TITLE);
}

ciphe_org_uk::~ciphe_org_uk()
{}

void ciphe_org_uk::doStart()
{
	SQLQueryBuilder lbuilder;

	mpDBLogger->logString(lbuilder.getSQLCreateTable(mstrTableName, mlstColName, mlstColKey));

	meState = E_STATE_INIT;
	gotoNextPage();
}

void ciphe_org_uk::doStop()
{
}

void ciphe_org_uk::doWebLoadFinished()
{
	switch(meState)
	{
	case E_STATE_INIT:
		gotoNextPage();
		break;

	case E_STATE_GET_MAIN_CATEGORY:
		getData();
		break;

	default:
		break;
	}
}

enum enuDatabase
{
	eName = 0,
	eDescription,
	eFullAdd, eAdd, eTown, eCounty, eCountry, ePostcode,
	ePhone,
	eFax,
	eMobile,
	eEmail,
	eWeb,
	eSpecialism
};

void ciphe_org_uk::initDatabase()
{
	mlstColName << "Name"
				<< "Description"
				<< "FullAddress" << "Address" << "Town" << "County" << "Country" << "Postcode"
				<< "Phone"
				<< "Fax"
				<< "Mobile"
				<< "Email"
				<< "Web Site"
				<< "Specialism";
	mlstColKey << 0 << 2 << 11;

	mstrTableName = "ciphe_org";
}

void ciphe_org_uk::loadPostCode()
{
	QString lstrLine;
	QFile lFile(":/postcode.csv");
	if(!lFile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		DEF_LOG("Can not open post code file");
		return;
	}

	mlstPostcode.clear();
	mlstPostcode << "EIRE";
	while(!lFile.atEnd())
	{
		lstrLine = lFile.readLine();
		lstrLine = lstrLine.remove("\n");
		mlstPostcode << lstrLine;
	}

	lFile.close();
}

QString ciphe_org_uk::getNextPostcode()
{
	QString lstrRet = "";
	QString lstrStatus = "Remain %1 items";

	do
	{
		if(mlstPostcode.isEmpty())
			break;

		lstrRet = mlstPostcode.takeFirst();
		setStatus(lstrStatus.arg(mlstPostcode.count()));
	} while(false);

	return lstrRet;
}

void ciphe_org_uk::gotoNextPage()
{
	QString lstrPostcode = getNextPostcode();

	do
	{
		if(lstrPostcode == "")
		{
			showFinishMsg();
			break;
		}

		setBaseURL(BASEURL2 + lstrPostcode);
		meState = E_STATE_GET_MAIN_CATEGORY;

		loadPage();
	} while(false);
}

void ciphe_org_uk::getData()
{
	QWebElementCollection   llstElements;
	QString     lstrClassNameTmp = "vcard mapnumb numb%1";
	QString     lstrSelectorTmp = "div[class=\"%1\"]";
	QString     lstrSelector;
	int         i = 1;
	int         liCount;

	DataEntry   lData(mlstColName);
	QString     lstrText, lstrTmp;
	QString     lstrLogTmp = "%1 \t--> log %2 items";
	TAG_ADDRESS ltAdd;

	QWebElementCollection   llstUL;
	QWebElementCollection   llstLI;

	do
	{
		lData.clearData();

//        lstrClassName = lstrClassNameTmp.arg(i);
		lstrSelector = lstrSelectorTmp.arg(lstrClassNameTmp.arg(i));
//        qDebug() << "selector: " << lstrSelector;

		llstElements = mpWebView->document().findAll(lstrSelector);
		if(llstElements.count() == 0)
		{
			// Save data
			liCount = mpDBLogger->logDataList(mlstData, mstrTableName);
			mlstData.clear();
			mLogger.logString(lstrLogTmp.arg(getURL()).arg(liCount));

			gotoNextPage();
			break;
		}

		//For next item
		i++;


		lstrText = "Name: " + llstElements[0].toPlainText().trimmed() + "\nEnd";
//        qDebug() << lstrText;

		Utilities::extractAndUpdate(&lData, mlstColName[eName], lstrText, "Name: ", "\n");
		Utilities::extractAndUpdate(&lData, mlstColName[eDescription], lstrText, lData.getProperty(mlstColName[eName]) + "\n", "\n");
		//Address
		lstrTmp = Utilities::extractValue(lstrText, "Address:", "\n");
		ltAdd = Utilities::parseUK_Add(lstrTmp);
		lData.updateProperty(mlstColName[eFullAdd], ltAdd.strFullAddress);
		lData.updateProperty(mlstColName[eAdd], ltAdd.strAddress);
		lData.updateProperty(mlstColName[eTown], ltAdd.strTown);
		lData.updateProperty(mlstColName[eCounty], ltAdd.strCounty);
		lData.updateProperty(mlstColName[eCountry], ltAdd.strCountry);
		lData.updateProperty(mlstColName[ePostcode], ltAdd.strPostcode);

		Utilities::extractAndUpdate(&lData, mlstColName[ePhone], lstrText, "Phone:", "\n");
		Utilities::extractAndUpdate(&lData, mlstColName[eFax], lstrText, "Fax:", "\n");
		Utilities::extractAndUpdate(&lData, mlstColName[eMobile], lstrText, "Mobile:", "\n");
		Utilities::extractAndUpdate(&lData, mlstColName[eEmail], lstrText, "Email:", "\n");
		Utilities::extractAndUpdate(&lData, mlstColName[eWeb], lstrText, "Web Site:", "\n");

		// Specialsm:
		lstrTmp = "";
		llstUL = llstElements[0].findAll("ul[class=\"specialism\"]");
		if(llstUL.count() <= 0)
			goto _next;
		llstLI = llstUL[0].findAll("li");
		lstrTmp = "";
		for(int li = 1; li < llstLI.count(); li++)
			lstrTmp += llstLI[li].toPlainText() + ", ";
		if(lstrTmp.endsWith(", "))
			lstrTmp.chop(2);
		_next:
		lData.updateProperty(mlstColName[eSpecialism], lstrTmp);

//        lData.printDebug();
		mlstData << lData;
	}while(true);
}

WebEngine_Main(ciphe_org_uk)
