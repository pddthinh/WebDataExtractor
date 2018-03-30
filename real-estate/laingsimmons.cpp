#include "laingsimmons.h"

laingsimmons::laingsimmons(QString url)
	:real_estate(url, 0)
{
}

void laingsimmons::doWebLoadFinished()
{
	if (meState == E_STATE_INIT)
		parseData();
}

void laingsimmons::parseData()
{
	DataEntry ldata(mDbLstColName);

	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], itemURL());
	ldata.updateProperty(mDbLstColName[eAddress], "11/15 Castlereagh Street, Sydney, NSW 2000");
	ldata.updateProperty(mDbLstColName[eFax], "02 9223 7242");

	ldata.updateProperty(mDbLstColName[eName], "LEANNE PILKINGTON");
	ldata.updateProperty(mDbLstColName[eBusinessName], "General Manager");
	ldata.updateProperty(mDbLstColName[eMobile], "0413 444 847");
	ldata.updateProperty(mDbLstColName[ePhone], "9291 7000");
	ldata.updateProperty(mDbLstColName[eEmail], "leanne@lsre.com.au");
	mlstData << ldata;

	ldata.updateProperty(mDbLstColName[eName], "AMANDA WARD");
	ldata.updateProperty(mDbLstColName[eBusinessName], "Marketing & Communications Manager");
	ldata.updateProperty(mDbLstColName[eMobile], "0410 331 555");
	ldata.updateProperty(mDbLstColName[ePhone], "9291 7000");
	ldata.updateProperty(mDbLstColName[eEmail], "amanda.ward@lsre.com.au");
	mlstData << ldata;

	ldata.updateProperty(mDbLstColName[eName], "LISA PERKINS");
	ldata.updateProperty(mDbLstColName[eBusinessName], "Franchise & Auction Coordinator");
	ldata.updateProperty(mDbLstColName[eMobile], "");
	ldata.updateProperty(mDbLstColName[ePhone], "9291 7000");
	ldata.updateProperty(mDbLstColName[eEmail], "lisa.perkins@lsre.com.au");
	mlstData << ldata;

	ldata.updateProperty(mDbLstColName[eName], "DAVID SCHOLES");
	ldata.updateProperty(mDbLstColName[eBusinessName], "Chief Auctioneer");
	ldata.updateProperty(mDbLstColName[eMobile], "");
	ldata.updateProperty(mDbLstColName[ePhone], "");
	ldata.updateProperty(mDbLstColName[eEmail], "david@auctionworks.net.au");
	mlstData << ldata;

	ldata.updateProperty(mDbLstColName[eName], "RODNEY MENDES");
	ldata.updateProperty(mDbLstColName[eBusinessName], "Performance Coach");
	ldata.updateProperty(mDbLstColName[eMobile], "0425 291 640");
	ldata.updateProperty(mDbLstColName[ePhone], "");
	ldata.updateProperty(mDbLstColName[eEmail], "rodney.mendes@lsre.com.au");
	mlstData << ldata;

	ldata.updateProperty(mDbLstColName[eName], "KATHERINE MCNEILL");
	ldata.updateProperty(mDbLstColName[eBusinessName], "Accounts Administrator");
	ldata.updateProperty(mDbLstColName[eMobile], "");
	ldata.updateProperty(mDbLstColName[ePhone], "9291 7009");
	ldata.updateProperty(mDbLstColName[eEmail], "katherine.mcneill@lsre.com.au");
	mlstData << ldata;

	ldata.updateProperty(mDbLstColName[eName], "Laing+Simmons Corporation");
	ldata.updateProperty(mDbLstColName[eBusinessName], "");
	ldata.updateProperty(mDbLstColName[eMobile], "");
	ldata.updateProperty(mDbLstColName[ePhone], "02 9291 7000");
	ldata.updateProperty(mDbLstColName[eEmail], "enquiries@laingsimmons.com.au");
	mlstData << ldata;

	saveData(&mlstData, true);
	showFinishMsg();
}
