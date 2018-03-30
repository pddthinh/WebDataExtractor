#include "noeljones.h"

noeljones::noeljones(QString url)
	:real_estate(url, 0)
{
}

void noeljones::doWebLoadFinished()
{
	switch (meState)
	{
	case E_STATE_INIT:
		getOfficeList();
		break;

	case E_STATE_GET_SUB_CATEGORY:
		getOfficeInfo();
		break;

	case E_STATE_GET_DETAIL_INFO:
		parseData();
		break;

	default:
		break;
	}
}

void noeljones::getOfficeList()
{
	QString url = mpWebView->url().toString();

	do
	{
		mElement = mpWebView->getElementById("menu-offices");
		DEF_CHECK_BREAK(mElement, QString("Can not find menu-offices: %1")
						.arg(url));

		mCol = mElement.findAll("a");
		foreach (mElement, mCol)
			mlstCategoryLink << mpWebView->getHrefURL(&mElement);
	} while (false);

	qDebug() << "Found offices: " << mlstCategoryLink.count();
	if (gotoNextCategoryLink() == false)
		showFinishMsg("Goto next category is fail");
}

void noeljones::getOfficeInfo()
{
	QString lstrTmp;
	DataEntry ldata(mDbLstColName);
	bool lblGoToNext = true;


	// Save office contact
	do
	{
		if (itemURL().indexOf("offices") < 0)
			break;

		mElement = mpWebView->getElementById("page-quote");
		DEF_CHECK_BREAK(mElement, QString("Can not find page-quote: %1")
						.arg(itemURL()));

		lstrTmp = mElement.lastChild().toPlainText() + "\n";

		ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
		ldata.updateProperty(mDbLstColName[eItemLink], itemURL());

		Utilities::extractAndUpdate(&ldata, mDbLstColName[ePhone], lstrTmp, "Phone: ");
		Utilities::extractAndUpdate(&ldata, mDbLstColName[eMobile], lstrTmp, "Mobile: ");
		Utilities::extractAndUpdate(&ldata, mDbLstColName[eFax], lstrTmp, "Fax: ");
		ldata.updateProperty(mDbLstColName[eEmail], Utilities::extractEmailAddress(lstrTmp).at(0));

		lstrTmp.replace("Phone: " + ldata.getProperty(mDbLstColName[ePhone]), "");
		lstrTmp.replace("Mobile: " + ldata.getProperty(mDbLstColName[eMobile]), "");
		lstrTmp.replace("Fax: " + ldata.getProperty(mDbLstColName[eFax]), "");
		lstrTmp.replace(ldata.getProperty(mDbLstColName[eEmail]), "");
		ldata.updateProperty(mDbLstColName[eAddress], lstrTmp);

		mElement = mpWebView->findFirstElement("h1", "class", "entry-title lettering");
		updateData(&ldata, eName, &mElement);

		mlstData << ldata;
		saveData(&mlstData, true);
	} while (false);

	// Goto staff page
	do
	{
		lstrTmp = itemURL();
		if (lstrTmp.indexOf("offices") < 0)
			break;

		lstrTmp.replace("offices", "staff");
		lblGoToNext = false;
		loadPage(lstrTmp);
	} while (false);

	// Collect agents links
	do
	{
		if (itemURL().indexOf("staff") < 0)
			break;

		mElement = mpWebView->findFirstDiv("class", "inner");
		mCol = mElement.findAll("a");
		foreach (mElement, mCol)
		{
			lstrTmp = mpWebView->getHrefURL(&mElement);
			if (lstrTmp.indexOf("staff") >= 0)
				mlstDataLink << lstrTmp;
		}
	} while (false);

	if (lblGoToNext == true)
	{
		if (gotoNextCategoryLink() == false)
		{
			if (gotoNextDataLink() == false)
				showFinishMsg("Can not goto next datalink");
		}
	}
}

void noeljones::parseData()
{
	QWebElement lelement;
	QString lstrTmp;
	DataEntry ldata(mDbLstColName);

	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], itemURL());

	do
	{
		mElement = mpWebView->getElementById("staff-details");
		DEF_CHECK_BREAK(mElement, QString("Can not get staff-details: %1")
						.arg(itemURL()));

		lelement = mpWebView->findFirstElement("h1", "class", "entry-title lettering", &mElement);
		updateData(&ldata, eName, &lelement);

		lelement = mElement.findFirst("h3");
		updateData(&ldata, eBusinessName, &lelement);

		lstrTmp = mElement.toPlainText() + "\n";

		Utilities::extractAndUpdate(&ldata, mDbLstColName[ePhone], lstrTmp, "T:");
		Utilities::extractAndUpdate(&ldata, mDbLstColName[eMobile], lstrTmp, "M:");
		Utilities::extractAndUpdate(&ldata, mDbLstColName[eEmail], lstrTmp, "E:");

		mlstData << ldata;
	} while (false);

	saveData();

	if (gotoNextDataLink() == false)
	{
		saveData(&mlstData, true);
		showFinishMsg();
	}
}
