#include "magainagain.h"

magainagain::magainagain(QString url)
	:real_estate(url, 0)
{
}

void magainagain::doWebLoadFinished()
{
	if (meState == E_STATE_INIT)
		parseData();
}

void magainagain::parseData()
{
	DataEntry ldata(mDbLstColName);
	QWebElement lelement;
	QString lstrTmp;
	QWebElementCollection lCol;

	mCol = mpWebView->findAllDiv("class", "stdivLittle");
	foreach (mElement, mCol)
	{
		ldata.clearData();
		ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
		ldata.updateProperty(mDbLstColName[eItemLink], itemURL());

		lelement = mpWebView->findFirstElement("span", "class", "stname", &mElement);
		updateData(&ldata, eName, &lelement);

		lelement = mpWebView->findFirstElement("span", "class", "stposn", &mElement);
		updateData(&ldata, eBusinessName, &lelement);

		lCol = mpWebView->findAllElement("span", "class", "stphone", &mElement);
		if (lCol.count() >= 2)
		{
			lelement = lCol[1];
			updateData(&ldata, eMobile, &lelement);
		}
		if (lCol.count() >= 1)
		{
			lelement = lCol[0];
			updateData(&ldata, ePhone, &lelement);
		}

		lCol = mElement.findAll("a");
		foreach (lelement, lCol)
		{
			lstrTmp = lelement.attribute("href");
			if (lstrTmp.indexOf("mailto:") >= 0)
			{
				ldata.updateProperty(mDbLstColName[eEmail], lstrTmp.replace("mailto:", ""));
				break;
			}
		}

		mlstData << ldata;
		saveData();
	}

	// office contact
	ldata.clearData();
	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], "http://www.magainagain.com.au/index-tess1.php?ItemNo=1129");
	ldata.updateProperty(mDbLstColName[eName], "Ascot Park");
	ldata.updateProperty(mDbLstColName[eAddress], "771A MARION ROAD, ASCOT PARK. S.A. 5043");
	ldata.updateProperty(mDbLstColName[ePhone], "08 8277 1777");
	ldata.updateProperty(mDbLstColName[eFax], "08 8277 6629");
	ldata.updateProperty(mDbLstColName[eEmail], "more@magainagain.com.au");
	mlstData << ldata;

	ldata.updateProperty(mDbLstColName[eName], "Aberfoyle Park");
	ldata.updateProperty(mDbLstColName[eAddress], "2/1 SUMMERFORD ROAD, ABERFOYLE PARK. S.A. 5159");
	ldata.updateProperty(mDbLstColName[ePhone], "08 8370 5999");
	ldata.updateProperty(mDbLstColName[eFax], "08 8370 5464");
	ldata.updateProperty(mDbLstColName[eEmail], "more@magainagain.com.au");
	mlstData << ldata;

	ldata.updateProperty(mDbLstColName[eName], "Happy Valley");
	ldata.updateProperty(mDbLstColName[eAddress], "SHOP 4, HAPPY VALLEY SHOPPING CENTRE, KENIHANS ROAD, HAPPY VALLEY S.A. 5159");
	ldata.updateProperty(mDbLstColName[ePhone], "08 8322 6111");
	ldata.updateProperty(mDbLstColName[eFax], "08 8322 5522");
	ldata.updateProperty(mDbLstColName[eEmail], "more@magainagain.com.au");
	mlstData << ldata;

	ldata.updateProperty(mDbLstColName[eName], "Woodcroft ");
	ldata.updateProperty(mDbLstColName[eAddress], "SHOP 7A, WOODCROFT MARKET PLAZA, 217 PIMPALA ROAD, WOODCROFT SA 5162");
	ldata.updateProperty(mDbLstColName[ePhone], "08 8381 6000 ");
	ldata.updateProperty(mDbLstColName[eFax], "08 8381 6222");
	ldata.updateProperty(mDbLstColName[eEmail], "more@magainagain.com.au");
	mlstData << ldata;

	saveData(&mlstData, true);
	showFinishMsg();
}
