#ifndef CENTURY21_H
#define CENTURY21_H

#include "WDEInclude.h"

#include <QDebug>
#include <QWebElement>
#include <QWebElementCollection>

using namespace nsBaseWebEngine;

class Century21 : public BaseWebEngine
{
public:
	Century21(QWidget *apParent = 0);

protected:
	void doStart();
	void doStop(){}
	void doWebLoadFinished();
	void initDatabase();
	void initExtWidget(){}

protected:
	void getListState();
	void getMainDataList();
	void getData();
};

#endif // CENTURY21_H
