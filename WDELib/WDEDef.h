#ifndef WDEDEF_H
#define WDEDEF_H

#include <QList>
#include <QString>
#include <QLatin1String>
#include <QMap>

class DataEntry;

namespace nsWDEDef
{
typedef QList<int>          INT_LIST;

typedef QList<DataEntry>   DATA_LIST;
typedef QMap<QString, QStringList> DATA_MAP;
#define DATA_LIST_SEPARATOR		";"

///////////////////////////////////////////////////////////
typedef struct tagAddressInfo
{
	QString strAddress;
	QString strTown;
	QString strCounty;
	QString strCountry;
	QString strPostcode;
	QString strFullAddress;

	void init()
	{
		strAddress.clear();
		strTown.clear();
		strCounty.clear();
		strCountry.clear();
		strPostcode.clear();
		strFullAddress.clear();
	}

	QString toString()
	{
		QString lstrRet = "Add: %1\nTown: %2\nCounty: %3\nCountry: %4\nPostCode: %5\nFull Add: %6";

		lstrRet = lstrRet.arg(strAddress)
						.arg(strTown)
						.arg(strCounty)
						.arg(strCountry)
						.arg(strPostcode)
						.arg(strFullAddress);
		return lstrRet;
	}
} TAG_ADDRESS;

///////////////////////////////////////////////////////////
typedef struct tagPersonName
{
	QString strFirstname;
	QString strLastName;
	QString strFullname;

	void init()
	{
		strFirstname.clear();
		strLastName.clear();
		strFullname.clear();
	}

	QString toString()
	{
		QString lstrRet = "Firstname: %1\nLastname: %2\nFull name: %3";

		lstrRet = lstrRet.arg(strFirstname)
						.arg(strLastName)
						.arg(strFullname);

		return lstrRet;
	}
} TAG_PERSON_NAME;

///////////////////////////////////////////////////////////
#define DEF_STR_EMPTY					QLatin1String("")

#define DEF_STR_TITLE					QLatin1String("WebDataExtractor")
#define DEF_STR_WEB_LOAD_PROGRESS		QLatin1String("Loading --- ")

#define DEF_INT_TIMER_STOP_LOAD			40000	// 40s

#define DEF_STR_DEFAULT_LOGGER_NAME		QLatin1String("DefaultWDELog.txt")
#define DEF_STR_DEFAULT_DATA_LOG_NAME	QLatin1String("DefaultWDEData.sql")
#define	DEF_STR_DEFAULT_DATA_XLS_NAME	QLatin1String("DefaultWDEData.xml")

#ifdef Q_OS_LINUX
#define	DEF_STR_NEWLINE		QLatin1String("\n")
#else
#define	DEF_STR_NEWLINE		QLatin1String("\r\n")
#endif

//////////////////////////////////
// Utilities macros
#define DEF_CHECK_BREAK(aWebElement, aWarningMsg) \
	if(aWebElement.isNull()) \
	{ \
		DEF_LOG(aWarningMsg); \
		break; \
	}

#define DEF_LOG(str) \
	mpLog->logString(str);

#define DEF_LOG_LIST(aList, aMsg) \
	mpLog->logStringList(aList, aMsg);

#define DEF_LOG_DATA_LIST(aList, astrTableName) \
	mpLog->logDataList(aList, astrTableName);

} // namespace WDEDef

#endif // WDEDEF_H
