#include "mcsproduct.h"
#include <QtWebKit/QWebElement>
#include <QtWebKit/QWebElementCollection>

#ifdef WDE_QT5
#include <QUrlQuery>
#endif

#define BASEURL		"http://www.microgenerationcertification.org/consumers/product-search?filterPaginationPage=0"
#define SAVED_LINK	"product-link.txt"
#define XML_LOG_PATH	"product.xml"

MCSProduct::MCSProduct(QWidget *apParent)
	: BaseWebEngine(apParent)
{
	setWindowTitle("Product - [microgenerationcertification.org]");
	setBaseURL(BASEURL);
	initCustomControl();

	if(mpDBLogger != NULL)
		mpDBLogger->setLogPath("MCSProduct.sql");
	mpLog->setFilePath("MCSProduct.txt");

	setWebLoadTimeout(40000);

//	// init xmlUtil module
//	mpXMLUtil = new XMLUtil(XML_LOG_PATH);
//	if (mpXMLUtil->init() == false)
//		qDebug() << QLatin1String("Can not init XMLUtil module!");
}

void MCSProduct::doStart()
{
	do
	{
		if (mpExtFileLoading->getExtractLink() == true)
		{
			meState = E_STATE_SUB1;
			loadPage();
			break;
		}

		if (mpExtFileLoading->getLoadPath().isEmpty() == false)
		{
			if (loadSavedData(mpExtFileLoading->getLoadPath()) == false)
			{
				qDebug() << QLatin1String("Fail to load saved data!");
				showFinishMsg(QLatin1String("Fail to load saved data!"));
				break;
			}

			if (gotoNextSavedEntry() == false)
				qDebug() << QLatin1String("Fail to goto next saved entry");

			break;
		}
	} while (false);
}

void MCSProduct::doWebLoadFinished()
{
	switch(meState)
	{
	case E_STATE_SUB1:
		initSearchState1();
		break;

	case E_STATE_SUB2:
		initSearchState2();
		break;

	case E_STATE_GET_MAIN_CATEGORY:
		getProductListInfo();
		break;

	case E_STATE_GET_DETAIL_INFO:
		getDetailData();
		break;

	default:
		break;
	}
}

enum enumDatabase
{
	eKey = 0, eURL,
	eProName, eManufacturer, eProductType, eProModel, eCerNo,
	eWebsite, eDescription
};

void MCSProduct::initDatabase()
{
	mDbTableName = "MCS_Product";
	mDbLstColName	<< "Key" << "URL"
					<< "Product Name" << "Manufacturer" << "Product Type" << "Product Models" << "Cer No"
					<< "Website" << "Description";
	mDbLstKey << 0 << 2;
}

/**
 * Searching state 1: select [Search All] and press [Search] button
 *
 * @brief MCSProduct::initSearchState1
 * @return
 */
bool MCSProduct::initSearchState1()
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
			meState = E_STATE_SUB2;
			mpWebView->doClick(&leTmp);
		}

		lblRet = true;
	} while (false);

	return lblRet;
}

/**
 * Searching state 2: select max number of result (500)
 *
 * @brief MCSProduct::initSearchState2
 * @return
 */
bool MCSProduct::initSearchState2()
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
		lPageSize.evaluateJavaScript(QLatin1String("this.onchange();"));

		lblRet = true;
	} while (false);

	return lblRet;
}

void MCSProduct::getProductListInfo()
{
	QWebElement			leElement, leNext;
	int					liIdx;
	DataEntry			ldata(mDbLstColName);
	QWebElementCollection	leLst;

	do
	{
		leElement = mpWebView->findFirstElement(QLatin1String("table"),
												QLatin1String("class"),
												QLatin1String("mcsResultsTable"));
		DEF_CHECK_BREAK(leElement, QLatin1String("Can not get mcsResultsTable element"));

		leLst = leElement.findFirst(QLatin1String("tbody"))
				.findAll(QLatin1String("tr"));
		for(liIdx = 0; liIdx < leLst.count(); liIdx++)
		{
			if(liIdx % 2 != 0)
				continue;

			if(parseData(leLst[liIdx], &ldata) == true)
				mlstData << ldata;
		}

		qDebug() << QString("Extracted %1").arg(mlstData.count());
		DEF_LOG(QString("Extracted %1").arg(mlstData.count()));

		if (mpExtFileLoading->getExtractLink() == true)
		{
			qDebug() << "Save to xml: " << mpXMLUtil->saveData(&mlstData);
			mlstData.clear();
		}

		// Go to next result page
		leLst = mpWebView->findAll(QLatin1String("a[class=\"ismPaginatorArrow\"]"));
		for(liIdx = 0; liIdx < leLst.count(); liIdx++)
		{
			leNext = leLst[liIdx];
			if(leNext.toPlainText() == QLatin1String(">"))
				break;
		}

		if(liIdx == leLst.count())
		{
			qDebug() << "go to the end of search result";
			if (mpExtFileLoading->getExtractLink() == true)
			{
				mpXMLUtil->close();
				showFinishMsg(QLatin1String("Finished extract all links..."));
				break;
			}

//			// start to extract detail
//			if(startExtractDetail() == false)
//			{
//				// Finished
//				showFinishMsg();
//			}
//			break;
		}

		qDebug() << "Go to next page";

		mpWebView->doClick(&leNext);
	} while(false);
}

