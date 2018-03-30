#include "whitepage_au.h"

const QString gstrBaseURL		= "http://www.whitepages.com.au";
const QString gstrSearchURLFrm	= "http://www.whitepages.com.au/busSearch.do?subscriberName=%1&location=%2";

#define DEF_INT_MAX_KEYWORD		1

QString gstrKeyword[] = {
	"project builder"
};
int giCurKeyword = -1;

whitepage_au::whitepage_au(QWidget *apParent)
	: BaseWebEngine(apParent)
{
	initCustomControl();

	setWindowTitle("whitepage_au");
	setBaseURL(gstrBaseURL);

	if(mpDBLogger != NULL)
		mpDBLogger->setLogPath("whitepage_au.sql");
	mpLog->setFilePath("whitepage_au.txt");

	setWebLoadTimeout(10000);	// timeout 20s

	miCurState = -1;
	loadStates();
}

bool whitepage_au::loadStates()
{
	bool	lblRet = false;
	QFile	lfile;

	do
	{
		lfile.setFileName(":/australian_state");
		if(lfile.open(QFile::ReadOnly | QFile::Text) == false)
		{
			qDebug() << "Can not open resource file";
			break;
		}

		QTextStream ltext(&lfile);

		mlstState.clear();
		while(!ltext.atEnd())
			mlstState << lfile.readLine().replace("\n","");

		lfile.close();

		qDebug() << QString("Load %1 states").arg(mlstState.size());
		DEF_LOG(QString("Load %1 states").arg(mlstState.size()));

		lblRet = true;
	} while(false);

	return lblRet;
}

enum enuDatabase
{
	eID = 0,
	eName,
	eCategory,
	eState, eAddress,
	ePhone, eEmail, eWebsite,
	eItemLink,
	eDatabaseMax
};

void whitepage_au::initDatabase()
{
	mDbLstColName << QLatin1String("ID")
				  << QLatin1String("Name")
				  << QLatin1String("Category")
				  << QLatin1String("State") << QLatin1String("Address")
				  << QLatin1String("Phone") << QLatin1String("Email") << QLatin1String("Website")
				  << QLatin1String("Item Link");
	mDbLstKey << eName << ePhone << eEmail;
	mDbTableName = QLatin1String("whitepage_au");
}

void whitepage_au::doStart()
{
	inputSearchKeyword();
}

void whitepage_au::doWebLoadFinished()
{
	switch(meState)
	{
	case E_STATE_GET_DETAIL_INFO:
		getDetailData();
		break;

	default:
		qDebug() << QLatin1String("default");
		break;
	}
}

bool whitepage_au::gotoNextState()
{
	bool	lblRet = false;

	do
	{
		miCurState++;
		if(miCurState >= mlstState.size())
		{
			qDebug() << QLatin1String("Reaches end of State list");
			DEF_LOG(QLatin1String("Reaches end of State list"));
			break;
		}

		setStatus(QString("Current state [%1]").arg(mlstState[miCurState]));
		DEF_LOG(QString("Current state [%1]").arg(mlstState[miCurState]));

		lblRet = true;
	} while(false);

	return lblRet;
}

bool whitepage_au::gotoNextKeyword()
{
	bool lblRet = false;

	do
	{
		giCurKeyword++;
		if(giCurKeyword >= DEF_INT_MAX_KEYWORD)
		{
			qDebug() << QLatin1String("Reach end of keywords");
			break;
		}

		lblRet = true;
	} while(false);

	return lblRet;
}

void whitepage_au::inputSearchKeyword()
{
	do
	{
		// Check for the initial
		if(miCurState < 0)
		{
			if(gotoNextState() == false)
			{
				DEF_LOG(QLatin1String("Processed all States"));
				showFinishMsg();
				break;
			}
		}

		// Check keyword
		if(gotoNextKeyword() == false)
		{
			// Restart the keyword
			giCurKeyword = 0;

			// Goto next state
			if(gotoNextState() == false)
			{
				DEF_LOG(QLatin1String("Processed all States"));
				showFinishMsg();
				break;
			}
		}

		QString lstrSearchURL;
		QString lstrKeyword, lstrState;

		lstrKeyword = gstrKeyword[giCurKeyword].replace(" ", "+");
		lstrState = mlstState[miCurState].replace(" ", "+");

		lstrSearchURL = gstrSearchURLFrm.arg(lstrKeyword).arg(lstrState);

		meState = E_STATE_GET_DETAIL_INFO;

		loadPage(lstrSearchURL);
	} while(false);
}

