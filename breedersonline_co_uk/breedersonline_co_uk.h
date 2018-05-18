#ifndef breedersonline_co_uk_H
#define breedersonline_co_uk_H

#include "WDEInclude.h"

#include <QDebug>
#include <QWebElement>
#include <QWebElementCollection>

using namespace nsBaseWebEngine;

class breedersonline_co_uk : public BaseWebEngine
{
	Q_OBJECT

public:
	breedersonline_co_uk(QWidget *apParent = 0);

protected:
	void doStart();
	void doStop(){}
	void doWebLoadFinished();
	void initDatabase();
	void initExtWidget() {
		mpExtFileLoading = new ExtFileLoading(this, mpAppPref);
	}

private:
	void loadList();
};

#endif // breedersonline_co_uk_H