//bool MCSProduct::startExtractDetail()
//{
//	DataEntry	ldata;
//	bool lblRet = false;

//	do
//	{
//		if(mlstData.isEmpty())
//			break;

//		setStatus(QString("Remain %1 items").arg(mlstData.count()));

//		meState = E_STATE_GET_DETAIL_INFO;
//		ldata = mlstData.at(0);
//		loadPage(ldata.getProperty(mDbLstColName[eURL]));

//		lblRet = true;
//	} while(false);

//	return lblRet;
//}


/**
 * Extract search result items
 *
 * @brief MCSProduct::extractSearchResult
 */
void MCSProduct::extractSearchResult()
{
	QWebElement lElement;
	QWebElementCollection lCol;

	do
	{
		lElement = mpWebView->findFirstElement(QLatin1String("table"),
											   QLatin1String("class"),
											   QLatin1String("mcsResultsTable"));
		DEF_CHECK_BREAK(lElement, QLatin1String("Can not find mcsResultsTable entry"));

		lCol = mpWebView->findAllElement(QLatin1String("a"),
										 QLatin1String("class"),
										 QLatin1String("mcsButtonLink"),
										 &lElement);
		foreach (lElement, lCol)
			mlstDataLink << mstrBaseRootURL + lElement.attribute("href");

		qDebug() << "Save " << mlstDataLink.count() << " items";
		DEF_LOG(QString("Save %2 items").arg(mlstDataLink.count()));
	} while (false);
}

/**
 * Parse detail data
 *
 * @brief MCSProduct::getDetailData
 */
void MCSProduct::getDetailData()
{
	QWebElement		leElement;
	QString			lstrTmp;
	QWebElementCollection	leLst;

	do
	{
		leElement = mpWebView->document().findFirst("div[class=\"mcsColumnsTwoOne\"]");
		DEF_CHECK_BREAK(leElement, "Can not get data column element");

		lstrTmp = "Name: " + leElement.toPlainText();

		Utilities::extractAndUpdate(&mCurrentEntry, mDbLstColName[eProName], lstrTmp, "Name: ", "\nTechnology");
		Utilities::extractAndUpdate(&mCurrentEntry, mDbLstColName[eProductType], lstrTmp, "Technology:");
		Utilities::extractAndUpdate(&mCurrentEntry, mDbLstColName[eProModel], lstrTmp, "Models:");

		leLst = leElement.findAll("a");
		foreach(leElement, leLst)
			if(leElement.toPlainText() == "More Info")
			{
				mCurrentEntry.updateProperty(mDbLstColName[eWebsite], leElement.attribute("href"));
				break;
			}

		Utilities::extractAndUpdate(&mCurrentEntry, mDbLstColName[eDescription], lstrTmp, "About this Product\n");

		mFinalDataList << mCurrentEntry;

		if(mFinalDataList.count() == 20)
		{
			mpDBLogger->logDataList(mFinalDataList);
			DEF_LOG(QString("Save data: %1").arg(mFinalDataList.count()));
			mFinalDataList.clear();
		}

		if(gotoNextSavedEntry() == false)
		{
			if(mFinalDataList.count() > 0)
			{
				mpDBLogger->logDataList(mFinalDataList);
				DEF_LOG(QString("Save data: %1").arg(mFinalDataList.count()));
				mFinalDataList.clear();
			}

			showFinishMsg();
		}
	} while(false);
}

/**
 * Parse web data to Data Entry object
 *
 * @brief MCSProduct::parseData
 * @param aTR
 * @param apData
 * @return
 */
