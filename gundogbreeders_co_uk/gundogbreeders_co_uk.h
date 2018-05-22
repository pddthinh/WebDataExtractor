#ifndef gundogbreeders_co_uk_H
#define gundogbreeders_co_uk_H

#include "WDEInclude.h"

#include <QDebug>
#include <QWebElement>
#include <QWebElementCollection>

using namespace nsBaseWebEngine;

class gundogbreeders_co_uk : public BaseWebEngine
{
	Q_OBJECT

public:
	gundogbreeders_co_uk(QWidget *apParent = 0);

protected:
	void doStart();
	void doStop(){}
	void doWebLoadFinished();
	void initDatabase();
	void initExtWidget() {
		mpExtFileLoading = new ExtFileLoading(this, mpAppPref);
	}

private:
	void getAllCountry();
	void getAllCounty();
	void getAllBreeder();
	void getDetailBreeder();

private:
	QStringList listCountry;
	QStringList listCounty;
};

#endif // gundogbreeders_co_uk_H
