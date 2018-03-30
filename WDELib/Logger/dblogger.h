#ifndef DBLOGGER_H
#define DBLOGGER_H

//#include "WDE.h"
#include "WDEInclude.h"
using namespace nsLogger;

class WDE_LIB_EXPORT DBLogger
{
public:
	DBLogger(Logger *apLog, QString astrDBPath = "");
	virtual ~DBLogger() {}

	void addData(const DATA_LIST &aList);
	void logString(const QString &astrData);
	int logDataList(const DATA_LIST &aList, QString astrTable = "");

	void setLstColName(const QStringList &aLst){ mlstColName = aLst; }
	void setLstKey(const INT_LIST &aLst){ mlstKey = aLst; }
	void setTableName(const QString &astrName){ mstrTableName = astrName; }
	void setLogPath(const QString &astrPath);

	QStringList getLstColName(){ return mlstColName; }
	INT_LIST getLstKey(){ return mlstKey; }
	QString getTableName(){ return mstrTableName; }

private:
	Logger      *mpDataLog;     // SQL data log
	Logger      *mpLog;         // Common log
	DATA_LIST   mlstData;

	QStringList	mlstColName;
	INT_LIST	mlstKey;
	QString		mstrTableName;
};

#endif // DBLOGGER_H
