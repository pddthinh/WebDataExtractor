#include "yellowpage.h"

#define BASEURL ""

#define SEARCH_URL_FRM	"http://www.yellowpages.com/%1/%2?q=%3"
// %1 = state
// %2 = keyword (replace space = -)
// %3 = keyword (replace space = +)

#define KEYWORD_FILE	"yp-keywords.txt"

QString glstSkipState[] = {
	"HI",
	"AK",
	""
};

yellowpage::yellowpage(QWidget *apParent)
	: BaseWebEngine(apParent)
{
	setWindowTitle("yellowpage");
	setBaseURL(BASEURL);

	initCustomControl();

	if(mpDBLogger != NULL)
		mpDBLogger->setLogPath("yellowpage.sql");

	mpLog->setFilePath("yellowpage.txt");
	///////////////////////////////////////////////////////////
	loadUSStateList();
	loadKeywordList();

	setWebLoadTimeout(15000);
}

void yellowpage::initExtWidget()
{
	mpExtFileLoading = new ExtFileLoading(this);
}

enum enuDatabase
{
	eName = 0,
	ePhone, eFax,
	eAdd, eCity, eState, eZip,
	eEmail, eWebsite,
	eItemLink,
	eDatabaseMax
};

void yellowpage::initDatabase()
{
	mDbLstColName << "Name"
				  << "Phone" << "Fax"
				  << "Address" << "City" << "State" << "Zip"
				  << "Email" << "Website"
				  << "ItemLink";
	mDbLstKey << eName << eZip << ePhone;
	mDbTableName = "yellowpage";
}

void yellowpage::doStart()
{
	meState = E_STATE_INIT;
	miCurState = 0;
	miCurKeyword = 0;

	inputSearchKeyword();
}

void yellowpage::doWebLoadFinished()
{
	switch(meState)
	{
	case E_STATE_GET_MAIN_CATEGORY:
//		getDataList();
		break;

	case E_STATE_GET_DETAIL_INFO:
		getDetailData();
		break;

	default:
		qDebug() << "default";
		break;
	}
}

bool yellowpage::loadUSStateList()
{
	bool lblRet = false;

	mlstUSState.clear();
	mlstUSState = Utilities::loadTextData(":/us-state-list.txt");

	int i = 0;
	while (glstSkipState[i].isEmpty() == false)
	{
		mlstUSState.removeOne(glstSkipState[i]);
		i++;
	}

	lblRet = !(mlstUSState.isEmpty());

	return lblRet;
}

/* Load keywords from file */
bool yellowpage::loadKeywordList()
{
	bool lblRet = false;

	mlstKeyword = Utilities::loadTextData(KEYWORD_FILE);
	lblRet = (mlstKeyword.isEmpty());

	if (lblRet == false)
		DEF_LOG(QString("Can not load keyword in file: %1").arg(KEYWORD_FILE));

	return lblRet;
}

bool yellowpage::gotoNextState()
{
	bool	lblRet = false;

	do
	{
		miCurState++;
		if(miCurState >= mlstUSState.size())
		{
			qDebug() << "Reaches end of State list";
			DEF_LOG("Reaches end of State list");
			break;
		}

		setStatus(QString("Current state [%1]")
				  .arg(mlstUSState.at(miCurState)));
		DEF_LOG(QString("Current state [%1]")
				.arg(mlstUSState.at(miCurState)));

		lblRet = true;
	} while(false);

	return lblRet;
}

bool yellowpage::gotoNextKeyword()
{
	bool lblRet = false;

	do
	{
		if (miCurState < mlstUSState.size())
			break;

		miCurKeyword++;
		if (miCurKeyword >= mlstKeyword.size())
			break;

		// reset the state index
		miCurState = 0;
		qDebug() << "Next keyword: " << mlstKeyword[miCurKeyword];

		lblRet = true;
	} while (false);

	return lblRet;
}

void yellowpage::inputSearchKeyword()
{
	do
	{
		// Check State
		if(gotoNextState() == false)
		{
			if (gotoNextKeyword() == false)
			{
				DEF_LOG("Processed all States");
				showFinishMsg();
				break;
			}
		}

		QString lstrSearchURL;
		QString lstrParam1, lstrParam2, lstrParam3;

		lstrParam1 = mlstUSState.at(miCurState).toLower().trimmed().replace(" ", "+");
		lstrParam2 = mlstKeyword.at(miCurKeyword).toLower().trimmed().replace(" ", "-");
		lstrParam3 = mlstKeyword.at(miCurKeyword).toLower().trimmed().replace(" ", "+");

		lstrSearchURL = QString(SEARCH_URL_FRM)
						.arg(lstrParam1)
						.arg(lstrParam2)
						.arg(lstrParam3);
		meState = E_STATE_GET_MAIN_CATEGORY;
		loadPage(lstrSearchURL);
	} while(false);
}

