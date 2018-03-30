#include "utilities.h"
#include <QStringList>
#include <QFile>

//QString gstrEmailPattern = QLatin1String("([^\\s]+)@([a-zA-Z0-9_]+)\\.[a-zA-Z]{2,6}(\\.[a-zA-Z]{2,6})?\\b");

Utilities::Utilities()
{
}

QString Utilities::correctRegExString(const QString &astrIn)
{
	QString lstrRet = astrIn;

	lstrRet = lstrRet.replace(QLatin1String("\\"), QLatin1String("\\\\"));
	lstrRet = lstrRet.replace(QLatin1String("["), QLatin1String("\\["));
	lstrRet = lstrRet.replace(QLatin1String("]"), QLatin1String("\\]"));
	lstrRet = lstrRet.replace(QLatin1String("{"), QLatin1String("\\{"));
	lstrRet = lstrRet.replace(QLatin1String("}"), QLatin1String("\\}"));
	lstrRet = lstrRet.replace(QLatin1String("("), QLatin1String("\\("));
	lstrRet = lstrRet.replace(QLatin1String(")"), QLatin1String("\\)"));

	lstrRet = lstrRet.replace(QLatin1String("^"), QLatin1String("\\^"));
	lstrRet = lstrRet.replace(QLatin1String("$"), QLatin1String("\\$"));
	lstrRet = lstrRet.replace(QLatin1String("."), QLatin1String("\\."));
	lstrRet = lstrRet.replace(QLatin1String("?"), QLatin1String("\\?"));
	lstrRet = lstrRet.replace(QLatin1String("|"), QLatin1String("\\|"));
	lstrRet = lstrRet.replace(QLatin1String("*"), QLatin1String("\\*"));
	lstrRet = lstrRet.replace(QLatin1String("+"), QLatin1String("\\+"));

	return lstrRet;
}

/* Extract a value use Regular Expression */
QString Utilities::extractValue(const QString &astrIn,
								const QString &astrStart,
								const QString &astrEnd,
								const QString &astrNL,
								bool ablMultiLine)
{
	QString lstrRet = "";
	QString lstrPattern = "";
	QRegExp	lRegEx;

	if(ablMultiLine == true)
		lstrPattern = QString(DEF_STR_REGEX_MULTI_LINE).arg(correctRegExString(astrStart)).arg(correctRegExString(astrEnd));
	else
		lstrPattern = QString(DEF_STR_REGEX_SAME_LINE).arg(correctRegExString(astrStart)).arg(astrNL).arg(correctRegExString(astrEnd));

	lRegEx.setPattern(lstrPattern);

	if(lRegEx.indexIn(astrIn) != -1)
		lstrRet = lRegEx.cap(1);

	return lstrRet;
}

/**
 * Extract value from string using regular expression
 *
 * @param astrIn
 * @param astrStart
 * @param astrEnd
 * @param astrNL
 * @param ablMultiLine
 * @param escape
 * @return
 */
QString Utilities::extractValue2(const QString &astrIn,
								 const QString &astrStart,
								 const QString &astrEnd,
								 const QString &astrNL,
								 bool ablMultiLine,
								 bool escape)
{
	QString lstrRet = DEF_STR_EMPTY;

	do
	{
		if (escape == true)
		{
			lstrRet = extractValue(astrIn, astrStart, astrEnd, astrNL, ablMultiLine);
			break;
		}

		QRegExp	lRegEx;
		QString lstrPattern = DEF_STR_EMPTY;
		int iStartGrp;

		lRegEx.setPattern(astrIn);
		iStartGrp = lRegEx.captureCount();

#define DEF_STR_REGEX_MULTI_LINE2		QLatin1String("%1(.+?)%2")
#define DEF_STR_REGEX_SAME_LINE2		QLatin1String("%1([^%2]+?)%3")

		if (ablMultiLine == true)
			lstrPattern = QString(DEF_STR_REGEX_MULTI_LINE2)
					.arg(astrStart)
					.arg(astrEnd);
		else
			lstrPattern = QString(DEF_STR_REGEX_SAME_LINE2)
					.arg(astrStart)
					.arg(astrNL)
					.arg(astrEnd);

		lRegEx.setPattern(lstrPattern);
		if (lRegEx.indexIn(astrIn) != -1)
			lstrRet = lRegEx.cap(iStartGrp + 1);
	} while (false);

	return lstrRet.trimmed();
}

