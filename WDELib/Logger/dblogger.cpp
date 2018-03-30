#include "dblogger.h"

DBLogger::DBLogger(Logger *apLog, QString astrDBPath)
{
	mpLog = apLog;

	if(astrDBPath.isEmpty() == true)
		astrDBPath = DEF_STR_DEFAULT_DATA_LOG_NAME;
	mpDataLog = new Logger(astrDBPath);
}

void DBLogger::setLogPath(const QString &astrPath)
{
	if(mpDataLog == NULL)
		return;

	mpDataLog->setFilePath(astrPath);
}

void DBLogger::addData(const DATA_LIST &aList)
{
	mlstData << aList;
}

void DBLogger::logString(const QString &astrData)
{
	mpDataLog->logString(astrData);
}

int DBLogger::logDataList(const DATA_LIST &aList, QString astrTable)
{
	if(astrTable.isEmpty() == true)
		astrTable = mstrTableName;

	return mpDataLog->logDataList(aList, astrTable);
}
