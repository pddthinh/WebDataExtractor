#ifndef defaqto_com_H
#define defaqto_com_H

#include "WDEInclude.h"

#include <QDebug>
#include <QWebElement>
#include <QWebElementCollection>

using namespace nsBaseWebEngine;

class defaqto_com : public BaseWebEngine
{
	Q_OBJECT

public:
	defaqto_com(QWidget *apParent = 0);

protected:
	void doStart();
	void doStop(){}
	void doWebLoadFinished();
	void initDatabase();
	void initExtWidget() {
		mpExtFileLoading = new ExtFileLoading(this, mpAppPref);
	}

private:
	bool gotoNextUrlData();
	void getDetailData();

private:
	QList<QStringList>			mURLData; // loaded data from CSV file
	QStringList					mURLItem;
};

#endif // defaqto_com_H
