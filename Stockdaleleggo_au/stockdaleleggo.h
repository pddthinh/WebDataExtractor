#ifndef stockdaleleggo_H
#define stockdaleleggo_H

#include "WDEInclude.h"

#include <QDebug>
#include <QWebElement>
#include <QWebElementCollection>

using namespace nsBaseWebEngine;

class stockdaleleggo : public BaseWebEngine
{
public:
	stockdaleleggo(QWidget *apParent = 0);

protected:
	void doStart();
	void doWebLoadFinished();
	void initDatabase();
	void initExtWidget(){}
	void doStop(){}

protected:
	void getMainCategoryList();
	void getTeamInfo();
	void getDetailInfo();
};

#endif // stockdaleleggo_H
