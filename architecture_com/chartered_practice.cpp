#include "chartered_practice.h"

#define BASEURL "https://members.architecture.com/directory/default.asp?dir=1"
#define DATA_FILE "chartered-practice-links.txt"

chartered_practice::chartered_practice(QWidget *apParent)
	: architecture_base(QLatin1String("UK Directory of RIBA Chartered Practices"),
						apParent)
{
	setBaseURL(BASEURL);
	mpLinkLogger = new Logger(DATA_FILE);
}

void chartered_practice::doStart()
{
	do
	{
		if (mpExtFileLoading->getExtractLink() == true)
		{
			// extract data only
			meState = E_STATE_INIT;
			miPageTotal = -1;
			miPageIdx = 1;

			loadPage(BASEURL);
			break;
		}

		mlstDataLink = Utilities::loadTextData(mpExtFileLoading->getLoadPath());
		int count = 0;
		int skipped = mpExtFileLoading->getNumberOfSkipItem();
		while (count < skipped)
		{
			mlstDataLink.removeFirst();
			count++;
		}

		mlstDataLink.removeDuplicates();
		if (gotoNextDataLink() == false)
			showFinishMsg("Data link is empty");
	} while (false);
}

void chartered_practice::doWebLoadFinished()
{
	switch(meState)
	{
	case E_STATE_INIT:
		searchFirm();
		break;

	case E_STATE_GET_MAIN_CATEGORY:
		getFirmDetailLink();
		break;

	case E_STATE_GET_DETAIL_INFO:
		getDetailData(true);
		break;

	default:
		qDebug() << "default";
		break;
	}
}

void chartered_practice::searchFirm()
{
	// select Town/county
	mElement = mpWebView->getElementById("AddType");
	mpWebView->changeCmbSelectedElement(&mElement, 1);

	// select RIBA Region
	mElement = mpWebView->getElementById("attribute1");
	mCol = mElement.findAll("option");
	int size = mCol.count();
	if (size > 0)
	{
		mCol[0].removeAttribute("selected");
		for (int i = 1; i < size; i++)
			mCol[i].setAttribute("selected", "true");
	}

	// press button
	meState = E_STATE_GET_MAIN_CATEGORY;
	mElement = mpWebView->getElementById("submit1");
	mpWebView->doClick(&mElement);
}

void chartered_practice::getFirmDetailLink()
{
	QString lstrTmp;

	if (miPageTotal == -1)
	{
		// Get the total search result
		miPageTotal = getTotalPage();
		DEF_LOG(QString("Total page: %1")
				.arg(miPageTotal));
	}

	mCol = mpWebView->findAllElement("a", "class", "text");
	foreach (mElement, mCol)
	{
		lstrTmp = mpWebView->getHrefURL(&mElement);
		if (lstrTmp.indexOf("dir_projects.asp?id=") < 0)
			continue;

		lstrTmp.replace("dir_projects.asp", "dir_details.asp");
		mlstDataLink << lstrTmp;
	}

	if (mlstDataLink.count() > 0)
	{
		mpLinkLogger->logStringList(mlstDataLink);
		DEF_LOG(QString("Save links: %1")
				.arg(mlstDataLink.count()));
		mlstDataLink.clear();
	}

	// goto next result page
	if (gotoNextResultPage() == false)
		showFinishMsg("Extract data link finished!");
}

//void chartered_practice::getDetailData()
//{
//	QString lstrTmp;
//	QWebElement leTmp, leCheck;
//	DataEntry ldata(mDbLstColName);
//	TAG_ADDRESS ltAdd;
//	TAG_PERSON_NAME ltName;

//	mElement = mpWebView->getElementById("ribacontent");

//	ldata.updateProperty(mDbLstColName[eItemLink], getURL());

//	leTmp = mElement.findFirst("h4");
//	updateData(&ldata, eName, &leTmp);

//	mElement = mpWebView->getElementById("form1");
//	mCol = mElement.findAll("tr");
//	int i;
//	int size = mCol.count();

//	for (i = 0; i < size; i++)
//	{
//		leTmp = mCol[i];

//		switch (i)
//		{
//		case 0:
//		{
//			// check data
//			leCheck = mCol[i].findFirst("img[title=\"Address\"]");
//			if (leCheck.isNull() == true)
//				break;

//			lstrTmp = leTmp.toPlainText();
//			ltAdd = Utilities::parseUK_Add(lstrTmp, false);
//			ldata.updateProperty(mDbLstColName[eAdd], ltAdd.strAddress);
//			ldata.updateProperty(mDbLstColName[eCountry], ltAdd.strCountry);
//			ldata.updateProperty(mDbLstColName[eCounty], ltAdd.strCounty);
//			ldata.updateProperty(mDbLstColName[eFullAdd], ltAdd.strFullAddress);
//			ldata.updateProperty(mDbLstColName[ePostCode], ltAdd.strPostcode);
//			ldata.updateProperty(mDbLstColName[eTown], ltAdd.strTown);
//			break;
//		}

//		case 1:
//		{
//			// check data
//			leCheck = mCol[i].findFirst("img[title=\"Telephone\"]");
//			if (leCheck.isNull() == true)
//				break;

//			updateData(&ldata, eTel, &leTmp);
//			break;
//		}

//		case 2:
//		{
//			// check data
//			leCheck = mCol[i].findFirst("img[title=\"Fax\"]");
//			if (leCheck.isNull() == true)
//				break;

//			updateData(&ldata, eFax, &leTmp);
//			break;
//		}

//		case 3:
//		{
//			// check data
//			leCheck = mCol[i].findFirst("img[title=\"Email\"]");
//			if (leCheck.isNull() == true)
//				break;

//			updateData(&ldata, eEmail, &leTmp);
//			break;
//		}

//		case 4:
//		{
//			// check data
//			leCheck = mCol[i].findFirst("img[title=\"Website\"]");
//			if (leCheck.isNull() == true)
//				break;

//			updateData(&ldata, eWeb, &leTmp);
//			break;
//		}

//		case 5:
//		{
//			// check data
//			leCheck = mCol[i].findFirst("img[title=\"Contact\"]");
//			if (leCheck.isNull() == true)
//				break;

//			lstrTmp = leTmp.toPlainText();
//			lstrTmp.replace("Mr", "");
//			lstrTmp.replace("Ms", "");
//			ltName = Utilities::parsePersonName(lstrTmp);
//			ldata.updateProperty(mDbLstColName[eFirstname], ltName.strFirstname);
//			ldata.updateProperty(mDbLstColName[eLastname], ltName.strLastName);
//			ldata.updateProperty(mDbLstColName[eFullName], ltName.strFullname);
//			break;
//		}
//		} // switch
//	} // for

//	mlstData << ldata;

//	if (mlstData.count() == 20)
//	{
//		DEF_LOG(QString("Save: %1")
//				.arg(mlstData.count()));
//		mpDBLogger->logDataList(mlstData);
//		mlstData.clear();
//	}

//	if (gotoNextDataLink() == false)
//	{
//		if (mlstData.count() > 0)
//		{
//			DEF_LOG(QString("Save: %1")
//					.arg(mlstData.count()));
//			mpDBLogger->logDataList(mlstData);
//			mlstData.clear();
//		}

//		showFinishMsg("Finished extract data");
//	}
//}
