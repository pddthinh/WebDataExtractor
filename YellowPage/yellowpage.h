#ifndef yellowpage_H
#define yellowpage_H

#include "WDEInclude.h"

#include <QDebug>
#include <QStringList>
#include <QWebElement>
#include <QWebElementCollection>

using namespace nsBaseWebEngine;

class yellowpage : public BaseWebEngine
{
	Q_OBJECT

public:
	yellowpage(QWidget *apParent = 0);

protected:
	void doStart();
	void doStop(){}
	void doWebLoadFinished();
	void initDatabase();
	void initExtWidget();

private:
	QStringList mlstUSState;
	int			miCurState;

	QStringList mlstKeyword;
	int			miCurKeyword;

private:
	bool loadUSStateList();
	bool gotoNextState();

	bool loadKeywordList();
	bool gotoNextKeyword();

	void inputSearchKeyword();

	void getDataList();
	bool gotoNextResultPage();

	void getDetailData();
};

#endif // yellowpage_H