bool MCSProduct::parseData(const QWebElement &aTR, DataEntry *apData)
{
	bool			lblRet = false;
	QWebElement		leElement;
	QString			lstrTmp;
	QWebElementCollection	leLst;

	do
	{
		if(aTR.isNull() || apData == 0)
		{
			qDebug() << QLatin1String("parseData, invalide input");
			DEF_LOG(QLatin1String("parseData, invalide input"));
			break;
		}

		leLst = aTR.findAll(QLatin1String("td"));
		if(leLst.count() != 6)
		{
			qDebug() << QLatin1String("parseData, invalid td element");
			DEF_LOG(QLatin1String("parseData, invalid td element"));
			break;
		}

		lstrTmp = leLst[1].toPlainText();
		if (lstrTmp.indexOf("...") > 0)
			lstrTmp.clear();
		apData->updateProperty(mDbLstColName[eProName], lstrTmp);
		apData->updateProperty(mDbLstColName[eManufacturer], leLst[2].toPlainText());
		apData->updateProperty(mDbLstColName[eProductType], leLst[3].toPlainText());

		lstrTmp = leLst[4].toPlainText();
		if (lstrTmp.indexOf("...") > 0)
			lstrTmp.clear();
		apData->updateProperty(mDbLstColName[eProModel], lstrTmp);
		apData->updateProperty(mDbLstColName[eCerNo], leLst[5].toPlainText()
				.replace("More Info", ""));

		leElement = leLst[5].findFirst(QLatin1String("a"));
		DEF_CHECK_BREAK(leElement, QLatin1String("Can not find the product link element"));

		QUrl url(leElement.attribute(QLatin1String("href")));
#ifndef WDE_QT5
		lstrTmp = url.queryItemValue(QLatin1String("ID"));
#else
		QUrlQuery urlQuery(url);
		lstrTmp = urlQuery.queryItemValue(QLatin1String("ID"));
#endif

		if(lstrTmp.isEmpty() == true)
		{
			qDebug() << QLatin1String("Gotten invalid product key");
			DEF_LOG(QLatin1String("Gotten invalid product key"));
			break;
		}
		apData->updateProperty(mDbLstColName[eKey], lstrTmp);

//		apData->updateProperty(mDbLstColName[eURL], mstrBaseRootURL + leElement.attribute("href"));
		apData->updateProperty(mDbLstColName[eURL], mpWebView->getHrefURL(&leElement));

		lblRet = true;
	} while(false);

	return lblRet;
}

/**
 * Debuging loaded data list. Print to csv format.
 *
 * @brief MCSProduct::debugDataList
 */
void MCSProduct::debugDataList()
{
	do
	{
		QString path = QLatin1String("datalist.csv");
		QFile file(path);

		if (file.open(QFile::Append | QFile::Text) == false)
		{
			qDebug() << QLatin1String("Can not open file: ")
					 << path;
			break;
		}

		QTextStream pCSV(&file);

		if (mSavedDataList.isEmpty() == true)
		{
			pCSV << QLatin1String("Data list is empty");
			break;
		}

		foreach (DataEntry entry, mSavedDataList)
		{
			pCSV << entry.getCSVString();
			pCSV << QLatin1Char('\n');
		}

		file.close();
	} while (false);
}

/**
 * @brief MCSProduct::loadSavedData
 * @param path
 * @return
 */
bool MCSProduct::loadSavedData(QString path)
{
	bool lblRet = false;
	DATA_LIST lTmpList;
	QMap<QString, DataEntry> lMap;

	do
	{
		lblRet = mpXMLUtil->loadData(&lTmpList, path);
		if (lblRet == false)
			break;

		qDebug() << QLatin1String("Loaded from xml: ")
				 << lTmpList.size();

		foreach (DataEntry entry, lTmpList)
			lMap.insert(entry.getProperty(mDbLstColName[eKey]), entry);

		mSavedDataList.clear();
		mSavedDataList.append(lMap.values());

		qDebug() << QLatin1String("After filter: ") << mSavedDataList.size();

		debugDataList();

		lblRet = true;
	} while (false);

	return lblRet;
}

/**
 * Go to next saved entry
 *
 * @brief MCSProduct::gotoNextSavedEntry
 * @return
 */
bool MCSProduct::gotoNextSavedEntry()
{
	bool lblRet = false;

	do
	{
		if (mSavedDataList.isEmpty() == true)
		{
			showFinishMsg(QLatin1String("Saved data list is empty!"));
			break;
		}

		do
		{
			mCurrentEntry = mSavedDataList.takeFirst();
		} while (mCurrentEntry.isEmpty() == true &&
				 mSavedDataList.isEmpty() == false);

		if (mCurrentEntry.isEmpty() == true)
		{
			showFinishMsg(QLatin1String("All remained entries are empty!"));
			break;
		}

		setStatus(QString("Remain: %1").arg(mSavedDataList.count()));

		meState = E_STATE_GET_DETAIL_INFO;
		loadPage(mCurrentEntry.getProperty(mDbLstColName[eURL]));
		lblRet = true;
	} while (false);

	return lblRet;
}
