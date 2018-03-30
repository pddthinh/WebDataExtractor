#ifndef UTILITIES_H
#define UTILITIES_H

#include "WDEDef.h"
#include <QString>
#include <QRegExp>

#include <QtGlobal>

#include <DataEntry/dataentry.h>

#define DEF_STR_REGEX_SAME_LINE		"%1([^%2]+)%3"	// %1: Start pattern
													// %2: New line mark
													// %3: End pattern

#define DEF_STR_REGEX_MULTI_LINE	"%1([^.]+)%2"	// %1: Start pattern
													// %2: End pattern

#define DEF_STR_REGEX_EMAIL_VALIDATOR	QLatin1String("(([a-zA-Z0-9_\\-\\.!#$%\\*/?|^`~&'\\+]*" \
											"[a-zA-Z0-9_\\-!#$%\\*/?|^`~&'\\+])|" \
											"(\"([\\x0001-\\x0008,\\x000B,\\x000C,\\x000E-\\x0019,\\x007F," \
											"\\x0021,\\x0023-\\x005B,\\x005D-\\x007E," \
											"\\x0009,\\x0020]|" \
											"(\\\\[\\x0001-\\x0009,\\x000B,\\x000C," \
											"\\x000E-\\x007F]))*\"))" \
											"@" \
											"[a-zA-Z0-9\\-]+(\\.[a-zA-Z0-9\\-]+)*")

class WDE_LIB_EXPORT Utilities
{
public:
	Utilities();

	// Extract value data from the input string using RegEx
	static QString extractValue(const QString &astrIn,
								const QString &astrStart,
								const QString &astrEnd,
								const QString &astrNL = DEF_STR_NEWLINE,
								bool ablMultiLine = false);
	static QString extractValue2(const QString &astrIn,
								 const QString &astrStart,
								 const QString &astrEnd,
								 const QString &astrNL = DEF_STR_NEWLINE,
								 bool ablMultiLine = false,
								 bool escape = true);

	static QString extractValue(const QString &strIn,
								const QString &strPattern,
								bool blEscape = false);

	// Extract and update DataEntry usign Regular Expression
	static void extractAndUpdate(DataEntry *apData,
								 const QString &astrDataKey,
								 const QString &astrIn,
								 const QString &astrStart,
								 const QString &astrEnd = QLatin1String("\n"),
								 const QString &astrNL = DEF_STR_NEWLINE,
								 bool ablMultiLine = false);

	// Extract and update DataEntry usign Regular Expression
	static void extractAndUpdate2(DataEntry *apData,
								 const QString &astrDataKey,
								 const QString &astrIn,
								 const QString &astrStart,
								 const QString &astrEnd = QLatin1String("\n"),
								 const QString &astrNL = DEF_STR_NEWLINE,
								 bool ablMultiLine = false);

	// Extract email addresses from input text
	static QStringList extractEmailAddress(const QString &astrIn);

	// Remove all encoded data
	static QString correctData(QString astrData);
	static QString correctCSVData(QString astrData);

	// Parse a string to UK address structure
	static TAG_ADDRESS parseUK_Add(const QString &astrIn,
								   bool ablHaveCountry = true,
								   const QString &astrInSeperator = QLatin1String(","),
								   const QString &astrOutSeparator = QLatin1String(", "),
								   const QString &astrNewLine = QLatin1String("\n"));
	// Parse a string to UK address structure
	static TAG_ADDRESS parseAustralia_Add(const QString &astrIn,
										  const QString &astrInSeperator = QLatin1String(","),
										  const QString &astrOutSeparator = QLatin1String(", "),
										  const QString &astrNewLine = QLatin1String("\n"));
	// Parse a person name
	static TAG_PERSON_NAME parsePersonName(const QString &astrIn);

	// Load data list from a file
	static QStringList loadTextData(const QString &astrPath);
	static QList<QStringList> loadCSVData(const QString &astrPath,
										   bool skippedTitle = true,
										   const QString &separator = QLatin1String(","));

	static int randInt(int low, int high);

private:
	static QString correctRegExString(const QString &astrIn);
};

#endif // UTILITIES_H
