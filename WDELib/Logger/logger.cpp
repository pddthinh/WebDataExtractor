#include "logger.h"

#include <QFile>
#include <QTextStream>
#include <QStringList>

using namespace std;
using namespace nsLogger;

Logger::Logger()
{
}

Logger::Logger(const QString &astrPath)
{
	setFilePath(astrPath);
}

QString Logger::getFilePath() const
{
	return mstrPath;
}

void Logger::setFilePath(const QString &astrPath)
{
	mstrPath = astrPath;
}

bool Logger::logString(const QString &astrIn)
{
	bool lblRet = false;

	do
	{
		QFile lFile(mstrPath);
		QTextStream lstream(&lFile);

		if(!lFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
		{
			qWarning("Can not open log file");
			break;
		}

		lstream << astrIn << endl;

		lFile.close();
	}while(false);

	return lblRet;
}

bool Logger::logStringList(const QStringList &aList, const QString &astrMsg, bool append)
{
	bool lblRet = false;

	do
	{
		QFile lFile(mstrPath);
		QTextStream lstream(&lFile);

		QIODevice::OpenMode mode = QIODevice::WriteOnly | QIODevice::Text;
		if (append == true)
			mode |= QIODevice::Append;

		if(!lFile.open(mode))
		{
			qWarning("Can not open log file");
			break;
		}

		if(!astrMsg.isEmpty())
			lstream << astrMsg << endl;

		foreach(QString lstr, aList)
			lstream << lstr << endl;

		lFile.close();
	}while(false);

	return lblRet;
}

int Logger::logDataList(const DATA_LIST &aList, const QString &astrTableName, bool append)
{
	int liSaved = -1;

	do
	{
		QFile lFile(mstrPath);
		QTextStream lstream(&lFile);

		QIODevice::OpenMode mode = QIODevice::WriteOnly | QIODevice::Text;
		if (append == true)
			mode |= QIODevice::Append;

		if(!lFile.open(mode))
		{
			qWarning("Can not open log file");
			break;
		}

		liSaved = 0;
		foreach(DataEntry lData, aList)
		{
			lstream << lData.getSQLString(astrTableName) << endl;
			liSaved++;
		}

		lFile.close();
	}while(false);

	return liSaved;
}
