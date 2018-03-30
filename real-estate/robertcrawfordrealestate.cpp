#include "robertcrawfordrealestate.h"

robertcrawfordrealestate::robertcrawfordrealestate(QString url)
	:real_estate(url, 0)
{
}

void robertcrawfordrealestate::doWebLoadFinished()
{
	if (meState == E_STATE_INIT)
		parseData();
}

void robertcrawfordrealestate::parseData()
{
	DataEntry ldata(mDbLstColName);
	QWebElement lelement;
	QString lstrTmp;

	mCol = mpWebView->findAll("td[align=left][valign=top][style=\"padding-left:20px;\"]");
	foreach (mElement, mCol)
	{
		ldata.clearData();
		ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
		ldata.updateProperty(mDbLstColName[eItemLink], itemURL());

		lelement = mpWebView->findFirstDiv("class", "colortwoheadingcolor mainheadingsmall zoom", &mElement);
		updateData(&ldata, eName, &lelement);

		lelement = mElement.findFirst("strong");
		updateData(&ldata, eBusinessName, &lelement);

		lstrTmp = mElement.toPlainText() + "\n";

		Utilities::extractAndUpdate(&ldata, mDbLstColName[ePhone], lstrTmp, "T:");
		Utilities::extractAndUpdate(&ldata, mDbLstColName[eMobile], lstrTmp, "M:");
		Utilities::extractAndUpdate(&ldata, mDbLstColName[eEmail], lstrTmp, "E:");

		mlstData << ldata;
		saveData();
	}

	// office contact
	ldata.clearData();
	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], "http://www.robertcrawfordrealestate.com.au/index.cfm?pagecall=contact&MenuItemID=27446&/search/pageid=Contact%20Us");
	ldata.updateProperty(mDbLstColName[eName], "Robert Crawford Real Estate - Sales Team");
	ldata.updateProperty(mDbLstColName[eAddress], "Shop 33 Elermore Shopping Centre, Elermore Vale, NSW 2287");
	ldata.updateProperty(mDbLstColName[ePhone], "4955 7888");
	ldata.updateProperty(mDbLstColName[eFax], "4955 7899");
	ldata.updateProperty(mDbLstColName[eEmail], "sales@robertcrawfordrealestate.com.au");
	mlstData << ldata;

	ldata.updateProperty(mDbLstColName[eName], "Robert Crawford Real Estate - Sales Team");
	ldata.updateProperty(mDbLstColName[eAddress], "3/71 Regent Street, New Lambton, NSW 2305");
	ldata.updateProperty(mDbLstColName[ePhone], "(02) 4957 6166  ");
	ldata.updateProperty(mDbLstColName[eFax], "(02)4957 6188  ");
	ldata.updateProperty(mDbLstColName[eEmail], "amanda@robertcrawfordrealestate.com.au");
	mlstData << ldata;

	ldata.updateProperty(mDbLstColName[eName], "Robert Crawford Real Estate - Property Management Team");
	ldata.updateProperty(mDbLstColName[eAddress], "3/71 Regent Street, New Lambton, NSW 2305");
	ldata.updateProperty(mDbLstColName[ePhone], "4957 6166  ");
	ldata.updateProperty(mDbLstColName[eFax], " 4957 6188  ");
	ldata.updateProperty(mDbLstColName[eEmail], "reception@robertcrawfordrealestate.com.au");
	mlstData << ldata;

	ldata.updateProperty(mDbLstColName[eName], "Robert Crawford Real Estate - Administration Team");
	ldata.updateProperty(mDbLstColName[eAddress], "3/71 Regent Street, New Lambton, NSW 2305");
	ldata.updateProperty(mDbLstColName[ePhone], "(02) 4957 6166  ");
	ldata.updateProperty(mDbLstColName[eFax], "");
	ldata.updateProperty(mDbLstColName[eEmail], "reception@robertcrawfordrealestate.com.au");
	mlstData << ldata;

	saveData(&mlstData, true);
	showFinishMsg();
}
