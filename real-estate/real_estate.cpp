#include "real_estate.h"

#define BASEURL ""

real_estate::real_estate(QWidget *apParent)
	: BaseWebEngine(apParent)
{
	init(BASEURL);
}

real_estate::real_estate(QString url, QWidget *apParent)
	: BaseWebEngine(apParent)
{
	init(url);
}

void real_estate::init(QString url)
{
	mstrURL = url;
	setWindowTitle("real_estate");
	setBaseURL(mstrURL);

	if(mpDBLogger != NULL)
		mpDBLogger->setLogPath("real_estate.sql");

	mpLog->setFilePath("real_estate.txt");
	///////////////////////////////////////////////////////////
	DEF_LOG(LOG_SEPARATOR);
	DEF_LOG(QString("Start site: %1").arg(mstrURL));
}

void real_estate::initDatabase()
{
	mDbLstColName << "Name"
				  << "Business Name"
				  << "Phone" << "Mobile" << "Fax"
				  << "Address" << "Email"
				  << "Website" << "Item Link";
	mDbLstKey << eName << eEmail;
	mDbTableName = "real_estate";
}

void real_estate::doStart()
{
	meState = E_STATE_INIT;

	loadPage(mstrURL);
}

void real_estate::doWebLoadFinished()
{
	// child will implement this
}

void real_estate::logEndSite()
{
	DEF_LOG("End site");
	DEF_LOG(LOG_SEPARATOR);
}

QString real_estate::itemURL()
{
	return mpWebView->url().toString();
}
