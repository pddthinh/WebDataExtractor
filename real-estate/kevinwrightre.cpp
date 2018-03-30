#include "kevinwrightre.h"

kevinwrightre::kevinwrightre(QString url)
	:real_estate(url, 0)
{
}

void kevinwrightre::doWebLoadFinished()
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

void kevinwrightre::parseData()
{
	DataEntry ldata(mDbLstColName);

	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], itemURL());
	ldata.updateProperty(mDbLstColName[eName], "Kevin Wright");
	ldata.updateProperty(mDbLstColName[eAddress], "72 Main Street, Mornington VIC 3931");
	ldata.updateProperty(mDbLstColName[eBusinessName], "Commercial & Business Sales Specialists");
	ldata.updateProperty(mDbLstColName[eMobile], "0417 564 454");
	ldata.updateProperty(mDbLstColName[eEmail], "kevin@kevinwrightre.com.au");
	mlstData << ldata;
	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], itemURL());
	ldata.updateProperty(mDbLstColName[eName], "Russell Murphy");
	ldata.updateProperty(mDbLstColName[eMobile], "0407 839 184");
	ldata.updateProperty(mDbLstColName[eEmail], "russell@kevinwrightre.com.au");
	mlstData << ldata;
	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], itemURL());
	ldata.updateProperty(mDbLstColName[eName], "Gary Ralph ");
	ldata.updateProperty(mDbLstColName[eMobile], "0418 535 503");
	ldata.updateProperty(mDbLstColName[eEmail], "gary@kevinwrightre.com.au");
	mlstData << ldata;
	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], itemURL());
	ldata.updateProperty(mDbLstColName[eName], "Tanya Scagliarini");
	ldata.updateProperty(mDbLstColName[eMobile], "0438 289 859");
	ldata.updateProperty(mDbLstColName[eEmail], "tanya@kevinwrightre.com.au");
	mlstData << ldata;
	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], itemURL());
	ldata.updateProperty(mDbLstColName[eName], "Lina Luppino");
	ldata.updateProperty(mDbLstColName[eBusinessName], "Residential Sales Specialists");
	ldata.updateProperty(mDbLstColName[eMobile], "0419 571 583");
	ldata.updateProperty(mDbLstColName[eEmail], "lina@kevinwrightre.com.au");
	mlstData << ldata;
	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], itemURL());
	ldata.updateProperty(mDbLstColName[eName], "Louise Varigos");
	ldata.updateProperty(mDbLstColName[eMobile], "0408 885 982 ");
	ldata.updateProperty(mDbLstColName[eEmail], "louise@kevinwrightre.com.au");
	mlstData << ldata;

	saveData(&mlstData, true);
	showFinishMsg();
}
