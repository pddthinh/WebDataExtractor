#ifndef gdorb_decc_gov_uk_H
#define gdorb_decc_gov_uk_H

#include "WDEInclude.h"

#include <QDebug>
#include <QWebElement>
#include <QWebElementCollection>

using namespace nsBaseWebEngine;

class gdorb_decc_gov_uk : public BaseWebEngine
{
	Q_OBJECT

public:
	gdorb_decc_gov_uk(QWidget *apParent = 0);

protected:
	void doStart();
	void doStop(){}
	void doWebLoadFinished();
	void initDatabase();
	void initExtWidget()
	{
		mpExtFileLoading = new ExtFileLoading(this);
	}

	void getMainList();
	void updatePageLimit();
	void getCompanyList();
	bool gotoNextResultPage();
	bool loadSavedData();
	void parseDetailData();
};

#endif // gdorb_decc_gov_uk_H
