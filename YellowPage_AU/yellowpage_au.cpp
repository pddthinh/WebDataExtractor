#include "yellowpage_au.h"
#include <QLabel>
#include <QGroupBox>

#include <QUrl>
#include <QUrlQuery>

#include <QTime>

#define BASEURL "http://www.yellowpages.com.au/"

#define SEARCH_URL	"http://www.yellowpages.com.au/search/listings?clue=%1&locationClue=%2"

#define DATA_LINK_FILE			"yellowpage-data-link.txt"
#define DATA_SKIPPED_FILE		"yellowpage-skipped-link.txt"
#define FILE_SEARCH_KEYWORD		"yellowpage-search-keyword.txt"
#define FILE_SEARCH_LOCATION	"yellowpage-search-location.txt"
#define	FILE_CATEGORY_LINK		"yp-category-link.txt"

yellowpage_au::yellowpage_au(QWidget *apParent)
	: BaseWebEngine(apParent)
{
	setWindowTitle(QLatin1String("yellowpage_au"));
	setBaseURL(BASEURL);
	setWebLoadTimeout(25000);

	if(mpDBLogger != NULL)
		mpDBLogger->setLogPath(QLatin1String("yellowpage_au.sql"));

	mpLog->setFilePath(QLatin1String("yellowpage_au.txt"));
	miCurState = -1;

	mpDataLinkLog = new Logger(DATA_LINK_FILE);
	mpSkippedLinkLog = new Logger(DATA_SKIPPED_FILE);
	initCustomControl();

	loadStates();

	loadKeyword();
	miCurKeyword = 0;

	mpYPLogger = new Logger();
	mpLoadTimer = new QTimer(this);
	mpLoadTimer->setInterval(3000);
	mpLoadTimer->setSingleShot(true);
	connect(mpLoadTimer, SIGNAL(timeout()), this, SLOT(sltLoadTimeout()));

	// init random
	QTime time = QTime::currentTime();
	qsrand((uint)time.msec());
}

bool yellowpage_au::loadStates()
{
	bool	lblRet = false;
	QFile	lfile;

	do
	{
		// Load local state file first
		lfile.setFileName(FILE_SEARCH_LOCATION);
		if (lfile.open(QFile::ReadOnly | QFile::Text) == false)
		{
			qDebug() << QLatin1String("could not open local state file, name: ")
					 << FILE_SEARCH_LOCATION;

			DEF_LOG(QString("could not open local state file, %1").arg(FILE_SEARCH_LOCATION));
			DEF_LOG(QLatin1String("Use internal state list file..."));

			lfile.setFileName(QLatin1String(":/australian_state"));
			if(lfile.open(QFile::ReadOnly | QFile::Text) == false)
			{
				qDebug() << QLatin1String("Can not open resource file");
				DEF_LOG(QLatin1String("Could not open internal resource file"));
				break;
			}
		}

		QTextStream ltext(&lfile);

		mlstState.clear();
		while(!ltext.atEnd())
			mlstState << lfile.readLine().replace("\n","");

		lfile.close();

		qDebug() << QString("Load %1 locations").arg(mlstState.size());
		DEF_LOG(QString("Load %1 locations").arg(mlstState.size()));

		lblRet = true;
	}while(false);

	return lblRet;
}

