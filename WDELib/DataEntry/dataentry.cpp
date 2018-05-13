#include "dataentry.h"
#include "Utilities/utilities.h"
#include "Database/sqlquerybuilder.h"

#include <QWebElement>

using namespace nsDatabase;

DataEntry::DataEntry()
{
	mData.clear();
}

DataEntry::DataEntry(const DataEntry &aData)
{
	mData.clear();

	QStringList llstKeys = aData.mData.keys();
	foreach(QString lstrKey, llstKeys)
		mData[lstrKey] = aData.mData[lstrKey];

	mColumns = aData.mColumns;
}

DataEntry::DataEntry(const QStringList &alstColName)
{
	initColName(alstColName);
}

void DataEntry::initColName(const QStringList &alstColName)
{
	mData.clear();

	foreach (QString lstrKey, alstColName)
		mData.insert(lstrKey, QStringList());

	mColumns = alstColName;
}

DataEntry &DataEntry::operator =(const DataEntry &aData)
{
	this->clearTable();
	QStringList llstKeys = aData.mData.keys();
	foreach(QString lstrKey, llstKeys)
		mData.insert(lstrKey, aData.mData[lstrKey]);

	return *this;
}

void DataEntry::clearTable()
{
	mData.clear();
	mColumns.clear();
}

void DataEntry::clearData()
{
	foreach(QString lstrKey, mData.keys())
		mData[lstrKey] = QStringList();
}

/**
 * Update data entry property
 *
 * @param aColumnIdx
 * @param astrValue
 * @param autoCorrect
 * @return
 */
bool DataEntry::updatePropertyString(int aColumnIdx, const QString &astrValue, bool autoCorrect) {
	bool blRet = false;

	do {
		if (aColumnIdx < 0 || aColumnIdx >= mColumns.size())
			break;

		blRet = updatePropertyString(mColumns[aColumnIdx], astrValue, autoCorrect);
	} while (0);

	return blRet;
}

/**
 * Update data entry property
 *
 * @param astrKey
 * @param astrValue
 * @param autoCorrect Auto correct (remove invalid character) or not (default: true)
 * @return
 */
bool DataEntry::updatePropertyString(const QString &astrKey, const QString &astrValue, bool autoCorrect)
{
	bool lblRet = false;

	do {
		if(!mData.contains(astrKey))
			break;

		lblRet = true;
		if (autoCorrect == true) {
			mData[astrKey] = QStringList(Utilities::correctData(astrValue));
			break;
		}

		mData[astrKey] = QStringList(astrValue.trimmed());
	} while(0);

	return lblRet;
}

/**
 * @brief DataEntry::updateProperty
 *		Update data property
 *
 * @param key
 * @param value
 * @param autoCorrect (default = true) preprocessing data
 * @param forceList (default = true) force input value is a list (>= 2 entries)
 * @return
 */
bool DataEntry::updateProperty(const QString &key, QStringList value, bool autoCorrect, bool forceList) {
	bool blRet = false;

	do {
		if (!mData.contains(key))
			break;

		blRet = true;
		mData[key].clear();

		if (forceList && value.count() == 1)
			value << QLatin1String(""); // add an empty padding entry

		if (autoCorrect) {
			QStringList tmpList;
			foreach(QString tmp, value)
				if (tmp.length() > 0)
					tmpList << tmp;

			foreach(QString tmp, tmpList)
				mData[key] << Utilities::correctData(tmp);

			break;
		}

		mData[key] << value;
	} while (0);

	return blRet;
}

bool DataEntry::updateProperty(int colIdx, QStringList value, bool autoCorrect, bool forceList) {
	bool blRet = false;

	do {
		if (colIdx < 0 || colIdx >= mColumns.size())
			break;

		blRet = updateProperty(mColumns[colIdx], value, autoCorrect, forceList);
	} while (0);

	return blRet;
}

/**
 * Add a property and its value
 *
 * @param astrKey
 * @param astrValue
 */