/**
 * Extract value with regular expression
 *
 * @param strIn Input string
 * @param strPattern Pattern string
 * @param blEscape Do escape pattern or not
 * @return
 */
QString Utilities::extractValue(const QString &strIn, const QString &strPattern, bool blEscape)
{
	QString strRet;
	QString pattern;
	QRegExp regEx;

	pattern = strPattern;
	if (blEscape == true)
		pattern = correctRegExString(strPattern);

	regEx.setPattern(pattern);
	if (regEx.indexIn(strIn) != -1)
		strRet = regEx.cap();

	return strRet.trimmed();
}

/**
 * Extract DataEntry use Regular Expression
 *
 * @param apData
 * @param astrDataKey
 * @param astrIn
 * @param astrStart
 * @param astrEnd
 * @param astrNL
 * @param ablMultiLine
 */
void Utilities::extractAndUpdate(DataEntry *apData,
								 const QString &astrDataKey,
								 const QString &astrIn,
								 const QString &astrStart,
								 const QString &astrEnd,
								 const QString &astrNL,
								 bool ablMultiLine)
{
	do
	{
		if(apData == NULL)
			break;

		QString lstrValue = extractValue(astrIn, astrStart, astrEnd, astrNL, ablMultiLine);
		apData->addProperty(astrDataKey, lstrValue);
	}while(false);
}

/**
 * Extract DataEntry use Regular Expression
 *
 * @param apData
 * @param astrDataKey
 * @param astrIn
 * @param astrStart
 * @param astrEnd
 * @param astrNL
 * @param ablMultiLine
 */
void Utilities::extractAndUpdate2(DataEntry *apData,
								 const QString &astrDataKey,
								 const QString &astrIn,
								 const QString &astrStart,
								 const QString &astrEnd,
								 const QString &astrNL,
								 bool ablMultiLine)
{
	do
	{
		if(apData == NULL)
			break;

		QString lstrValue = extractValue2(astrIn, astrStart, astrEnd, astrNL, ablMultiLine);
		apData->addProperty(astrDataKey, lstrValue);
	}while(false);
}

/**
 * Extract all email addresses from input text
 *
 * @param astrIn
 * @return
 */
QStringList Utilities::extractEmailAddress(const QString &astrIn)
{
	QStringList llstResult;
	QRegExp lRegEx(DEF_STR_REGEX_EMAIL_VALIDATOR);
	int liPos = 0;

	while ((liPos = lRegEx.indexIn(astrIn, liPos)) != -1)
	{
		llstResult << lRegEx.cap();

		liPos += lRegEx.matchedLength();
	}

	return llstResult;
}

QString Utilities::correctData(QString astrData)
{
	do
	{
		if(astrData.isEmpty())
			break;

		astrData = astrData.trimmed();
		astrData.replace(QLatin1String("&amp;"), QLatin1String("&"));
		astrData.replace(QLatin1String("&quot;"), QLatin1String("\""));
		astrData.remove(QLatin1String("\r\n"));
		astrData.replace(QLatin1String("\n"), QLatin1String(", "));
//		astrData.replace(QLatin1String("'"), QLatin1String("''"));
	} while(false);

	return astrData;
}

QString Utilities::correctCSVData(QString astrData)
{
	return QString("\"") + astrData + QString("\"");
}

/**
 * Parse UK address
 *
 * @param astrIn
 * @param ablHaveCountry
 * @param astrInSeperator
 * @param astrOutSeparator
 * @param astrNewLine
 * @return
 */
