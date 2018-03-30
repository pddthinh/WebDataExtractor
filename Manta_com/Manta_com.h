#ifndef manta_com_H
#define manta_com_H

#include "WDEInclude.h"

#include <QDebug>
#include <QTimer>
#include <QWebElement>
#include <QWebElementCollection>

using namespace nsBaseWebEngine;

class manta_com : public BaseWebEngine
{
	Q_OBJECT

public:
	manta_com(QWidget *apParent = 0);

protected:
	void doStart();
	void doStop(){}
	void doWebLoadFinished();
	void initDatabase();
	void initExtWidget(){}

private:
	QStringList		mlstState;
	int				miCurState;

	QStringList		mlstKeyword;
	int				miCurKeyword;

	QTimer			*mpTimer;

private Q_SLOTS:
	void sltWaitTimeout();

private:
	bool loadStateList();
	bool loadKeywordList();

	void buildSearchLink();

	bool gotoNextState();

	void getCityLinkList();
	void parseCityInfo();
	void parseDetailData();

	QString parseOwnerName(QWebElement *apElement);
};

#endif // manta_com_H
