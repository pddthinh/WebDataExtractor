#ifndef CIPHE_ORG_UK_H
#define CIPHE_ORG_UK_H


#include "../WDELib/WDEInclude.h"
#include <QStringList>
//#include "dblogger.h"

using namespace nsBaseWebEngine;
using namespace nsDatabase;

class ciphe_org_uk
	: public BaseWebEngine
{
public:
	ciphe_org_uk(QWidget *apParent = 0);
	virtual ~ciphe_org_uk();

protected:
	void doStart();
	void doStop();
	void doWebLoadFinished();
	void initDatabase();
	void initExtWidget(){}

protected:
	void loadPostCode();
	QString getNextPostcode();
	void gotoNextPage();
	void getData();

protected:
	QString     mstrPostCode;
	QStringList mlstPostcode;
//    LoggerTask  *mpLoggerTask;
	DBLogger    *mpDBLogger;
	Logger      mLogger;

	//Database
	QString         mstrTableName;
	QStringList		mlstColName;
	INT_LIST		mlstColKey;
	DATA_LIST       mlstData;
};

#endif // CIPHE_ORG_UK_H