void DataEntry::addProperty(const QString &astrKey, const QString &astrValue)
{
	if(updatePropertyString(astrKey, astrValue) == false) {
		// Key not existed, insert new
		QString tmpKey = astrKey;
		tmpKey.trimmed();

		mData[tmpKey] << Utilities::correctData(astrValue);
		mColumns << tmpKey;
	}
}

QStringList DataEntry::getProperty(const QString &astrKey)
{
	return mData[astrKey];
}

QStringList DataEntry::getProperty(int aColumnIdx) {
	QStringList property;

	do {
		if (aColumnIdx < 0 || aColumnIdx >= mColumns.size())
			break;

		property = mData[mColumns[aColumnIdx]];
	} while (0);

	return property;
}

QString DataEntry::getPropertyString(const QString &astrKey) {
	QString ret;

	do {
		if (astrKey.isEmpty())
			break;

		QString tmp;
		foreach (tmp, mData[astrKey]) {
			if (tmp.isEmpty())
				continue;

			ret += tmp + QLatin1String(DATA_LIST_SEPARATOR);
		}

		if (ret.endsWith(QLatin1String(DATA_LIST_SEPARATOR)))
			ret.chop(1);
	} while (0);

	return ret;
}

QString DataEntry::getPropertyString(int aColumnIdx) {
	QString ret;

	do {
		if (aColumnIdx < 0 || aColumnIdx >= mColumns.size())
			break;

		ret = getPropertyString(mColumns[aColumnIdx]);
	} while (0);

	return ret;
}

//QString DataEntry::getCSVString(const QStringList &alstKey, const QString &astrSep)
//{
//	QString lstrRet = "";
//	QString lstrTmp;
//	QList<QString> llstData;

//	foreach (lstrTmp, alstKey)
//		llstData << mData[lstrTmp];

//	if(llstData.isEmpty())
//		llstData = mData.values();

//	foreach(lstrTmp, llstData)
//		lstrRet += Utilities::correctCSVData(lstrTmp) + astrSep;

//	lstrRet.truncate(lstrRet.length() - astrSep.length());

//	return lstrRet;
//}

QString DataEntry::getSQLString(const QString &astrTableName)
{
	SQLQueryBuilder lbuilder;

	return lbuilder.getSQLInsert(astrTableName, mData);
}

//QString DataEntry::getSqlSelectString(const QString &astrTableName,
//									  const QStringList &alstKey)
//{
//	SQLQueryBuilder lbuilder;
//	return lbuilder.getSQLSelect(astrTableName, mData, alstKey);
//}

void DataEntry::printDebug()
{
	foreach(QString lstrKey, mData.keys())
		qDebug() << lstrKey << QLatin1String(":\t") << mData[lstrKey];
}

QString DataEntry::getDataString()
{
	QString lstrRet = QLatin1String("{ ");
	QString lstrFormat = QLatin1String("[%1]=%2");

	QString tmp, value;
	foreach(QString key, mData.keys()) {
		tmp.clear();
		foreach (value, mData[key])
			tmp += value + QLatin1String(DATA_LIST_SEPARATOR);
		if (tmp.endsWith(QLatin1String(DATA_LIST_SEPARATOR)))
			tmp.chop(1);

		lstrRet += lstrFormat.arg(key).arg(tmp) + QLatin1String(", ");
	}

	if (lstrRet.endsWith(QLatin1String(", ")))
		lstrRet.chop(2);

	lstrRet += QLatin1String(" }");

	return lstrRet;
}

void DataEntry::parseElement(const QString &key, const QWebElement &element)
{
	do
	{
		QString value = element.toPlainText();
		if (value.isEmpty() == true)
			break;

		updatePropertyString(key, value);
	} while (0);
}

void DataEntry::parseElement(int aColumnIdx, const QWebElement &element) {
	do {
		if (aColumnIdx < 0 || aColumnIdx >= mColumns.size())
			break;

		parseElement(mColumns[aColumnIdx], element);
	} while (0);
}
