#include "garypeer.h"

garypeer::garypeer(QString url, QWidget *apParent)
	:real_estate(url, apParent)
{
}

void garypeer::doWebLoadFinished()
{
	switch (meState)
	{
	case E_STATE_INIT:
		getProfileList();
		break;

	case E_STATE_GET_DETAIL_INFO:
		parseData();
		break;

	default:
		break;
	}
}

void garypeer::getProfileList()
{
	mCol = mpWebView->findAllElement("a", "class", "view-property");
	foreach (mElement, mCol)
		mlstDataLink << mpWebView->getHrefURL(&mElement);

	qDebug() << "Found data link: " << mlstDataLink.count();

	if (gotoNextDataLink() == false)
	{
		// parse office contact
		parseSiteContact();
		showFinishMsg();
	}
}

void garypeer::parseData()
{
	QWebElement lelement;
	DataEntry ldata(mDbLstColName);
	QString lstrTmp;
	QString url;

	do
	{
		url = mpWebView->url().toString();
		mElement = mpWebView->getElementById("profile");
		DEF_CHECK_BREAK(mElement, QString("Can not find profile element: %1")
						.arg(url));

		mElement = mElement.findFirst("div[class=\"right\"]");
		DEF_CHECK_BREAK(mElement, QString("Can not find data element: %1")
						.arg(url));

		ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
		ldata.updateProperty(mDbLstColName[eItemLink], url);

		lelement = mElement.findFirst("h2");
		updateData(&ldata, eName, &lelement);

		lelement = mElement.findFirst("h4");
		updateData(&ldata, eBusinessName, &lelement);

		lelement = mElement.findFirst("a[class=\"office-no\"]");
		updateData(&ldata, ePhone, &lelement);

		lelement = mElement.findFirst("a[class=\"mob-no\"]");
		updateData(&ldata, eMobile, &lelement);

		lelement = mElement.findFirst("a[class=\"mail\"]");
		lstrTmp.clear();
		if (lelement.isNull() == false)
			lstrTmp = lelement.attribute("href").replace("mailto:", "");
		ldata.updateProperty(mDbLstColName[eEmail], lstrTmp);

		mlstData << ldata;
	} while (false);

	saveData();

	if (gotoNextDataLink() == false)
	{
		saveData(&mlstData, true);

		parseSiteContact();
		showFinishMsg();
	}
}

void garypeer::parseSiteContact()
{
	DataEntry ldata(mDbLstColName);
	QString url = "http://www.garypeer.com.au/contact/";

	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], url);
	ldata.updateProperty(mDbLstColName[eName], "GaryPeer Sales Office");
	ldata.updateProperty(mDbLstColName[eAddress], "348 Orrong Road, Caulfield North. VIC 3161");
	ldata.updateProperty(mDbLstColName[ePhone], "03 9526 1999");
	ldata.updateProperty(mDbLstColName[eFax], "03 9527 7289");
	ldata.updateProperty(mDbLstColName[eEmail], "reception@garypeer.com.au");
	mlstData << ldata;

	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], url);
	ldata.updateProperty(mDbLstColName[eName], "GaryPeer Sales Office");
	ldata.updateProperty(mDbLstColName[eAddress], "55 Inkerman Street, St Kilda. VIC 3182");
	ldata.updateProperty(mDbLstColName[ePhone], "03 9066 4688");
	ldata.updateProperty(mDbLstColName[eFax], "03 9066 4666");
	ldata.updateProperty(mDbLstColName[eEmail], "stkildareception@garypeer.com.au");
	mlstData << ldata;

	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], url);
	ldata.updateProperty(mDbLstColName[eName], "GaryPeer Rentals Office");
	ldata.updateProperty(mDbLstColName[eAddress], "55 Inkerman Street, St Kilda. VIC 3182");
	ldata.updateProperty(mDbLstColName[ePhone], "03 9066 4688");
	ldata.updateProperty(mDbLstColName[eFax], "03 9066 4666");
	ldata.updateProperty(mDbLstColName[eEmail], "rentalreception@garypeer.com.au");
	mlstData << ldata;

	saveData(&mlstData, true);
}