bool yellowpage_au::loadKeyword()
{
	bool lblRet = false;
	QFile lfile;

	do
	{
		lfile.setFileName(FILE_SEARCH_KEYWORD);
		if (lfile.open(QFile::ReadOnly | QFile::Text) == false)
		{
			qDebug() << QLatin1String("Could not load keyword file, [")
					 << FILE_SEARCH_KEYWORD
					 << QLatin1String("]");
			DEF_LOG(QString("Could not load keyword file, [%1]").arg(FILE_SEARCH_KEYWORD));
			break;
		}

		QTextStream lstream(&lfile);
		mlstKeyword.clear();
		while(!lstream.atEnd())
			mlstKeyword << lfile.readLine().replace("\n","");
		lfile.close();

		qDebug() << QString("Load %1 keyword").arg(mlstKeyword.size());
		DEF_LOG(QString("Load %1 keyword").arg(mlstKeyword.size()));

		lblRet = true;
	} while (false);

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

void yellowpage_au::initDatabase()
{
	mDbLstColName << QLatin1String("ID")
				  << QLatin1String("Name")
				  << QLatin1String("Category")
				  << QLatin1String("State") << QLatin1String("Address")
				  << QLatin1String("Phone") << QLatin1String("Email") << QLatin1String("Website")
				  << QLatin1String("Item Link");
	mDbLstKey << eID << eEmail << eWebsite;
	mDbTableName = QLatin1String("yellowpage_au");
}

void yellowpage_au::doStart()
{
	do
	{
		// set the web timeout if any
		int timeout = mpExtFileLoading->getWebTimeout();
		if (timeout > 0)
			setWebLoadTimeout(timeout);

		// extract link only ?
		if (mpExtFileLoading->getExtractLink() == true)
		{
			inputSearchKeyword();
			break;
		}

		// load from file ?
		if (mpExtFileLoading->getLoadPath().isEmpty() == false)
		{
			mlstDataLink = Utilities::loadTextData(mpExtFileLoading->getLoadPath());
			if (mlstDataLink.isEmpty() == true)
			{
				showFinishMsg(QLatin1String("Fail to load data link!"));
				break;
			}

			int iSkipped = mpExtFileLoading->getNumberOfSkipItem();
			while (iSkipped > 0)
			{
				mlstDataLink.takeFirst();
				iSkipped --;
			}

			qDebug() << QLatin1String("Loaded items: ") << mlstDataLink.count();

			if(gotoNextDataLink() == false)
			{
				qDebug() << QLatin1String("Finished all data links");
				DEF_LOG(QLatin1String("Finished all data links"));

				showFinishMsg(QLatin1String("Finished all data links"));
				break;
			}

			break;
		}

		showFinishMsg(QLatin1String("Invalid setting!"));
	} while (false);
}

void yellowpage_au::doWebLoadFinished()
{
	switch(meState)
	{
	case E_STATE_INIT:
		inputSearchKeyword();
		break;

	case E_STATE_GET_MAIN_CATEGORY:
	{
		if (mpExtFileLoading->getExtractLink() == true)
		{
			getMainCategoryLink();
			break;
		}

		qDebug() << QLatin1String("Invalid state ...");
		break;
	}

	case E_STATE_GET_DETAIL_INFO:
		extractData();
		break;

	default:
		break;
	}
}

bool yellowpage_au::gotoNextState()
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

		qDebug() << QLatin1String("Next state: ") << mlstState[miCurState];

		lblRet = true;
	} while(false);

	return lblRet;
}

void yellowpage_au::inputSearchKeyword()
{
	do
	{
		// Check State
		if(gotoNextState() == false)
		{
			if (gotoNextKeyword() == false)
			{
				DEF_LOG(QLatin1String("Processed all States"));
				showFinishMsg();
				break;
			}
		}

		if (mlstKeyword.length() <= miCurKeyword)
		{
			qDebug() << QLatin1String("End of list keyword");
			break;
		}

		QString lstrSearchURL;
		QString lstrKeyword, lstrState;

		lstrKeyword = mlstKeyword[miCurKeyword].replace(" ", "+");
		lstrState = mlstState[miCurState].replace(" ", "+");

		DEF_LOG(QString("Current state [%1], keyword [%2]")
				.arg(mlstState[miCurState])
				.arg(mlstKeyword[miCurKeyword]));

		lstrSearchURL = QString(SEARCH_URL).arg(lstrKeyword).arg(lstrState);
		meState = E_STATE_GET_MAIN_CATEGORY;
		loadPage(lstrSearchURL);
	} while(false);
}

/**
 * Extract search result list v2
 *
 * @brief yellowpage_au::extractData
 */
