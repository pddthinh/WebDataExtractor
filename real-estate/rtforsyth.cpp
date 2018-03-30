#include "rtforsyth.h"

rtforsyth::rtforsyth(QString url)
	:real_estate(url, 0)
{
}

void rtforsyth::doWebLoadFinished()
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

void rtforsyth::parseData()
{
	DataEntry ldata(mDbLstColName);
	QWebElement lelement;
	QString lstrTmp;

	mElement = mpWebView->findFirstElement("ul", "class", "teamList");
	mCol = mElement.findAll("li");
	foreach (mElement, mCol)
	{
		ldata.clearData();

		ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
		ldata.updateProperty(mDbLstColName[eItemLink], itemURL());

		lelement = mElement.findFirst("small");
		updateData(&ldata, eBusinessName, &lelement);

		lelement = mElement.findFirst("h4");
		lstrTmp = lelement.toPlainText()
				  .replace(ldata.getProperty(mDbLstColName[eBusinessName]), "");
		ldata.updateProperty(mDbLstColName[eName], lstrTmp);

		lstrTmp = mElement.toPlainText() + "\n";

		Utilities::extractAndUpdate(&ldata, mDbLstColName[ePhone], lstrTmp, "Telephone:");
		Utilities::extractAndUpdate(&ldata, mDbLstColName[eMobile], lstrTmp, "Mobile:");
		Utilities::extractAndUpdate(&ldata, mDbLstColName[eEmail], lstrTmp, "E-mail:");

		mlstData << ldata;
		saveData();
	}

	saveData(&mlstData, true);
	showFinishMsg();
}
