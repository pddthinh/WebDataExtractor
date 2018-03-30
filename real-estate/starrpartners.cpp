#include "starrpartners.h"

starrpartners::starrpartners(QString url)
	:real_estate(url, 0)
{
}

void starrpartners::doWebLoadFinished()
{
	switch (meState)
	{
	case E_STATE_INIT:
		getOfficeInfo();
		break;

	case E_STATE_GET_SUB_CATEGORY:
		getAgentList();
		break;

	case E_STATE_GET_DETAIL_INFO:
		parseData();
		break;

	case E_STATE_SUB1:
		getOfficeContact();
		break;

	default:
		break;
	}
}

void starrpartners::getOfficeInfo()
{
	QString lstrLink;

	mCol = mpWebView->findAll("a");
	foreach (mElement, mCol)
	{
		lstrLink = mpWebView->getHrefURL(&mElement);

		if (lstrLink.indexOf("/office/") > 0)
			mlstOffice << lstrLink;

		if (lstrLink.indexOf("/about/meet-the-team?agentID=") > 0)
			mlstCategoryLink << lstrLink;
	}

	mlstOffice.removeDuplicates();
	mlstCategoryLink.removeDuplicates();

	qDebug() << "Found office: " << mlstOffice.count();
	qDebug() << "Found agent group: " << mlstCategoryLink.count();

	if (gotoNextCategoryLink() == false)
		showFinishMsg("Can not goto category link");
}

void starrpartners::getAgentList()
{
	QWebElement lelement;

	mCol = mpWebView->findAllDiv("class", "staff-list");
	foreach (mElement, mCol)
	{
		lelement = mElement.findFirst("a");
		mlstDataLink << mpWebView->getHrefURL(&lelement);
	}

//	while (mlstCategoryLink.count() > 2)
//		mlstCategoryLink.takeFirst();

	if (gotoNextCategoryLink() == false)
	{
		mlstDataLink.removeDuplicates();
		qDebug() << "Found agent: " << mlstDataLink.count();

		if (gotoNextDataLink() == false)
			showFinishMsg("Can not goto next data link!");
	}
}

void starrpartners::parseData()
{
	DataEntry ldata(mDbLstColName);
	QWebElement lelement;
	QString lstrTmp;

	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], itemURL());

	do
	{
		mElement = mpWebView->getElementById("content-layout");
		DEF_CHECK_BREAK(mElement, QString("Can not find content-layout: %1")
						.arg(itemURL()));

		lelement = mElement.findFirst("h2");
		updateData(&ldata, eName, &lelement);

		lelement = mElement.findFirst("h3");
		updateData(&ldata, eBusinessName, &lelement);

		lelement = mpWebView->findFirstDiv("class", "summary", &mElement);
		lstrTmp = lelement.toPlainText() + "\n";

		Utilities::extractAndUpdate(&ldata, mDbLstColName[eMobile], lstrTmp, "Mobile:");
		Utilities::extractAndUpdate(&ldata, mDbLstColName[ePhone], lstrTmp, "Phone:");
		Utilities::extractAndUpdate(&ldata, mDbLstColName[eEmail], lstrTmp, "Email:");

		if (ldata.getProperty(mDbLstColName[ePhone]).isEmpty())
			Utilities::extractAndUpdate(&ldata, mDbLstColName[ePhone], lstrTmp, "Office:");

		mlstData << ldata;
	} while (false);

	saveData();

	if (gotoNextDataLink() == false)
	{
		saveData(&mlstData, true);

		// start get office contact
		if (gotoNextOffice() == false)
			showFinishMsg("Can not goto next office");
	}
}

bool starrpartners::gotoNextOffice()
{
	bool lblRet = false;

	do
	{
		if (mlstOffice.isEmpty())
			break;

		meState = E_STATE_SUB1;

		loadPage(mlstOffice.takeFirst());
		lblRet = true;
	} while (false);

	return lblRet;
}

void starrpartners::getOfficeContact()
{
	DataEntry ldata(mDbLstColName);
	QWebElement lelement;
	QString lstrTmp;

	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], itemURL());

	do
	{
		mElement = mpWebView->findFirstDiv("class", "content office-details");
		DEF_CHECK_BREAK(mElement, QString("Can not find office data: %1")
						.arg(itemURL()));

		lelement = mElement.findFirst("h2");
		updateData(&ldata, eName, &lelement);

		lelement = mpWebView->findFirstDiv("class", "summary", &mElement);
		lstrTmp = lelement.toPlainText() + "\n";

		Utilities::extractAndUpdate(&ldata, mDbLstColName[eAddress], lstrTmp, "Address:", "Phone:");
		Utilities::extractAndUpdate(&ldata, mDbLstColName[ePhone], lstrTmp, "Phone:", "Fax:");
		Utilities::extractAndUpdate(&ldata, mDbLstColName[eFax], lstrTmp, "Fax:", "Email:");
		Utilities::extractAndUpdate(&ldata, mDbLstColName[eEmail], lstrTmp, "Email:");

		lstrTmp = ldata.getProperty(mDbLstColName[eAddress]).replace("NSW", ", NSW");
		ldata.updateProperty(mDbLstColName[eAddress], lstrTmp);

		mlstData << ldata;
	} while (false);

	saveData();

	if (gotoNextOffice() == false)
	{
		saveData(&mlstData, true);
		showFinishMsg();
	}
}