void yellowpage_au::extractData()
{
	QWebElement		lelement, leEmailLink, leWebsiteLink, leTmp;
	QWebElementCollection	leCol;
	QString lstrTmp, lstrEmail, lstrWebsite;
	bool lblSaveEntry = false;
	DataEntry ldata(mDbLstColName);

	QStringList llstSkipped;

	QUrl url;
	QUrlQuery urlQuery;

	do
	{
		lelement = mpWebView->findFirst(QLatin1String("div.search-results"));
		DEF_CHECK_BREAK(lelement, QLatin1String("Can not get Search Result Listing element"));

		leCol = lelement.findAll(QLatin1String("div.cell.in-area-cell.middle-cell"));
		qDebug() << QLatin1String("Found items: ") << leCol.count();

		urlQuery = QUrlQuery(mpWebView->url());
		mcurState = urlQuery.queryItemValue(QLatin1String("locationClue"));

		foreach (lelement, leCol)
		{
			lblSaveEntry = false;
			ldata.clearData();

			// find the email, website link
			leEmailLink = lelement.findFirst(QLatin1String("a.contact.contact-email"));
			lstrEmail = leEmailLink.attribute(QLatin1String("data-email"));
			lstrTmp = lstrEmail.replace(QLatin1String("%40"), QLatin1String("@"));
			if (lstrTmp.indexOf('@') < 0)
			{
				url = QUrl(lstrTmp);
				lstrEmail = url.path();
			}
			if (lstrEmail.indexOf('@') > 0)
			{
				ldata.addProperty(mDbLstColName[eEmail], lstrEmail);
				lblSaveEntry = true;
			}

			leWebsiteLink = lelement.findFirst(QLatin1String("a.contact.contact-url"));
			lstrWebsite = mpWebView->getHrefURL(&leWebsiteLink);
			url = QUrl(lstrWebsite);
			if (url.host().isEmpty())
			{
				lstrWebsite.clear();
				lstrTmp = leWebsiteLink.attribute(QLatin1String("title"));
				lstrTmp = lstrTmp.replace(QLatin1String("(opens in a new window)"), QLatin1String(""))
						.trimmed();
				if (lstrTmp.indexOf('.') > 0)
					lstrWebsite = lstrTmp;
			}
			if (lstrWebsite.indexOf('.') > 0)
			{
				ldata.addProperty(mDbLstColName[eWebsite], lstrWebsite);
				lblSaveEntry = true;
			}

			// item link
			leTmp = lelement.findFirst(QLatin1String("a.listing-name"));
			lstrTmp = mpWebView->getHrefURL(&leTmp);

			if (lblSaveEntry == false)
			{
				// save to skipped list
				llstSkipped << lstrTmp;
				continue;
			}

			if (lstrTmp.isEmpty() == false)
				ldata.addProperty(mDbLstColName[eItemLink], lstrTmp);

			// name
			ldata.parseElement(mDbLstColName[eName], leTmp);

			// ID
			leTmp = lelement.findFirst(QLatin1String("div.listing.listing-search"));
			lstrTmp = leTmp.attribute(QLatin1String("data-listing-id"));
			if (lstrTmp.isEmpty() == false)
				ldata.addProperty(mDbLstColName[eID], lstrTmp);

			// item state
			ldata.updateProperty(mDbLstColName[eState], mcurState);

			// phone
			leTmp = lelement.findFirst(QLatin1String("a.contact.contact-phone"));
			lstrTmp = mpWebView->getHrefURL(&leTmp).replace(QLatin1String("tel:"), "");
			ldata.updateProperty(mDbLstColName[ePhone], lstrTmp);

			// address
			leTmp = lelement.findFirst(QLatin1String("p.listing-address"));
			ldata.parseElement(mDbLstColName[eAddress], leTmp);

			// category
			urlQuery = QUrlQuery(mpWebView->url());
			lstrTmp = urlQuery.queryItemValue(QLatin1String("suburb"));
			if (lstrTmp.isEmpty() == false)
				ldata.addProperty(mDbLstColName[eCategory], lstrTmp);

			mlstData << ldata;
		} // foreach

		// check to save extracted data
		if (mlstData.count() > 0)
		{
			mpDBLogger->logDataList(mlstData);
			DEF_LOG(QString("Direct extracted: %1").arg(mlstData.count()));
			mlstData.clear();
		}

		// check to save the skipped links
		if (llstSkipped.count() > 0)
		{
			mpSkippedLinkLog->logStringList(llstSkipped);
			DEF_LOG(QString("\tSkipped: %1").arg(llstSkipped.count()));
		}
	} while(false);

	// make a little wait
	doWait();
} // extractData

