#ifndef nia_org_uk_H
#define nia_org_uk_H

#include "WDEInclude.h"

#include <QDebug>
#include <QWebElement>
#include <QWebElementCollection>

using namespace nsBaseWebEngine;

class nia_org_uk : public BaseWebEngine
{
	Q_OBJECT

public:
	nia_org_uk(QWidget *apParent = 0);

protected:
	void doStart();
	void doStop(){}
	void doWebLoadFinished();
	void initDatabase();
	void initExtWidget(){}

private:
	void getDataLink();
	void getDetailData();
};

#endif // nia_org_uk_H