TAG_ADDRESS Utilities::parseUK_Add(const QString &astrIn,
								   bool ablHaveCountry,
								   const QString &astrInSeperator,
								   const QString &astrOutSeparator,
								   const QString &astrNewLine)
{
	TAG_ADDRESS ltAdd;
	QStringList	llstData;
	QString lstrTmp;

	ltAdd.init();

	lstrTmp = astrIn;
	lstrTmp.replace(astrNewLine, astrOutSeparator);
	lstrTmp.replace(QLatin1String(",,"), QLatin1String(","));

	llstData = lstrTmp.split(astrInSeperator, QString::SkipEmptyParts);
	llstData.removeDuplicates();

	for(int i = 0; i < llstData.length(); i++)
		llstData.replace(i, llstData[ i ].trimmed());

	// Get Country
	if(!llstData.isEmpty() && ablHaveCountry == true)
		ltAdd.strCountry = correctData(llstData.takeLast());

	// Get Postcode
	if(!llstData.isEmpty())
		ltAdd.strPostcode = correctData(llstData.takeLast());

	// Get County
	if(!llstData.isEmpty())
		ltAdd.strCounty = correctData(llstData.takeLast());

	// Get Town
	if(llstData.count() >= 2)
	{
		lstrTmp = correctData(llstData.last());
		if(!lstrTmp.contains(QRegExp(QLatin1String("\\s(street|road|way|park|Estate|building|Avenue)"), Qt::CaseInsensitive) ))
		{
			ltAdd.strTown = lstrTmp;
			llstData.removeLast();
		}
	}

	// Get Address
	while(!llstData.isEmpty())
		ltAdd.strAddress += correctData(llstData.takeFirst()) + astrOutSeparator;

	if(ltAdd.strAddress.endsWith(astrOutSeparator) )
		ltAdd.strAddress.remove(ltAdd.strAddress.length() - astrOutSeparator.length(), astrOutSeparator.length());

	ltAdd.strFullAddress = "";
	if(!ltAdd.strAddress.isEmpty())
		ltAdd.strFullAddress += ltAdd.strAddress + astrOutSeparator;
	if(!ltAdd.strTown.isEmpty())
		ltAdd.strFullAddress += ltAdd.strTown + astrOutSeparator;
	if(!ltAdd.strCounty.isEmpty())
		ltAdd.strFullAddress += ltAdd.strCounty + astrOutSeparator;
	if(!ltAdd.strCountry.isEmpty())
		ltAdd.strFullAddress += ltAdd.strCountry + astrOutSeparator;
	if(!ltAdd.strPostcode.isEmpty())
		ltAdd.strFullAddress += ltAdd.strPostcode;

	return ltAdd;
}

/**
 * Parse Australia address
 *
 * @param astrIn
 * @param astrInSeperator
 * @param astrOutSeparator
 * @param astrNewLine
 * @return
 */
TAG_ADDRESS Utilities::parseAustralia_Add(const QString &astrIn,
										  const QString &astrInSeperator,
										  const QString &astrOutSeparator,
										  const QString &astrNewLine)
{
	TAG_ADDRESS ltAdd;
	QStringList	llstData;
	QString lstrTmp;

	ltAdd.init();

	lstrTmp = astrIn;
	lstrTmp.replace(astrNewLine, astrOutSeparator);
	lstrTmp.replace(QLatin1String(",,"), QLatin1String(","));

	llstData = lstrTmp.split(astrInSeperator, QString::SkipEmptyParts);
	llstData.removeDuplicates();

	for(int i = 0; i < llstData.length(); i++)
		llstData.replace(i, llstData[ i ].trimmed());

	// Get Country
	if(!llstData.isEmpty())
		ltAdd.strCountry = correctData(llstData.takeLast());

	// Get Postcode
	if(!llstData.isEmpty())
		ltAdd.strPostcode = correctData(llstData.takeLast());

	// Get Town
	if(llstData.count() >= 2)
	{
		lstrTmp = correctData(llstData.last());
		if(!lstrTmp.contains(QRegExp(QLatin1String("street|road"), Qt::CaseInsensitive) ))
		{
			ltAdd.strTown = lstrTmp;
			llstData.removeLast();
		}
	}

	// Get Address
	while(!llstData.isEmpty())
		ltAdd.strAddress += correctData(llstData.takeFirst()) + astrOutSeparator;

	if(ltAdd.strAddress.endsWith(astrOutSeparator) )
		ltAdd.strAddress.remove(ltAdd.strAddress.length() - astrOutSeparator.length(), astrOutSeparator.length());

	ltAdd.strFullAddress.clear();
	if(!ltAdd.strAddress.isEmpty())
		ltAdd.strFullAddress += ltAdd.strAddress + astrOutSeparator;
	if(!ltAdd.strTown.isEmpty())
		ltAdd.strFullAddress += ltAdd.strTown + astrOutSeparator;
	if(!ltAdd.strCounty.isEmpty())
		ltAdd.strFullAddress += ltAdd.strCounty + astrOutSeparator;
	if(!ltAdd.strCountry.isEmpty())
		ltAdd.strFullAddress += ltAdd.strCountry + astrOutSeparator;
	if(!ltAdd.strPostcode.isEmpty())
		ltAdd.strFullAddress += ltAdd.strPostcode;

	return ltAdd;
}

