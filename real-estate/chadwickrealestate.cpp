#include "chadwickrealestate.h"

chadwickrealestate::chadwickrealestate(QString url)
	:real_estate(url, 0)
{
}

void chadwickrealestate::doWebLoadFinished()
{
	if (meState == E_STATE_INIT)
		parseData();
}

void chadwickrealestate::getOfficeContact()
{
	DataEntry ldata(mDbLstColName);

	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], itemURL());
	ldata.updateProperty(mDbLstColName[eName], "Roseville Office");
	ldata.updateProperty(mDbLstColName[eAddress], "102 Pacific Highway, Roseville 2069");
	ldata.updateProperty(mDbLstColName[ePhone], "9416 1133");
	ldata.updateProperty(mDbLstColName[eFax], "9415 8747");
	ldata.updateProperty(mDbLstColName[eEmail], "realestate@chadwickrealestate.com.au");
	mlstData << ldata;
}

void chadwickrealestate::parseData()
{
	QWebElement lelement;
	DataEntry ldata(mDbLstColName);
	QString lstrTmp;

	mCol = mpWebView->findAllDiv("class", "two column agentFeature");
	foreach (mElement, mCol)
	{
		ldata.clearData();
		ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
		ldata.updateProperty(mDbLstColName[eItemLink], itemURL());

		lelement = mElement.findFirst("small");
		updateData(&ldata, eBusinessName, &lelement);

		lstrTmp = mElement.findFirst("h2").toPlainText();
		lstrTmp.replace(ldata.getProperty(mDbLstColName[eBusinessName]), "");
		ldata.updateProperty(mDbLstColName[eName], lstrTmp);

		lstrTmp = mElement.toPlainText() + "\n";

		Utilities::extractAndUpdate(&ldata, mDbLstColName[ePhone], lstrTmp, "Office\n");
		Utilities::extractAndUpdate(&ldata, mDbLstColName[eMobile], lstrTmp, "Mobile\n");
		Utilities::extractAndUpdate(&ldata, mDbLstColName[eEmail], lstrTmp, "E-mail\n");

		mlstData << ldata;
		saveData();
	}

	getOfficeContact();
	saveData(&mlstData, true);

	showFinishMsg();
}
