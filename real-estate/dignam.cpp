#include "dignam.h"

dignam::dignam(QString url)
	:real_estate(url, 0)
{
}

void dignam::doWebLoadFinished()
{
	if (meState == E_STATE_INIT)
		parseData();
}

void dignam::parseData()
{
	DataEntry ldata(mDbLstColName);

	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], itemURL());
	ldata.updateProperty(mDbLstColName[eAddress], "398 Lawrence Hargrave Drive, Thirroul");
	ldata.updateProperty(mDbLstColName[ePhone], "(02) 4267 5377");

	ldata.updateProperty(mDbLstColName[eName], "Dignam Real Estate");
	ldata.updateProperty(mDbLstColName[eBusinessName], "");
	ldata.updateProperty(mDbLstColName[eEmail], "enquiries@dignam.com.au ");
	mlstData << ldata;
	ldata.updateProperty(mDbLstColName[eName], "Adam McMahon");
	ldata.updateProperty(mDbLstColName[eBusinessName], "Director");
	ldata.updateProperty(mDbLstColName[eEmail], "adam@dignam.com.au");
	mlstData << ldata;
	ldata.updateProperty(mDbLstColName[eName], "Matt Dignam");
	ldata.updateProperty(mDbLstColName[eBusinessName], "Director");
	ldata.updateProperty(mDbLstColName[eEmail], "matt@dignam.com.au");
	mlstData << ldata;
	ldata.updateProperty(mDbLstColName[eName], "Dan Dignam");
	ldata.updateProperty(mDbLstColName[eBusinessName], "");
	ldata.updateProperty(mDbLstColName[eEmail], "dan@dignam.com.au");
	mlstData << ldata;
	ldata.updateProperty(mDbLstColName[eName], "Brendan Peacock");
	ldata.updateProperty(mDbLstColName[eBusinessName], "");
	ldata.updateProperty(mDbLstColName[eEmail], "brendan@dignam.com.au");
	mlstData << ldata;
	ldata.updateProperty(mDbLstColName[eName], "Rebecca Tuck");
	ldata.updateProperty(mDbLstColName[eBusinessName], "");
	ldata.updateProperty(mDbLstColName[eEmail], "bec@dignam.com.au");
	mlstData << ldata;
	ldata.updateProperty(mDbLstColName[eName], "Sue English");
	ldata.updateProperty(mDbLstColName[eBusinessName], "");
	ldata.updateProperty(mDbLstColName[eEmail], "accounts@dignam.com.au");
	mlstData << ldata;
	ldata.updateProperty(mDbLstColName[eName], "Sara Marcelli");
	ldata.updateProperty(mDbLstColName[eBusinessName], "");
	ldata.updateProperty(mDbLstColName[eEmail], "sara@dignam.com.au");
	mlstData << ldata;
	ldata.updateProperty(mDbLstColName[eName], "Aran Levin");
	ldata.updateProperty(mDbLstColName[eBusinessName], "");
	ldata.updateProperty(mDbLstColName[eEmail], "aran@dignam.com.au");
	mlstData << ldata;

	saveData(&mlstData, true);
}