/**
 * Parse personal name
 *
 * @param astrIn
 * @return
 */
TAG_PERSON_NAME Utilities::parsePersonName(const QString &astrIn)
{
	TAG_PERSON_NAME	ltName;
	QStringList llstData;

	llstData = correctData(astrIn).split(QLatin1String(" "), QString::SkipEmptyParts);

	if(!llstData.isEmpty())
		ltName.strLastName = llstData.takeLast();

	ltName.strFirstname.clear();
	while(!llstData.isEmpty())
		ltName.strFirstname += llstData.takeFirst() + QLatin1String(" ");

	ltName.strFirstname = ltName.strFirstname.trimmed();
	ltName.strFullname = ltName.strFirstname + " " + ltName.strLastName;
	ltName.strFullname = ltName.strFullname.trimmed();

	return ltName;
}

/**
 * Load data (links) from a text file
 *
 * @param astrPath
 * @return
 */
QStringList Utilities::loadTextData(const QString &astrPath)
{
	QStringList llst;

	do
	{
		if (astrPath.isEmpty() == true)
			break;

		QString lstrLine;
		QFile lfile(astrPath);

		if (!lfile.open(QIODevice::ReadOnly | QIODevice::Text))
			break;

		llst.clear();
		while (!lfile.atEnd())
		{
			lstrLine = lfile.readLine();
			lstrLine = lstrLine.remove("\n");
			llst << lstrLine;
		}

		lfile.close();
	} while (false);

	return llst;
}

/**
 * Load data from a csv file
 *
 * @param astrPath
 * @param skippedTitle
 * @param separator
 * @return
 */
QList<QStringList> Utilities::loadCSVData(const QString &astrPath,
										  bool skippedTitle,
										  const QString &separator) {
	QList<QStringList> result;

	do {
		if (astrPath.isEmpty() == true)
			break;

		QFile lfile(astrPath);

		if (!lfile.open(QIODevice::ReadOnly | QIODevice::Text))
			break;

		result.clear();

		QString line;
		QStringList cols;
		QString column;
		int iSize, i;
		while(!lfile.atEnd()) {
			line = lfile.readLine();
			line = line.remove("\n");
			if (line.length() == 0)
				continue;

			cols = line.split(separator, QString::KeepEmptyParts, Qt::CaseInsensitive);
			iSize = cols.length();
			for (i = 0; i < iSize; i++) {
				column = cols[i];
				if (column.startsWith('"') == true &&
						column.endsWith('"') == true) {
					// remove the first and last character
					column = column.remove(0, 1);
					column.chop(1);
				}

				// update back to list
				cols[i] = column;
			}


			result << cols;
		}
		lfile.close();

		if (skippedTitle == true)
			result.removeFirst();

		// refine the result
	} while (false);

	return result;
}

/**
 * Generate an random integer from low to high
 *
 * @param low
 * @param high
 * @return
 */
int Utilities::randInt(int low, int high)
{
	return qrand() % ((high + 1) - low) + low;
}
