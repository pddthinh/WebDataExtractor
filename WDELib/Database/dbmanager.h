#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QString>
#include <QSqlDatabase>

#define TMP_TABLE			QLatin1String("tmp")
#define TMP_TBL_COL_VALUE	QLatin1String("value")

#include "WDEDef.h"
#include "DataEntry/dataentry.h"

#include "sqlquerybuilder.h"

using namespace nsDatabase;

/**
 * @brief The DBManager class
 * SQLite DB manager
 */
class DBManager
{
public:
	DBManager();
	DBManager(const QString& dbPath);
	bool open(const QString& dbPath = 0);
	bool isOpen();
	void close();

	bool createTmpTable(const QString& tbName = TMP_TABLE);
	void insertTmpTable(const QString& value, const QString& tbName = TMP_TABLE);
	QStringList getAllFromTmpTable(const QString& tbName = TMP_TABLE);

	// data table
	bool createDataTable(const QString &table,
						 const QStringList &alstColumn,
						 const INT_LIST &alstKey);
	bool insertData(DataEntry* data, const QString &table);


///////////////////////////////////////////////////////////
	inline QString getDBPath() { return mDBPath; }

private:
	QString			mDBPath;
	QSqlDatabase	mDB;
	SQLQueryBuilder	*mpSQLBuilder;
};

#endif // DBMANAGER_H
