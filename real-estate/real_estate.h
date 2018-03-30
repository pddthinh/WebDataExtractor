#ifndef real_estate_H
#define real_estate_H

#include "WDEInclude.h"

#include <QDebug>
#include <QWebElement>
#include <QWebElementCollection>

#define LOG_SEPARATOR	"---------------------------------------------------------"

using namespace nsBaseWebEngine;

enum enuDatabase
{
	eName = 0,
	eBusinessName,
	ePhone, eMobile, eFax,
	eAddress, eEmail,
	eWebsite, eItemLink,
	eDatabaseMax
};

class real_estate : public BaseWebEngine
{
	Q_OBJECT

public:
	real_estate(QWidget *apParent = 0);
	real_estate(QString url, QWidget *apParent = 0);

protected:
	void doStart();
	void doStop(){}
	void doWebLoadFinished();
	void initDatabase();
	void initExtWidget()
	{
		//mpExtFileLoading = new ExtFileLoading(this);
	}

	void init(QString url);

protected:
	void logEndSite();
	QString itemURL();

protected:
	QString	mstrURL;
	QWebElement	mElement;
	QWebElementCollection mCol;
};

#endif // real_estate_H