bool yellowpage::gotoNextResultPage()
{
	bool lblRet = false;

	QWebElement leOL, leLI, lElement;

	do
	{
		leOL = mpWebView->findFirst("ol[class=\"track-pagination\"]");
		DEF_CHECK_BREAK(leOL, "Can not find track-pagination");

		leLI = leOL.findFirst("li[class=\"next\"]");
		DEF_CHECK_BREAK(leLI, "Can not find next (page-navigation)");

		lElement = leLI.findFirst("a");

		lblRet = true;
		mpWebView->doClick(&lElement);
	} while (false);

	return lblRet;
}

//void yellowpage::getDataList()
//{
//	QWebElement lElement;
//	QWebElementCollection lCol, lLinks;

//	QString lstrTmp;

//	do
//	{
//		lCol = mpWebView->findAll("div[class=\"clearfix\"]");
//		foreach (lElement, lCol)
//		{
//			lLinks = lElement.findAll("a");
//			if (lLinks.count() != 1)
//				continue;

//			lstrTmp = lLinks.at(0).attribute("href");
//			mlstDataLink << lstrTmp;
//		}

//		qDebug() << "Found items: " << mlstDataLink.count();

//		if (MAX_ITEM > 0 && mlstDataLink.count() >= MAX_ITEM)
//		{
//			qDebug() << "Finished get data list, parse detail";
//			meState = E_STATE_GET_DETAIL_INFO;
//			loadPage(mlstDataLink.takeFirst());
//			break;
//		}

//		if (gotoNextResultPage() == false)
//		{
//			// TODO: check goto next keyword?

//			if (gotoNextState() == false)
//			{
//				showFinishMsg("Finished");
//				break;
//			}

//			inputSearchKeyword();
//		}
//	} while (false);
//}

void yellowpage::getDetailData()
{
	QWebElement lElement;
	QString lstrTmp;

	DataEntry ldata(mDbLstColName);

	do
	{
		lstrTmp = "";
		lElement = mpWebView->findFirst("h1[class=\"fn org\"]");
		if (lElement.isNull() == false)
			lstrTmp = lElement.toPlainText();
		ldata.updateProperty(mDbLstColName[eName], lstrTmp);

		lstrTmp = "";
		lElement = mpWebView->findFirst("p[class=\"phone tel\"]");
		if (lElement.isNull() == false)
			lstrTmp = lElement.toPlainText();
		ldata.updateProperty(mDbLstColName[ePhone], lstrTmp);

		lstrTmp = "";
		lElement = mpWebView->findFirst("span[class=\"street-address\"]");
		if (lElement.isNull() == false)
			lstrTmp = lElement.toPlainText();
		if (lstrTmp.endsWith(", "))
			lstrTmp.chop(2);
		ldata.updateProperty(mDbLstColName[eAdd], lstrTmp);

		lstrTmp = "";
		lElement = mpWebView->findFirst("span[class=\"locality\"]");
		if (lElement.isNull() == false)
			lstrTmp = lElement.toPlainText();
		ldata.updateProperty(mDbLstColName[eCity], lstrTmp);

		lstrTmp = "";
		lElement = mpWebView->findFirst("span[class=\"region\"]");
		if (lElement.isNull() == false)
			lstrTmp = lElement.toPlainText();
		ldata.updateProperty(mDbLstColName[eState], lstrTmp);

		lstrTmp = "";
		lElement = mpWebView->findFirst("span[class=\"postal-code\"]");
		if (lElement.isNull() == false)
			lstrTmp = lElement.toPlainText();
		ldata.updateProperty(mDbLstColName[eZip], lstrTmp);

		mlstData << ldata;
	} while (false);

	if (mlstData.count() == 10)
	{
		mpDBLogger->logDataList(mlstData);
		DEF_LOG(QString("Saved: %1").arg(mlstData.count()));

		mlstData.clear();
	}

	if (gotoNextDataLink() == false)
	{
		if (mlstData.count() > 0)
		{
			mpDBLogger->logDataList(mlstData);
			DEF_LOG(QString("Saved: %1").arg(mlstData.count()));

			mlstData.clear();
		}

		showFinishMsg("Finished all");
	}
}

WebEngine_Main(yellowpage)
