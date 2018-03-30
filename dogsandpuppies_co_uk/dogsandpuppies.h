#ifndef dogsandpuppies_H
#define dogsandpuppies_H

#include "WDEInclude.h"

#include <QDebug>
#include <QWebElement>
#include <QWebElementCollection>

using namespace nsBaseWebEngine;

class dogsandpuppies : public BaseWebEngine
{
	Q_OBJECT

public:
	dogsandpuppies(QWidget *apParent = 0);

protected:
	void doStart();
	void doStop(){}
	void doWebLoadFinished();
	void initDatabase();
	void initExtWidget() {
		mpExtFileLoading = new ExtFileLoading(this, mpAppPref);
	}

private:
	int miCatalog;

private:
	void extractBreeders();
	void extractDetail();
};

#endif // dogsandpuppies_H
