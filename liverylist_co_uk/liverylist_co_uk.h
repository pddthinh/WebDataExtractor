#ifndef liverylist_co_uk_H
#define liverylist_co_uk_H

#include "WDEInclude.h"

#include <QDebug>
#include <QWebElement>
#include <QWebElementCollection>

using namespace nsBaseWebEngine;

class liverylist_co_uk : public BaseWebEngine
{
	Q_OBJECT

public:
	liverylist_co_uk(QWidget *apParent = 0);

protected:
	void doStart();
	void doStop(){}
	void doWebLoadFinished();
	void initDatabase();
	void initExtWidget() {
		mpExtFileLoading = new ExtFileLoading(this, mpAppPref);
	}

private:
	void parseLiveryProfile();
	void parseProfileDetail();
};

#endif // liverylist_co_uk_H
