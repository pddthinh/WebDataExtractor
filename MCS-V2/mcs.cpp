#include "mcs.h"

MCS::MCS(QString baseURL,QString saveLinkPath, QWidget *apParent)
	:BaseWebEngine(apParent)
{
	mMCSBaseURL = baseURL;
	mMCSLinkFile = saveLinkPath;
	if (mMCSLinkFile.isEmpty())
		qWarning() << "Path for saving links file is empty!";

	initCustomControl();
	setBaseURL(mMCSBaseURL);
	setWebLoadTimeout(60000);
}

void MCS::initExtWidget()
{
	mpExtFileLoading = new ExtFileLoading(this);
}

void MCS::doStart()
{
	do
	{
		if (mpExtFileLoading->getExtractLink() == true)
		{
			meState = E_STATE_INIT;
			loadPage(mMCSBaseURL);
			break;
		}

		if (mpExtFileLoading->getLoadPath().isEmpty() == false)
		{
			if (loadDataFromFile() == false)
			{
				showFinishMsg("Can not load data from file!");
				break;
			}

			if (gotoNextDataLink() == false)
			{
				showFinishMsg("Error occur while going to next data link!");
				break;
			}
		}
	} while (false);
}

void MCS::doWebLoadFinished()
{
	switch(meState)
	{
	case E_STATE_INIT:
		initSearchState1();
		break;

	case E_STATE_SUB1:
		initSearchState2();
		break;

	case E_STATE_GET_MAIN_CATEGORY:
		getListItem();
		break;

	case E_STATE_GET_DETAIL_INFO:
		getDetailData();
		break;

	default:
		break;
	}
}

/**
 * Load data from input file
 *
 * @brief MCS::loadDataFromFile
 * @return
 */
bool MCS::loadDataFromFile()
{
	bool lblRet = false;

	do
	{
		QString filePath = mpExtFileLoading->getLoadPath();
		if (filePath.isEmpty())
		{
			qDebug() << "Load path is empty!";
			break;
		}

		mlstDataLink = Utilities::loadTextData(filePath);
		if (mlstDataLink.isEmpty())
		{
			qDebug() << "Data list is empty";
			break;
		}

		int liSkipped = mpExtFileLoading->getNumberOfSkipItem();
		while (liSkipped > 0)
		{
			mlstDataLink.takeFirst();
			liSkipped --;
		}

		qDebug() << "Loaded items: " << mlstDataLink.count();
		lblRet = true;
	} while (false);

	return lblRet;
}

/**
 * Searching state 1: select [Search All] and press [Search] button
 *
 * @brief MCS::initSearchState1
 * @return
 */
bool MCS::initSearchState1()
{
	bool	lblRet = false;

	do
	{
		QWebElement lSearchForm, lElement, leTmp;
		QWebElementCollection lCol;

		lSearchForm = mpWebView->findFirstDiv(QLatin1String("class"),
										   QLatin1String("mcsSearchForm"));
		DEF_CHECK_BREAK(lSearchForm, QLatin1String("Can not find element mcsSearchForm"));

		// Find the checkbox All
		{
			bool lblFound = false;
			lCol = lSearchForm.findAll("label");
			foreach (lElement, lCol)
			{
				if (lElement.toPlainText() != "All")
					continue;

				leTmp = lElement.previousSibling();
				DEF_CHECK_BREAK(leTmp, QLatin1String("Can not find element ismCheckBox"));
				mpWebView->doClick(&leTmp);
				lblFound = true;
				break;
			}

			if (lblFound == false)
			{
				qDebug() << "Can not find [All] element!";
				DEF_LOG("Can not find [All] element");
				break;
			}
		}

		// Find button Search
		{
			leTmp = mpWebView->findFirstElement(QLatin1String("input"),
												QLatin1String("value"),
												QLatin1String("Search"),
												&lSearchForm);
			DEF_CHECK_BREAK(leTmp, QLatin1String("Can not find [Search] button"));
			meState = E_STATE_SUB1;
			mpWebView->doClick(&leTmp);
		}

		lblRet = true;
	} while (false);

	return lblRet;
}

/**
 * Searching state 2: select max number of result (500)
 *
 * @brief MCS::initSearchState2
 * @return
 */
bool MCS::initSearchState2()
{
	bool lblRet = false;

	do
	{
		QWebElement lPaginator = mpWebView->findFirstDiv(QLatin1String("class"),
														 QLatin1String("ismPaginatorLeft"));
		DEF_CHECK_BREAK(lPaginator, QLatin1String("Can not find ismPaginatorLeft"));

		QWebElement lPageSize = mpWebView->findFirstElement(QLatin1String("select"),
															QLatin1String("class"),
															QLatin1String("ismSelectBox "),
															&lPaginator);
		DEF_CHECK_BREAK(lPageSize, QLatin1String("Can not get Page size element"));
		lblRet = mpWebView->changeCmbSelectedElement(&lPageSize,
													 QLatin1String("500"),
													 QLatin1String("true"));
		if (lblRet == false)
		{
			DEF_LOG(QLatin1String("Can not change Page size to 500!"));
			break;
		}

		meState = E_STATE_GET_MAIN_CATEGORY;
		lPageSize.evaluateJavaScript("this.onchange();");

		lblRet = true;
	} while (false);

	return lblRet;
}

/**
 * Get searched items list
 *
 * @brief MCS::getListItem
 */
void MCS::getListItem()
{
	do
	{
		// call child implementation
		extractSearchResult();

		if (goNextResultPage() == true)
			break;

		qDebug() << "Finished saving all items";

		{
			// filter the list result
			QString lstrLink;
			QMap<QString, QString> lMap;
			lMap.clear();

			foreach (lstrLink, mlstDataLink)
				lMap.insert(lstrLink, lstrLink);

			qDebug() << "Number of raw data: " << mlstDataLink.count();
			mlstDataLink.clear();
			mlstDataLink << lMap.values();
			qDebug() << "Number of data after filter: " << mlstDataLink.count();
		}

		if (mpExtFileLoading->getExtractLink() == true)
		{
			Logger *lpLogger = new Logger(mMCSLinkFile);
			lpLogger->logStringList(mlstDataLink);
			showFinishMsg("Saved all data links...");
			break;
		}
	} while (false);
}

/**
 * Go to next search result page
 *
 * @brief MCS::goNextResultPage
 * @return
 */
bool MCS::goNextResultPage()
{
	bool			lblRet = false;
	QWebElement		leNext;
	int				liCnt;
	QWebElementCollection	leLst = mpWebView->document().findAll("a[class=\"ismPaginatorArrow\"]");

	do
	{
		if(leLst.count() <= 0)
		{
			DEF_LOG("Can not find Next button group - End of search result page");
			break;
		}

		for(liCnt = 0; liCnt < leLst.count(); liCnt++)
		{
			leNext = leLst[liCnt];
			if(leNext.toPlainText() == ">")
				break;
		}

		if(liCnt == leLst.count())
		{
			DEF_LOG("End of search result page");
			qDebug() << "End of search result page";
			break;
		}

		qDebug() << "Next button: " << leNext.toPlainText();

		lblRet = true;
		mpWebView->doClick(&leNext);
		qDebug() << "Go to next search result page ...";
	}while(false);

	return lblRet;
}
