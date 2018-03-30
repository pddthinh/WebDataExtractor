#ifndef find_a_vet_H
#define find_a_vet_H

#include "WDEInclude.h"

#include <QDebug>
#include <QWebElement>
#include <QWebElementCollection>

using namespace nsBaseWebEngine;

class find_a_vet : public BaseWebEngine
{
	Q_OBJECT

public:
	find_a_vet(QWidget *apParent = 0);

protected:
	void doStart();
	void doStop(){}
	void doWebLoadFinished();
	void initDatabase();
	void initExtWidget() {
		mpExtFileLoading = new ExtFileLoading(this, mpAppPref);
	}

private:
	void wrapperPageLoadFinished();
	void getLocationSearchResult();
	void doWaitFinished();

	void getVetDetail();

	// 2017-04-13
	void getCountyList();
};

#endif // find_a_vet_H
