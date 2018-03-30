#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QString>
#include <QSqlDatabase>

#define TMP_TABLE			QLatin1String("tmp")
#define TMP_TBL_COL_VALUE	QLatin1String("value")

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
	void close();

	bool createTmpTable(const QString& tbName = TMP_TABLE);
	void insertTmpTable(const QString& value,
	const QString& tbName = TMP_TABLE);
	QStringList getAllFromTmpTable(const QString& tbName = TMP_TABLE);

///////////////////////////////////////////////////////////
	inline QString getDBPath() { return mDBPath; }

private:
	QString			mDBPath;
	QSqlDatabase	mDB;
};

#endif // DBMANAGER_H
