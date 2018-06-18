#ifndef thekennelclub_org_uk3_H
#define thekennelclub_org_uk3_H

#include "WDEInclude.h"

#include <QDebug>
#include <QWebElement>
#include <QWebElementCollection>

using namespace nsBaseWebEngine;

class thekennelclub_org_uk3 : public BaseWebEngine
{
	Q_OBJECT

public:
	thekennelclub_org_uk3(QWidget *apParent = 0);

protected:
	void doStart();
	void doStop(){}
	void doWebLoadFinished();
	void initDatabase();
	void initExtWidget() {
		mpExtFileLoading = new ExtFileLoading(this, mpAppPref);
	}

private:
	void getBreederList();
	void getBreederInAllArea();
	void getBreederListItemsInAllArea();
	void getBreederDetailData();
};

#endif // thekennelclub_org_uk3_H
