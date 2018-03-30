#include "jennyandrews.h"

jennyandrews::jennyandrews(QString url, QWidget *apParent)
	:real_estate(url, apParent)
{
}

void jennyandrews::doWebLoadFinished()
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

void jennyandrews::parseData()
{
	DataEntry ldata(mDbLstColName);
	QString url = mpWebView->url().toString();

	ldata.updateProperty(mDbLstColName[eWebsite], url);
	ldata.updateProperty(mDbLstColName[eItemLink], url);
	ldata.updateProperty(mDbLstColName[eName], "Jenny Andrews");
	ldata.updateProperty(mDbLstColName[eBusinessName], "");
	ldata.updateProperty(mDbLstColName[ePhone], "0432 128 080");
	ldata.updateProperty(mDbLstColName[eEmail], "jennyandrews.sales@bigpond.com");
	mlstData << ldata;

	ldata.updateProperty(mDbLstColName[eWebsite], url);
	ldata.updateProperty(mDbLstColName[eItemLink], url);
	ldata.updateProperty(mDbLstColName[eName], "Luke Andrews");
	ldata.updateProperty(mDbLstColName[eBusinessName], "Sales Consultant");
	ldata.updateProperty(mDbLstColName[ePhone], "0422 945 710");
	ldata.updateProperty(mDbLstColName[eEmail], "lukeandrews.sales@bigpond.com");
	mlstData << ldata;

	ldata.updateProperty(mDbLstColName[eWebsite], url);
	ldata.updateProperty(mDbLstColName[eItemLink], url);
	ldata.updateProperty(mDbLstColName[eName], "Fiona Williams");
	ldata.updateProperty(mDbLstColName[eBusinessName], "Sales Consultant");
	ldata.updateProperty(mDbLstColName[ePhone], "0450 651 125");
	ldata.updateProperty(mDbLstColName[eEmail], "fiona.jennyandrews@bigpond.com");
	mlstData << ldata;

	ldata.updateProperty(mDbLstColName[eWebsite], url);
	ldata.updateProperty(mDbLstColName[eItemLink], url);
	ldata.updateProperty(mDbLstColName[eName], "Kezia Andrews");
	ldata.updateProperty(mDbLstColName[eBusinessName], "Property Manager");
	ldata.updateProperty(mDbLstColName[ePhone], "0406 118 391");
	ldata.updateProperty(mDbLstColName[eEmail], "kezandrews.admin@bigpond.com");
	mlstData << ldata;

	ldata.updateProperty(mDbLstColName[eWebsite], url);
	ldata.updateProperty(mDbLstColName[eItemLink], url);
	ldata.updateProperty(mDbLstColName[eName], "Janet Norman");
	ldata.updateProperty(mDbLstColName[eBusinessName], "Senior Property Manager");
	ldata.updateProperty(mDbLstColName[ePhone], " 0467 530 513");
	ldata.updateProperty(mDbLstColName[eEmail], "janet.jennyandrews@bigpond.com");
	mlstData << ldata;

	saveData(&mlstData, true);
	showFinishMsg();
}
