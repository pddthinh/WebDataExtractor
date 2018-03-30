#include "chartered_member.h"

#define BASEURL "https://members.architecture.com/directory/default.asp?dir=3"
#define DATA_FILE "chartered-member-links.txt"

chartered_member::chartered_member(QWidget *apParent)
	: architecture_base(QLatin1String("RIBA Chartered Members Directory"),
						apParent)
{
	setBaseURL(BASEURL);
	mpLinkLogger = new Logger(DATA_FILE);
	mIdxPostCode = 0;
	miPageTotal = -1;
	miPageIdx = 1;

	// load postcode list
	mblPostCodeLoad = false;
}

void chartered_member::doStart()
{
	do
	{
		if (mpExtFileLoading->getExtractLink() == true)
		{
			meState = E_STATE_INIT;
			miPageTotal = -1;
			miPageIdx = 1;

			// load postcode
			loadPostCode();

			loadPage(BASEURL);
			break;
		}

		// load saved data
		int iSkip = mpExtFileLoading->getNumberOfSkipItem();
		mlstDataLink = Utilities::loadTextData(mpExtFileLoading->getLoadPath());
		while (iSkip > 0)
		{
			mlstDataLink.takeFirst();
			iSkip --;
		}

		mlstDataLink.removeDuplicates();

//		DEF_LOG_LIST(mlstDataLink, "");
//		showFinishMsg("Finished");

		meState = E_STATE_GET_DETAIL_INFO;
		loadPage(mlstDataLink.takeFirst());
	} while (false);
}

void chartered_member::doWebLoadFinished()
{
	switch (meState)
	{
	case E_STATE_INIT:
		doSearch();
		break;

	case E_STATE_GET_MAIN_CATEGORY:
		getDataLinkList();
		break;

	case E_STATE_GET_DETAIL_INFO:
		getDetailData();
		break;

	default:
		break;
	}
}

void chartered_member::loadPostCode()
{
	do
	{
		// skipped if already loaded
		if (mblPostCodeLoad == true)
			break;

		QString path = mpExtFileLoading->getLoadPath();
		if (path.isEmpty() == true)
			path = QLatin1String(":/Simple-UK-postcode");

		mlstPostCode = Utilities::loadTextData(path);

		mblPostCodeLoad = true;
	} while (false);
}

void chartered_member::doSearch()
{
	do
	{
		// select town/county
		mElement = mpWebView->getElementById("AddType");
		mpWebView->changeCmbSelectedElement(&mElement, 1);

		// update postcode
		if (gotoNextPostCode() == false)
		{
			showFinishMsg("Finished all postcodes");
			break;
		}

		// press search button
		meState = E_STATE_GET_MAIN_CATEGORY;
		mElement = mpWebView->getElementById("submit1");
		mpWebView->doClick(&mElement);
	} while (false);
}

bool chartered_member::gotoNextPostCode()
{
	bool lblRet = false;

	do
	{
		if (mlstPostCode.isEmpty() == true)
			break;

		setStatus(QString("Remain postcode: %1")
				  .arg(mlstPostCode.size()));
		DEF_LOG(QString("Postcode: %1")
				.arg(mlstPostCode[0]));
		mElement = mpWebView->getElementById("PostCode");
		mElement.setAttribute("value", mlstPostCode.takeFirst());

		lblRet = true;
	} while (false);

	return lblRet;
}

void chartered_member::getDataLinkList()
{
	QString lstrTmp;

	do
	{
		if (miPageTotal == -1)
		{
			miPageTotal = getTotalPage();
			DEF_LOG(QString("Total page: %1")
					.arg(miPageTotal));
		}

		mElement = mpWebView->getElementById("ribacontent");
		mCol = mpWebView->findAllElement("a", "class", "text", &mElement);
		foreach (mElement, mCol)
		{
			lstrTmp = mpWebView->getHrefURL(&mElement);
			if (lstrTmp.indexOf("dir_details.asp?id=") < 0)
				continue;

			mlstDataLink << lstrTmp;
		}

		if (mlstDataLink.count() > 0)
		{
			DEF_LOG(QString("Save datalink: %1")
					.arg(mlstDataLink.count()));
			mpLinkLogger->logStringList(mlstDataLink);
			mlstDataLink.clear();
		}

		if (gotoNextResultPage() == false)
			doStart();
	} while (false);
}
