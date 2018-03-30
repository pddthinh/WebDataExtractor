#ifndef LOGGER_H
#define LOGGER_H

//#include "WDE.h"
#include <QString>
#include "WDEDef.h"
#include "DataEntry/dataentry.h"

class QFile;
class QTextStream;
class QStringList;

namespace nsLogger
{

class WDE_LIB_EXPORT Logger
{
public:
	Logger();
	Logger(const QString &astrPath);
	QString getFilePath() const;
	void setFilePath(const QString &astrPath);

	bool logString(const QString &astrIn);
	bool logStringList(const QStringList &aList, const QString &astrMsg = "", bool append = true);
	int logDataList(const DATA_LIST &aList, const QString &astrTableName, bool append = true);

private:
	QString     mstrPath;
};

} // namespace nsLogger

#endif // LOGGER_H
