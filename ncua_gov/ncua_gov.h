#ifndef ncua_gov_H
#define ncua_gov_H

#include "WDEInclude.h"

#include <QDebug>
#include <QWebElement>
#include <QWebElementCollection>

using namespace nsBaseWebEngine;

class ncua_gov : public BaseWebEngine
{
	Q_OBJECT

public:
	ncua_gov(QWidget *apParent = 0);

protected:
	void doStart();
	void doStop(){}
	void doWebLoadFinished();
	void initDatabase();
	void initExtWidget(){}

private:
	void doSearch();
	void getCharterNumber();
	void gotoNextResultPage();

	void startParseDetail();
	void parseDetailInfo();

private:
	QStringList mlstCharterNum;
};

#endif // ncua_gov_H
