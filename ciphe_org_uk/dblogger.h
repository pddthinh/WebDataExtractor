#ifndef DBLOGGER_H
#define DBLOGGER_H

#include "WDEInclude.h"
using namespace nsLogger;

class DBLogger
{
public:
    DBLogger(Logger *apLog);
    virtual ~DBLogger();

    void addData(const DATA_LIST &aList);
    void logString(const QString &astrData);
    int logDataList(const DATA_LIST &aList, const QString &astrTable);

private:
    Logger      *mpDataLog;     // SQL data log
    Logger      *mpLog;         // Common log
    DATA_LIST   mlstData;
};

#endif // DBLOGGER_H
