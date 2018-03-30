#include "whitepage_com.h"

#define BASEURL "http://www.whitepages.com/business?key=surgery+center&where=NJ"

whitepage_com::whitepage_com(QWidget *apParent)
	:BaseWebEngine(apParent)
{
	setWindowTitle("whitepage.com");
	setBaseURL(BASEURL);

	if(mpDBLogger != NULL)
		mpDBLogger->setLogPath("whitepage.sql");
}

void whitepage_com::doStart()
{

}

void whitepage_com::doStop()
{

}

void whitepage_com::doWebLoadFinished()
{

}

enum enuDatabase
{
	eName = 0,
	eAddress,
	eCity,
	ePhone,
	eEmail,
	eWebsite
};

void whitepage_com::initDatabase()
{
	mDbLstColName << "Name"
				  << "Address"
				  << "City"
				  << "Phone"
				  << "Email"
				  << "Website";
	mDbLstKey << eName << eEmail;
	mDbTableName = "whitepage";
}

WebEngine_Main(whitepage_com)
