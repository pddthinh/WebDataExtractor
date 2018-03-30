#include "crawfordrealty.h"

crawfordrealty::crawfordrealty(QString url)
	:real_estate(url, 0)
{
	setWebLoadTimeout(10000);
}

void crawfordrealty::doWebLoadFinished()
{
	switch (meState)
	{
	case E_STATE_INIT:
		getAgentLink();
		break;

	case E_STATE_GET_DETAIL_INFO:
		parseData();
		break;

	default:
		break;
	}
}

void crawfordrealty::getAgentLink()
{
	mlstDataLink << "http://www.crawfordrealty.com.au/team.aspx?ag=hd"
				 << "http://www.crawfordrealty.com.au/team.aspx?ag=kr"
				 << "http://www.crawfordrealty.com.au/team.aspx?ag=nw";

	qDebug() << "Found agent: " << mlstDataLink.count();
	if (gotoNextDataLink() == false)
		showFinishMsg("Can not goto next data link");
}

void crawfordrealty::parseData()
{
	QWebElement lelement;
	QWebElementCollection lCol;
	QString lstrTmp;
	DataEntry ldata(mDbLstColName);

	lCol = mpWebView->findAll("td[valign=top][width=\"11%\"][align=center]");
	foreach (lelement, lCol)
	{
		lelement = lelement.nextSibling().findFirst("tbody");
		mCol = lelement.findAll("tr");
		if (mCol.count() < 3)
			continue;

		ldata.clearData();
		ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
		ldata.updateProperty(mDbLstColName[eItemLink], itemURL());

		mElement = mCol.at(0);
		lstrTmp = mElement.toPlainText().replace("\240", " ");
		ldata.updateProperty(mDbLstColName[eName], lstrTmp);

		mElement = mCol.at(2);
		updateData(&ldata, eBusinessName, &mElement);

		lstrTmp = lelement.toPlainText() + "\n";

		Utilities::extractAndUpdate(&ldata, mDbLstColName[eMobile], lstrTmp, "Mobile:");
		Utilities::extractAndUpdate(&ldata, mDbLstColName[eEmail], lstrTmp, "Email:");

		mlstData << ldata;
	}

	saveData(&mlstData, true);
	if (gotoNextDataLink() == false)
	{
		getOfficeContact();
		showFinishMsg();
	}
}

