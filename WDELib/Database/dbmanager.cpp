#include "dbmanager.h"

#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>

DBManager::DBManager() {
	mpSQLBuilder = new SQLQueryBuilder();
}

DBManager::DBManager(const QString& dbPath) {
	mDBPath = dbPath;
	mpSQLBuilder = new SQLQueryBuilder();
}

bool DBManager::open(const QString &dbPath) {
	bool blRet = false;

	if (mDBPath != dbPath && !dbPath.isEmpty())
		mDBPath = dbPath;

	if (mDB.isValid() || mDB.isOpen())
		mDB.close();

	mDB = QSqlDatabase::addDatabase(QLatin1String("QSQLITE"));
	mDB.setDatabaseName(mDBPath);

	blRet = mDB.open();

	return blRet;
}

void DBManager::close() {
	mDB.close();
}

bool DBManager::createTmpTable(const QString &tbName) {
	bool blRet = false;

	do {
		if (!mDB.isOpen() && !open()) {
			qDebug() << QLatin1String("Can not open database path: ") << mDBPath;
			break;
		}

		if (tbName.isNull() || tbName.isEmpty()) {
			qDebug() << QLatin1String("Tempt table name is empty!");
			break;
		}

		QSqlQuery create;
		create.prepare(QString("CREATE TABLE IF NOT EXISTS '%1' ('%2' VARCHAR(1024) PRIMARY KEY)")
					   .arg(tbName)
					   .arg(TMP_TBL_COL_VALUE));
		blRet = create.exec();

		if (!blRet)
			qDebug() << QLatin1String("Create tmp table failed: ")
					 << create.lastError().text();
	} while(false);

	return blRet;
}

void DBManager::insertTmpTable(const QString &value, const QString &tbName) {
	do {
		if (value.isNull() || value.isEmpty()) break;

		QString tmp = value;
		tmp.replace('\'', "''");

		QString insertSQL = QString("INSERT OR IGNORE INTO '%1'('%2') VALUES('%3')")
							.arg(tbName)
							.arg(TMP_TBL_COL_VALUE)
							.arg(tmp)
							;

		QSqlQuery insert(mDB);
		bool inserted = insert.exec(insertSQL);

		if (!inserted)
			qDebug() << insert.lastError().text();
	} while (false);
}

QStringList DBManager::getAllFromTmpTable(const QString &tbName) {
	QStringList result;

	do {
		QSqlQuery select(QString("SELECT * FROM '%1'")
						 .arg(tbName)
						 );
		int idValue = select.record().indexOf(TMP_TBL_COL_VALUE);
		while (select.next())
			result << select.value(idValue).toString();
	} while(false);

	return result;
}

//---------------------------------------------------------
// Implementation for data table
bool DBManager::createDataTable(const QString &table,
								const QStringList &alstColumn,
								const INT_LIST &alstKey) {
	bool blRet = false;

	QString sqlCreate = mpSQLBuilder->getSQLCreateTable(table, alstColumn, alstKey);
#if 0
	qDebug() << sqlCreate;
#endif

	QSqlQuery create(mDB);
	blRet = create.exec(sqlCreate);
	if (!blRet)
		qDebug() << QLatin1String("Create data table failed: ")
				 << create.lastError().text();
	create.clear();

	return blRet;
}

bool DBManager::insertData(DataEntry* data,
						   const QString& table) {
	bool blRet = false;

	if (!data)
		return blRet;

	QSqlQuery insert(mDB);
	QString sql = data->getSQLString(table);
#if 0
	qDebug() << sql;
#endif

	blRet = insert.exec(sql);
	if (!blRet)
		qDebug() << QLatin1String("Can not insert data into DB!")
				 << insert.lastError().text();

	return blRet;
}
//---------------------------------------------------------
