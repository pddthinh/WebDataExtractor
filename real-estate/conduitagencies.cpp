#include "conduitagencies.h"

conduitagencies::conduitagencies(QString url)
	:real_estate(url, 0)
{
}

void conduitagencies::doWebLoadFinished()
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

void conduitagencies::parseData()
{
	DataEntry ldata(mDbLstColName);

	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], "http://www.conduitagencies.com/main.php?page=contactus");
	ldata.updateProperty(mDbLstColName[eName], "Conduit Agencies");
	ldata.updateProperty(mDbLstColName[eAddress], "Shop 14 / 90 - 110 Surf Parade, Broadbeach Gold Coast Australia, QLD 4218");
	ldata.updateProperty(mDbLstColName[ePhone], "+61755922884");
	ldata.updateProperty(mDbLstColName[eFax], "+61755923788");
	ldata.updateProperty(mDbLstColName[eEmail], "admin@conduitagencies.com");
	mlstData << ldata;

	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], itemURL());
	ldata.updateProperty(mDbLstColName[eName], "Tory Hirst");
	ldata.updateProperty(mDbLstColName[eBusinessName], "Sales Consultant");
	ldata.updateProperty(mDbLstColName[ePhone], "07 55922884");
	ldata.updateProperty(mDbLstColName[eMobile], "0402417260");
	ldata.updateProperty(mDbLstColName[eEmail], "tory@conduitagencies.com");
	mlstData << ldata;

	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], itemURL());
	ldata.updateProperty(mDbLstColName[eName], "Matt Conduit");
	ldata.updateProperty(mDbLstColName[eBusinessName], "Director");
	ldata.updateProperty(mDbLstColName[ePhone], "0755922884");
	ldata.updateProperty(mDbLstColName[eMobile], "0418741949");
	ldata.updateProperty(mDbLstColName[eEmail], "matt@conduitagencies.com");
	mlstData << ldata;

	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], itemURL());
	ldata.updateProperty(mDbLstColName[eName], "Ross Lukin");
	ldata.updateProperty(mDbLstColName[eBusinessName], "");
	ldata.updateProperty(mDbLstColName[ePhone], "07 5592 2884");
	ldata.updateProperty(mDbLstColName[eMobile], "0419555022");
	ldata.updateProperty(mDbLstColName[eEmail], "ross@conduitagencies.com");
	mlstData << ldata;

	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], itemURL());
	ldata.updateProperty(mDbLstColName[eName], "Amanda Conduit");
	ldata.updateProperty(mDbLstColName[eBusinessName], "");
	ldata.updateProperty(mDbLstColName[ePhone], "07 5592 2884");
	ldata.updateProperty(mDbLstColName[eMobile], " 0413734343");
	ldata.updateProperty(mDbLstColName[eEmail], "amanda@conduitagencies.com");
	mlstData << ldata;

	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], itemURL());
	ldata.updateProperty(mDbLstColName[eName], "Simon Worthington");
	ldata.updateProperty(mDbLstColName[eBusinessName], "");
	ldata.updateProperty(mDbLstColName[ePhone], "07 5592 2884");
	ldata.updateProperty(mDbLstColName[eMobile], "0412 145 551");
	ldata.updateProperty(mDbLstColName[eEmail], "simon@conduitagencies.com");
	mlstData << ldata;

	saveData(&mlstData, true);

	showFinishMsg();
}