void crawfordrealty::getOfficeContact()
{
	DataEntry ldata(mDbLstColName);

	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], itemURL());
	ldata.updateProperty(mDbLstColName[eName], "Crawford Realty - Hedland Office");
	ldata.updateProperty(mDbLstColName[eAddress], "Shop 2/2 Byass Street, South Hedland WA 6722");
	ldata.updateProperty(mDbLstColName[ePhone], "(08) 9172 5300");
	ldata.updateProperty(mDbLstColName[eFax], "(08) 9172 5311");
	ldata.updateProperty(mDbLstColName[eEmail], "info@crawfordrealty.com.au");
	mlstData << ldata;
	ldata.updateProperty(mDbLstColName[eName], "Crawford Realty - PM Department Manager");
	ldata.updateProperty(mDbLstColName[eAddress], "");
	ldata.updateProperty(mDbLstColName[ePhone], "");
	ldata.updateProperty(mDbLstColName[eFax], "");
	ldata.updateProperty(mDbLstColName[eEmail], "management@crawfordrealty.com.au");
	mlstData << ldata;
	ldata.updateProperty(mDbLstColName[eName], "Crawford Realty - Karratha Office");
	ldata.updateProperty(mDbLstColName[eAddress], "4/5 Warambie Road, PO Box 136, Karratha WA 6714");
	ldata.updateProperty(mDbLstColName[ePhone], "(08) 9143 1599");
	ldata.updateProperty(mDbLstColName[eFax], "(08) 9143 1699 ");
	ldata.updateProperty(mDbLstColName[eEmail], "info@crawfordrealtykarratha.com.au");
	mlstData << ldata;
	ldata.updateProperty(mDbLstColName[eName], "Crawford Realty - General Manager / Licensee");
	ldata.updateProperty(mDbLstColName[eAddress], "");
	ldata.updateProperty(mDbLstColName[ePhone], "");
	ldata.updateProperty(mDbLstColName[eFax], "");
	ldata.updateProperty(mDbLstColName[eEmail], "manager@crawfordrealtykarratha.com.au");
	mlstData << ldata;
	ldata.updateProperty(mDbLstColName[eName], "Crawford Realty - Pm Department");
	ldata.updateProperty(mDbLstColName[eAddress], "");
	ldata.updateProperty(mDbLstColName[ePhone], "");
	ldata.updateProperty(mDbLstColName[eFax], "");
	ldata.updateProperty(mDbLstColName[eEmail], "pm@crawfordrealtykarratha.com.au");
	mlstData << ldata;
	ldata.updateProperty(mDbLstColName[eName], "Crawford Realty - Leasing");
	ldata.updateProperty(mDbLstColName[eAddress], "");
	ldata.updateProperty(mDbLstColName[ePhone], "");
	ldata.updateProperty(mDbLstColName[eFax], "");
	ldata.updateProperty(mDbLstColName[eEmail], "leasing@crawfordrealtykarratha.com.au");
	mlstData << ldata;
	ldata.updateProperty(mDbLstColName[eName], "Crawford Realty - PM Accounts");
	ldata.updateProperty(mDbLstColName[eAddress], "");
	ldata.updateProperty(mDbLstColName[ePhone], "");
	ldata.updateProperty(mDbLstColName[eFax], "");
	ldata.updateProperty(mDbLstColName[eEmail], "accounts@crawfordrealtykarratha.com.au");
	mlstData << ldata;
	ldata.updateProperty(mDbLstColName[eName], "Crawford Realty - Newman Office");
	ldata.updateProperty(mDbLstColName[eAddress], "Shop 8B Newman Boulevard, Po Box 890, Newman WA, 6753");
	ldata.updateProperty(mDbLstColName[ePhone], "(08) 9175 1155");
	ldata.updateProperty(mDbLstColName[eFax], "(08) 9175 0055");
	ldata.updateProperty(mDbLstColName[eEmail], "info@crawfordrealtynewman.com.au");
	mlstData << ldata;
	ldata.updateProperty(mDbLstColName[eName], "Crawford Realty - General Manager / Licensee");
	ldata.updateProperty(mDbLstColName[eAddress], "");
	ldata.updateProperty(mDbLstColName[ePhone], "");
	ldata.updateProperty(mDbLstColName[eFax], "");
	ldata.updateProperty(mDbLstColName[eEmail], "manager@crawfordrealtynewman.com.au");
	mlstData << ldata;
	ldata.updateProperty(mDbLstColName[eName], "Crawford Realty - Pm Department");
	ldata.updateProperty(mDbLstColName[eAddress], "");
	ldata.updateProperty(mDbLstColName[ePhone], "");
	ldata.updateProperty(mDbLstColName[eFax], "");
	ldata.updateProperty(mDbLstColName[eEmail], "pm@crawfordrealtynewman.com.au");
	mlstData << ldata;
	ldata.updateProperty(mDbLstColName[eName], "Crawford Realty - PM Accounts");
	ldata.updateProperty(mDbLstColName[eAddress], "");
	ldata.updateProperty(mDbLstColName[ePhone], "");
	ldata.updateProperty(mDbLstColName[eFax], "");
	ldata.updateProperty(mDbLstColName[eEmail], "accounts@crawfordrealtynewman.com.au");
	mlstData << ldata;
	ldata.updateProperty(mDbLstColName[eName], "Crawford Realty - Community Liaison");
	ldata.updateProperty(mDbLstColName[eAddress], "");
	ldata.updateProperty(mDbLstColName[ePhone], "");
	ldata.updateProperty(mDbLstColName[eFax], "");
	ldata.updateProperty(mDbLstColName[eEmail], "community@crawfordrealtynewman.com.au");
	mlstData << ldata;

	saveData(&mlstData, true);
}
