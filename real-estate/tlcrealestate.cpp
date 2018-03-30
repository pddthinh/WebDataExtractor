#include "tlcrealestate.h"
#include <QDebug>

tlcrealestate::tlcrealestate(QString url, QWidget *apParent)
	: real_estate(url, apParent)
{
	setWindowTitle("tlcrealestate");
}

void tlcrealestate::doWebLoadFinished()
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

void tlcrealestate::parseData()
{
	DataEntry ldata(mDbLstColName);

	ldata.updateProperty(mDbLstColName[eName], "Trent Lloyd");
	ldata.updateProperty(mDbLstColName[eBusinessName], "Principal-Sales-Property Management-Auctioneer");
	ldata.updateProperty(mDbLstColName[ePhone], "0407 813 983");
	ldata.updateProperty(mDbLstColName[eEmail], "trent@tlcrealestate.com.au");
	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	mlstData << ldata;

	ldata.clearData();
	ldata.updateProperty(mDbLstColName[eName], "Kirsty Donaldson");
	ldata.updateProperty(mDbLstColName[eBusinessName], "Admin Assitant");
	ldata.updateProperty(mDbLstColName[ePhone], "0467 801 911");
	ldata.updateProperty(mDbLstColName[eEmail], "kirsty@tlcrealestate.com.au");
	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	mlstData << ldata;

	ldata.clearData();
	ldata.updateProperty(mDbLstColName[eName], "Cathy Reed");
	ldata.updateProperty(mDbLstColName[ePhone], "0467 801 911");
	ldata.updateProperty(mDbLstColName[eEmail], "cathy@tlcrealestate.com.au");
	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	mlstData << ldata;

	mpDBLogger->logDataList(mlstData);
	DEF_LOG(QString("Save %1").arg(mlstData.count()));
	mlstData.clear();

	showFinishMsg();
	logEndSite();
}
