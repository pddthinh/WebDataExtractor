#include "sqlquerybuilder.h"

using namespace nsDatabase;

SQLQueryBuilder::SQLQueryBuilder()
{
	setCommentStart(QLatin1String("-- "));
}

SQLQueryBuilder::SQLQueryBuilder(const QString &astrTableName)
{
	setTableName(astrTableName);
	setCommentStart(QLatin1String("-- "));
}

void SQLQueryBuilder::setTableName(const QString &astrTable)
{
	mstrTableName = astrTable;
	mstrDummyTable = mstrTableName + QLatin1String("_dummy");
}

QString SQLQueryBuilder::getSQLCreateTable(const QString &astrTableName,
										   const QStringList &alstColumn,
										   const INT_LIST &alstKey,
										   bool ablComment)
{
	QString lstrRet;
	QString lstrDefaultCharset = QLatin1String("ENGINE=InnoDB DEFAULT CHARSET=utf8;");
	QString lstrKey;
	QString lstrTmp;
	int		liIdx;

	// Build the primary key list
	lstrKey.clear();
	if(alstKey.count() > 0)
	{
		lstrKey = QLatin1String("PRIMARY KEY (");
		lstrTmp = QLatin1String("`%1`, ");
		foreach(liIdx, alstKey)
			lstrKey += lstrTmp.arg(alstColumn[ liIdx ]);
		lstrKey.chop(2);
		lstrKey += QLatin1String(")");
	}

	// Build the create table query
	lstrRet = QString("CREATE TABLE `%1`(").arg(astrTableName);
	for(liIdx = 0; liIdx < alstColumn.length(); liIdx++)
	{
		lstrTmp = QLatin1String("`%1` LONGTEXT, ");
		if(alstKey.contains(liIdx) )
			lstrTmp = QLatin1String("`%1` VARCHAR(200), ");
		lstrRet += lstrTmp.arg(alstColumn[ liIdx ]);
	}

	// Combine with key list
	if(lstrKey.isEmpty())
		lstrRet.chop(2);
	else
		lstrRet += lstrKey;

	lstrRet += QLatin1String(") ") + lstrDefaultCharset;

	if(ablComment == true)
		lstrRet = mstrCommentStart + lstrRet;

	return lstrRet;
}

QString SQLQueryBuilder::getSQLInsertPrepared(const QString &astrTableName,
											  const QStringList &alstColumn)
{
	QString lstrRet = QLatin1String("INSERT INTO `%1`(");
	QString lstrTmp;
	QString lstrValue = QLatin1String("VALUES (");

	lstrRet = lstrRet.arg(astrTableName);

	lstrTmp = QLatin1String("`%1`, ");
	foreach(QString lstrCol, alstColumn)
	{
		lstrRet += lstrTmp.arg(lstrCol);
		lstrValue += lstrTmp.arg(QLatin1String("?"));
	}

	if(lstrRet.endsWith(QLatin1String(", ")))
		lstrRet.chop(2);
	if(lstrValue.endsWith(QLatin1String(", ")))
		lstrValue.chop(2);

	lstrRet += QLatin1String(") ") + lstrValue + QLatin1String(");");

	return lstrRet;
}

QString SQLQueryBuilder::getSQLInsert(const QString &astrTableName,
									  const DATA_MAP &aMap)
{
	QString lstrRet = QLatin1String("INSERT IGNORE INTO `%1`(");
	QString lstrRetTmp = QLatin1String("`%1`, ");
	QString lstrValue = QLatin1String("VALUES (");
	QString lstrValueTmp = QLatin1String("'%1', ");
	QString lstrKey;
	QStringList llstKey = aMap.keys();

	QString tmp;

	lstrRet = lstrRet.arg(astrTableName);
	foreach(lstrKey, llstKey)
	{
		lstrRet += lstrRetTmp.arg(lstrKey);

		if (aMap[lstrKey].isEmpty()) {
			lstrValue += QLatin1String("null, ");
			continue;
		}

		QString value;
		foreach (tmp, aMap[lstrKey])
			value += tmp + QLatin1String(DATA_LIST_SEPARATOR);
		if (value.endsWith(QLatin1String(DATA_LIST_SEPARATOR)))
			value.chop(1);
		if (value.isEmpty()) {
			lstrValue += QLatin1String("null, ");
			continue;
		}

		value.replace(QLatin1String("'"), QLatin1String("''"));
		lstrValue += lstrValueTmp.arg(value);

//		if (aMap[lstrKey].isEmpty() == false)
//			lstrValue += lstrValueTmp.arg(aMap[lstrKey]);
//		else
//			lstrValue += QLatin1String("null, ");
	}

	if(lstrRet.endsWith(QLatin1String(", ")))
		lstrRet.chop(2);
	if(lstrValue.endsWith(QLatin1String(", ")))
		lstrValue.chop(2);

	lstrRet += QLatin1String(") ") + lstrValue + QLatin1String(");");

	return lstrRet;
}

//QString SQLQueryBuilder::getSQLSelect(const QString &astrTableName,
//									  const DATA_MAP &aMap,
//									  const QStringList &alstKey)
//{
//	QString lstrRet;
//	QString lstrValue = QLatin1String("(`%1` LIKE '%2')");
//	QString lstrTmp;

//	lstrRet = QString("SELECT * FROM `%1` WHERE").arg(astrTableName);
//	foreach (QString key, alstKey)
//	{
//		lstrTmp = aMap[key];
//		if (lstrTmp.isEmpty())
//			continue;

//		lstrRet += lstrValue.arg(key).arg(lstrTmp);
//		lstrRet += QLatin1String(" AND");
//	}

//	if (lstrRet.endsWith(QLatin1String(" AND")))
//		lstrRet.chop(4);

//	return lstrRet;
//}