bool yellowpage_au::gotoNextKeyword()
{
	bool lblRet = false;

	do
	{
		if (miCurState < mlstState.size())
			break;

		miCurKeyword++;
		if (miCurKeyword >= mlstKeyword.size())
			break;

		// reset the state index
		miCurState = 0;
		qDebug() << QLatin1String("Next keyword: ") << mlstKeyword[miCurKeyword];

		lblRet = true;
	} while (false);

	return lblRet;
}

bool yellowpage_au::gotoNextResultPage()
{
	bool			lblRet = false;
	QWebElementCollection lePageBtnList;
	QWebElement		lelement;
	QWebElement		lePageContainer;

	do
	{
		lePageContainer = mpWebView->findFirst(QLatin1String("div.button-pagination-container"));
		lePageBtnList = lePageContainer.findAll(QLatin1String("a.button.button-pagination"));
		int iSize = lePageBtnList.count();
		if (iSize <= 0)
		{
			qDebug() << QLatin1String("Fail to get list of pagination buttions!");
			DEF_LOG(QLatin1String("Fail to get list of pagination buttions!"));
			break;
		}

		lelement = lePageBtnList.at(iSize - 1);
		if(lelement.isNull() ||
				lelement.toPlainText().startsWith(QLatin1String("NEXT")) == false)
		{
			qDebug() << QLatin1String("Reach end of search page");
			DEF_LOG(QString("End of result page: %1").arg(miPageCount));
			miPageCount = 0;
			break;
		}

		lblRet = true;
		miPageCount++;
		mpWebView->doClick(&lelement);
	} while(false);

	return lblRet;
}

/**
 * Get main category link only (using filter by Category menu)
 */
void yellowpage_au::getMainCategoryLink()
{
	QWebElement lelement;
	QWebElementCollection leCol;
	QString url, strTmp;
	QStringList llstURL;

	do
	{
		url = mpWebView->url().toString();
		leCol = mpWebView->findAll(QLatin1String("input.checkbox[name=suburb]"));
		foreach (lelement, leCol)
		{
			strTmp = lelement.attribute(QLatin1String("value")).replace(" ", "+");
			if (strTmp.isEmpty() == true)
				continue;

			llstURL << QString("%1&lat=&lon=&referredBy=UNKNOWN&selectedViewMode=list"
							   "&eventType=refinement&sortBy=relevance&suburb=%2")
					   .arg(url)
					   .arg(strTmp);
		} // for

		if (llstURL.isEmpty() == true)
			break;

		// save to file
		mpYPLogger->setFilePath(FILE_CATEGORY_LINK);
		mpYPLogger->logStringList(llstURL);
		DEF_LOG(QString("[%1] - Category saved: %2")
				.arg(mlstState[miCurState])
				.arg(llstURL.size()));
	} while (false);

	do
	{
		if (miCurState >= mlstState.size())
		{
			if (gotoNextKeyword() == false)
				showFinishMsg(QLatin1String("Finshed search all keywords"));
			break;
		}

		// goto next state
		DEF_LOG(QString("Finished state [%1]\n----------------------")
				.arg(mlstState[miCurState]));
		setBaseURL(BASEURL);
		doStart();
	} while (false);
}

/**
 * Make a little wait to prevent blocking from server
 */
void yellowpage_au::doWait()
{
	int time = Utilities::randInt(5, 10) * 1000;

	qDebug() << QLatin1String("Wait time: ") << time;

	mpLoadTimer->stop();
	mpLoadTimer->setInterval(time);
	mpLoadTimer->start();
}

void yellowpage_au::sltLoadTimeout()
{
	mpLoadTimer->stop();

//	qDebug() << QLatin1String("Resume ...");

	if(gotoNextResultPage() == false)
	{
		// Goto end of search result
		qDebug() << QLatin1String("Finish search result, state") << mcurState;
		DEF_LOG(QString("Finish search result, state: %1").arg(mcurState));
		DEF_LOG(QString("Remain data links: %1").arg(mlstDataLink.count()));
		DEF_LOG(QLatin1String("-----------------------------------------------"));

		if(gotoNextDataLink() == false)
			showFinishMsg();
	}
}

WebEngine_Main(yellowpage_au)
