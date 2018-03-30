#ifndef ciga_co_uk_H
#define ciga_co_uk_H

#include "WDEInclude.h"

#include <QDebug>
#include <QWebElement>
#include <QWebElementCollection>

using namespace nsBaseWebEngine;

class ciga_co_uk : public BaseWebEngine
{
	Q_OBJECT

public:
	ciga_co_uk(QWidget *apParent = 0);

protected:
	void doStart();
	void doStop(){}
	void doWebLoadFinished();
	void initDatabase();
	void initExtWidget(){}

private:
	void getRegionLink();
	void getDetailData();
};

#endif // ciga_co_uk_H
