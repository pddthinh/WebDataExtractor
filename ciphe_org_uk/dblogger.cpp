#include "dblogger.h"

DBLogger::DBLogger(Logger *apLog)
{
    mpLog = apLog;
    mpDataLog = new Logger(DEF_STR_DEFAULT_DATA_LOG_NAME);
}

DBLogger::~DBLogger()
{}

void DBLogger::addData(const DATA_LIST &aList)
{
    mlstData << aList;
}

void DBLogger::logString(const QString &astrData)
{
    mpDataLog->logString(astrData);
}

int DBLogger::logDataList(const DATA_LIST &aList, const QString &astrTable)
{
    return mpDataLog->logDataList(aList, astrTable);
}
