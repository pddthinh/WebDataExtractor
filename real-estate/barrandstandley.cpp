#include "barrandstandley.h"

barrandstandley::barrandstandley(QString url)
	:real_estate(url, 0)
{
}

void barrandstandley::doWebLoadFinished()
{
	switch (meState)
	{
	case E_STATE_INIT:
		getAgentProfile();
		break;

	case E_STATE_GET_DETAIL_INFO:
		parseData();
		break;

	default:
		break;
	}
}

void barrandstandley::getOfficeContact()
{
	DataEntry ldata(mDbLstColName);

	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], itemURL());
	ldata.updateProperty(mDbLstColName[eName], "Barr and Standley Real Estate");
	ldata.updateProperty(mDbLstColName[eAddress], "19 Stirling Street, Bunbury, Western Australia, 6231");
	ldata.updateProperty(mDbLstColName[ePhone], "(08) 9721 1144 ");
	ldata.updateProperty(mDbLstColName[eFax], "(08) 9721 9372 ");
	ldata.updateProperty(mDbLstColName[eEmail], "sales@barrandstandley.com.au");
	mlstData << ldata;

	saveData(&mlstData, true);
}

void barrandstandley::getAgentProfile()
{
	QString lstrLink;

	mCol = mpWebView->findAll("a");
	foreach(mElement, mCol)
	{
		lstrLink = mpWebView->getHrefURL(&mElement);
		if (lstrLink.indexOf("sales-agent-profile.php?agent=") < 0)
			continue;

		mlstDataLink << lstrLink;
	}

	mlstDataLink.removeDuplicates();
	qDebug() << "Profile: " << mlstDataLink.count();

	if (gotoNextDataLink() == false)
		showFinishMsg("Unknown error");
}

void barrandstandley::parseData()
{
	QString lstrTmp;
	QWebElement lelement;
	DataEntry ldata(mDbLstColName);

	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], itemURL());

	do
	{
		mElement = mpWebView->getElementById("agent-details");
		DEF_CHECK_BREAK(mElement, QString("Can not get agent detail: %1")
						.arg(itemURL()));

		lelement = mElement.findFirst("h1");
		updateData(&ldata, eName, &lelement);

		lelement = mElement.findFirst("h2");
		updateData(&ldata, eBusinessName, &lelement);

		lstrTmp = mElement.toPlainText() + "\n";

		Utilities::extractAndUpdate(&ldata, mDbLstColName[eMobile], lstrTmp, "Mobile:");
		Utilities::extractAndUpdate(&ldata, mDbLstColName[eEmail], lstrTmp, "Email:");

		mlstData << ldata;
	} while (false);

	saveData();

	if (gotoNextDataLink() == false)
	{
		saveData(&mlstData, true);

		getOfficeContact();

		showFinishMsg();
	}
}
