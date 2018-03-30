#include "doylespillane.h"

doylespillane::doylespillane(QString url)
	:real_estate(url, 0)
{
}

void doylespillane::doWebLoadFinished()
{
	switch (meState)
	{
	case E_STATE_INIT:
		parseData();
		break;

	default:
		break;
	}
}

void doylespillane::getOfficeContact()
{
	DataEntry ldata(mDbLstColName);

	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], itemURL());
	ldata.updateProperty(mDbLstColName[eName], "Doyle Spillance Real Estate");
	ldata.updateProperty(mDbLstColName[eAddress], "761 Pittwater Road, Dee Why 2099");
	ldata.updateProperty(mDbLstColName[ePhone], "02 9981 3799");
	ldata.updateProperty(mDbLstColName[eFax], "02 9971 2371");
	ldata.updateProperty(mDbLstColName[eEmail], "info@doylespillane.com.au");
	mlstData << ldata;

	saveData(&mlstData, true);
}

void doylespillane::parseData()
{
	QWebElement lelement;
	DataEntry ldata(mDbLstColName);
	QString lstrTmp;

	mCol = mpWebView->findAllElement("li", "class", "big");
	foreach (mElement, mCol)
	{
		ldata.clearData();

		ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
		ldata.updateProperty(mDbLstColName[eItemLink], itemURL());

		lelement = mElement.findFirst("h3");
		updateData(&ldata, eName, &lelement);

		lelement = mElement.findFirst("h5");
		updateData(&ldata, eBusinessName, &lelement);

		lstrTmp = mElement.toPlainText() + "\n";

		Utilities::extractAndUpdate(&ldata, mDbLstColName[eEmail], lstrTmp, "E-mail:");
		Utilities::extractAndUpdate(&ldata, mDbLstColName[ePhone], lstrTmp, "Telephone:");
		Utilities::extractAndUpdate(&ldata, mDbLstColName[eMobile], lstrTmp, "Mobile:");

		mlstData << ldata;

		saveData();
	}

	saveData(&mlstData, true);
	getOfficeContact();
	showFinishMsg();
}
