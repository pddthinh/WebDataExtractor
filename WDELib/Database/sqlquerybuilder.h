#ifndef SQLQUERYBUILDER_H
#define SQLQUERYBUILDER_H

#include "WDE.h"
#include "WDEDef.h"

#include <QString>
#include <QStringList>
#include <QList>

using namespace nsWDEDef;

namespace nsDatabase
{

class WDE_LIB_EXPORT SQLQueryBuilder
{
public:
	SQLQueryBuilder();
	SQLQueryBuilder(const QString &astrTableName);

	QString getSQLCreateTable(const QString &astrTableName,
							  const QStringList &alstColumn,
							  const INT_LIST &alstKey,
							  bool ablComment = false);
	QString getSQLInsertPrepared(const QString &astrTableName,
								 const QStringList &alstColumn);
	QString getSQLInsert(const QString &astrTableName,
						 const DATA_MAP &aMap);

//	QString getSQLSelect(const QString &astrTableName,
//						 const DATA_MAP &aMap,
//						 const QStringList &alstKey);

// Properties get/set
public:
	void setCommentStart(QString astr) { mstrCommentStart = astr; }
	QString getCommentStart() const { return mstrCommentStart; }

	void setTableName(const QString &astrTable);
	QString getTableName() const { return mstrTableName; }

protected:
	QString		mstrCommentStart;
	QString     mstrTableName;
	QString     mstrDummyTable;
};

} // namespace nsDatabase

#endif // SQLQUERYBUILDER_H
