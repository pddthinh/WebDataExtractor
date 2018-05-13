#ifndef thekennelclub_org_uk_H
#define thekennelclub_org_uk_H

#include "WDEInclude.h"

#include <QDebug>
#include <QWebElement>
#include <QWebElementCollection>

using namespace nsBaseWebEngine;

class thekennelclub_org_uk : public BaseWebEngine
{
	Q_OBJECT

public:
	thekennelclub_org_uk(QWidget *apParent = 0);

protected:
	void doStart();
	void doStop(){}
	void initDatabase();
	void initExtWidget() {
		mpExtFileLoading = new ExtFileLoading(this, mpAppPref);
	}
	void doWebLoadFinished();

private:
	void getBreederList();
	void getBreederInAllArea();
	void getBreederListItemsInAllArea();

	void getBreederDetailInfo();
};

#endif // thekennelclub_org_uk_H
