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
	QStringList mBreederIds;

private:
	void loadList();
	void getBreederIdList();
	bool gotoNextBreeder(ENU_STATE state = E_STATE_GET_SUB_CATEGORY);
	void getBreederLinks();
};

#endif // breedersonline_co_uk_H
