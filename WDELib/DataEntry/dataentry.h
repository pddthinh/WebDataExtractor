#ifndef DATAENTRY_H
#define DATAENTRY_H

#include "WDE.h"
#include "WDEDef.h"
#include <QStringList>
#include <QDebug>

class QWebElement;

using namespace nsWDEDef;

class WDE_LIB_EXPORT DataEntry
{
public:
	DataEntry();
	DataEntry(const DataEntry &aData);
	DataEntry(const QStringList &alstColName);
	void clearData();
	void clearTable();
	void initColName(const QStringList &alstColName);

	DataEntry &operator =(const DataEntry &aData);

	bool updatePropertyString(const QString &astrKey, const QString &astrValue, bool autoCorrect = true);
	bool updatePropertyString(int aColumnIdx, const QString &astrValue, bool autoCorrect = true);
	bool updateProperty(const QString &key, QStringList value, bool autoCorrect = true, bool forceList = true);
	bool updateProperty(int colIdx, QStringList value, bool autoCorrect = true, bool forceList = true);
	void addProperty(const QString &astrKey, const QString &astrValue);
	void parseElement(const QString &key, const QWebElement &element);
	void parseElement(int aColumnIdx, const QWebElement &element);

	QStringList getProperty(const QString &astrKey);
	QStringList getProperty(int aColumnIdx);
	QString getPropertyString(const QString &astrKey);
	QString getPropertyString(int aColumnIdx);

//	QString getCSVString(const QStringList &alstKey = QStringList(), const QString &astrSep = ", ");
	QString getSQLString(const QString &astrTableName);

//	QString getSqlSelectString(const QString & astrTableName,
//							   const QStringList &alstKey);

	/**
	 * Get the whole data map of this entry
	 *
	 * @brief getDataMap
	 * @return
	 */
	inline DATA_MAP *getDataMap() { return &mData; }

	/**
	 * Check weather this entry is empty
	 *
	 * @brief isEmpty
	 * @return
	 */
	inline bool isEmpty(){ return mData.isEmpty(); }

	void printDebug();
	QString getDataString();

protected:
	DATA_MAP    mData;
	QStringList	mColumns;
};

#endif // DATAENTRY_H
