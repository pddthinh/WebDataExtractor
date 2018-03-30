#include "blackwaterrealestate.h"

blackwaterrealestate::blackwaterrealestate(QString url)
	:real_estate(url, 0)
{
}

void blackwaterrealestate::doWebLoadFinished()
{
	if (meState == E_STATE_INIT)
		parseData();
}

void blackwaterrealestate::parseData()
{
	DataEntry ldata(mDbLstColName);

	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], itemURL());
	ldata.updateProperty(mDbLstColName[eAddress], "15 Mackenzie Street, Blackwater QLD 4720");

	ldata.updateProperty(mDbLstColName[eName], "Jason Campbell");
	ldata.updateProperty(mDbLstColName[eBusinessName], "Principal / Auctioneer");
	ldata.updateProperty(mDbLstColName[eMobile], "0418 799 693");
	ldata.updateProperty(mDbLstColName[eEmail], "Jason@emeraldrealestate.com.au");
	mlstData << ldata;

	ldata.updateProperty(mDbLstColName[eName], "Joy Fernie");
	ldata.updateProperty(mDbLstColName[eBusinessName], "Property Consultant");
	ldata.updateProperty(mDbLstColName[eMobile], "0419 772 178");
	ldata.updateProperty(mDbLstColName[eEmail], "jfernie@blackwater.net.au");
	mlstData << ldata;

	ldata.updateProperty(mDbLstColName[eName], "Rashana Smith");
	ldata.updateProperty(mDbLstColName[eBusinessName], "Sales Administration");
	ldata.updateProperty(mDbLstColName[eMobile], "");
	ldata.updateProperty(mDbLstColName[ePhone], "(07) 4982 7599");
	ldata.updateProperty(mDbLstColName[eEmail], "sales@blackwater.net.au");
	mlstData << ldata;

	ldata.updateProperty(mDbLstColName[eName], "Ruth Poletto");
	ldata.updateProperty(mDbLstColName[eBusinessName], "Property Manager");
	ldata.updateProperty(mDbLstColName[eMobile], "");
	ldata.updateProperty(mDbLstColName[ePhone], "(07) 4982 7599");
	ldata.updateProperty(mDbLstColName[eEmail], "rpoletto@blackwater.net.au");
	mlstData << ldata;

	ldata.updateProperty(mDbLstColName[eName], "Penelope Bazzo");
	ldata.updateProperty(mDbLstColName[eBusinessName], "Property Manager");
	ldata.updateProperty(mDbLstColName[eMobile], "");
	ldata.updateProperty(mDbLstColName[ePhone], "(07) 4987 7600");
	ldata.updateProperty(mDbLstColName[eEmail], "rentals@blackwater.net.au");
	mlstData << ldata;

	ldata.updateProperty(mDbLstColName[eName], "Amanda Richardson");
	ldata.updateProperty(mDbLstColName[eBusinessName], "The Rock & Administration");
	ldata.updateProperty(mDbLstColName[eMobile], "");
	ldata.updateProperty(mDbLstColName[ePhone], "(07) 4982 7599");
	ldata.updateProperty(mDbLstColName[eEmail], "admin@blackwater.net.au");
	mlstData << ldata;

	ldata.updateProperty(mDbLstColName[eName], "Heidi Jai Goodwin");
	ldata.updateProperty(mDbLstColName[eBusinessName], "Reception");
	ldata.updateProperty(mDbLstColName[eMobile], "");
	ldata.updateProperty(mDbLstColName[ePhone], "(07) 4982 7599");
	ldata.updateProperty(mDbLstColName[eEmail], "reception@blackwater.net.au");
	mlstData << ldata;

	saveData(&mlstData, true);
	showFinishMsg();
}