bool whitepage_au::gotoNextSearchResultPage()
{
	bool lblRet = false;
	QWebElement	lelement;

	do
	{
		lelement = mpWebView->document().findFirst("li[class=\"navigation_next\"]");
		DEF_CHECK_BREAK(lelement, "Can not find navigation_next element");

		lelement = lelement.findFirst("a");
		DEF_CHECK_BREAK(lelement, "Can not find link in navigation_next element");

		lblRet = true;
		mpWebView->doClick(&lelement);
	} while(false);

	return lblRet;
}

void whitepage_au::getDetailData()
{
	QWebElement		leMain, lelement, leTmp;
	DataEntry		ldata(mDbLstColName);
	QString			lstrAdd;
	QWebElementCollection	lelst;
	QString			lstrTmp;
	QString			lstrId = QLatin1String("entry_%1");
	int				liCnt, liTmp;

	do
	{
		liCnt = 0;
		leMain = mpWebView->getElementById(lstrId.arg(liCnt));
		while(leMain.isNull() == false)
		{
			ldata.clearData();
			lstrAdd = "";

			// Name
			lstrTmp = "";
			lelement = leMain.findFirst("div[class=\"entry_title clearfix\"]");
			if(lelement.isNull() == false)
				lstrTmp = lelement.toPlainText();
			ldata.updateProperty(mDbLstColName[eName], lstrTmp);

			// Address
			lelement = leMain.findFirst("span[class=\"street_line\"]");
			if(lelement.isNull() == false)
				lstrAdd += lelement.toPlainText() + ", ";

			lelement = leMain.findFirst("span[class=\"locality\"]");
			if(lelement.isNull() == false)
				lstrAdd += lelement.toPlainText() + ", ";

			lelement = leMain.findFirst("span[class=\"state\"]");
			if(lelement.isNull() == false)
			{
				lstrAdd += lelement.toPlainText() + ", ";
				ldata.updateProperty(mDbLstColName[eState], lelement.toPlainText());
			}

			lelement = leMain.findFirst("span[class=\"postcode\"]");
			if(lelement.isNull() == false)
				lstrAdd += lelement.toPlainText() + ", ";

			if(lstrAdd.endsWith(", "))
				lstrAdd.chop(2);

			ldata.updateProperty(mDbLstColName[eAddress], lstrAdd);

			// Phone
			lelst = leMain.findAll("span");
			foreach(leTmp, lelst)
			{
				if(leTmp.attribute("class").startsWith("phone_number"))
				{
					ldata.updateProperty(mDbLstColName[ePhone], leTmp.toPlainText());
					break;
				}
			}

			// Email & Website
			lelst = leMain.findAll("a");
			liTmp = 0;
			foreach(leTmp, lelst)
			{
				// Email
				if(leTmp.attribute("class").startsWith("email"))
				{
					ldata.updateProperty(mDbLstColName[eEmail], leTmp.toPlainText());
					liTmp++;
				}

				// Website
				if(leTmp.attribute("class").startsWith("link website_link"))
				{
					ldata.updateProperty(mDbLstColName[eWebsite], leTmp.toPlainText());
					liTmp++;
				}

				if(liTmp >= 2)
					break;
			}

			ldata.updateProperty(mDbLstColName[eItemLink], mpWebView->url().toString());

			mlstData << ldata;
			if(mlstData.count() >= 10)
			{
				mpDBLogger->logDataList(mlstData);
				DEF_LOG(QString("Save %1 dataentry").arg(mlstData.count()));

				mlstData.clear();
			}

			liCnt++;
			leMain = mpWebView->getElementById(lstrId.arg(liCnt));
		}

		if(mlstData.count() >= 10)
		{
			mpDBLogger->logDataList(mlstData);
			DEF_LOG(QString("Save %1 dataentry").arg(mlstData.count()));

			mlstData.clear();
		}

		if(gotoNextSearchResultPage() == false)
		{
			qDebug() << "Can not go to next search result";

			inputSearchKeyword();
		}
	} while(false);
}

WebEngine_Main(whitepage_au)
