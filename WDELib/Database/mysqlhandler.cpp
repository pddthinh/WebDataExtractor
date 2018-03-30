#include "mysqlhandler.h"
#include "DataEntry/dataentry.h"

using namespace nsDatabase;

MySQLHandler::MySQLHandler()
{
	init();
}

MySQLHandler::MySQLHandler(Logger *apLog)
{
	init();

	setLogger(apLog);
}

void MySQLHandler::init()
{
	//	mpDatabase = new QSqlDatabase();
	setCommentStart(QLatin1String("-- "));
	mstrErrMsgTemplate = QLatin1String("MySQLHandler::error --> %1");

	mpQuery = NULL;
	mpLog = NULL;

	mblCreateConnection = false;
	mblPrepareSQL = false;
}

bool MySQLHandler::createConnection()
{
	bool lblRet = false;

	do
	{
		QSqlDatabase db = QSqlDatabase::addDatabase(QLatin1String("QMYSQL3"));
		db.setHostName(mstrHostname);
		db.setUserName(mstrUsername);
		db.setPassword(mstrPassword);

		lblRet = db.open();
		mblCreateConnection = lblRet;

		if (lblRet == false)
		{
			QString error = db.lastError().text();
			DEF_LOG(QString("Open DB error: %1").arg(error));
			qDebug() << error;
		}
	} while(false);

	return lblRet;
}

void MySQLHandler::closeConnection()
{
	//	if(mpDatabase != NULL)
	//	{
	//		mpDatabase->close();
	//		mblCreateConnection = false;
	//		mblPrepareSQL = false;
	//	}

	//TODO: CLOSE CONNECTION
}

/*
 * Create database and prepare SQL query
 *
 * Param In:	astrTableName
 * Param In:	alstColumn
 * Param In:	alstKey
 * Param Out:	alstBoundKey
 */
bool MySQLHandler::initDatabase(const QString &astrTableName,
								const QStringList &alstColumn,
								const INT_LIST &alstKey,
								QStringList &alstBoundKey)
{
	bool lblRet = false;
	QString lstrSQL;
	QSqlDatabase db = QSqlDatabase::database();

	do
	{
		if(!mblCreateConnection)
			lblRet = createConnection();

		if(!lblRet)
			break;

		mpQuery = new QSqlQuery(db);
		if(!mpQuery)
		{
			DEF_LOG(mstrErrMsgTemplate.arg(
						QLatin1String("can not create QSqlQuery")));
			break;
		}

		// Create database
		{
			lblRet = mpQuery->prepare(QString("CREATE DATABASE %1")
									  .arg(mstrDBName));
			if (lblRet == false)
				break;

			lblRet = mpQuery->exec();
			// Maybe database existed ?
			if (lblRet == false)
			{
				QString error = mpQuery->lastError().text();
				DEF_LOG(mstrErrMsgTemplate.arg(mpQuery->lastError().text()));
				qDebug() << error;
			}
		}

		// Select the database
		{
			lblRet = mpQuery->exec(QString("USE %1").arg(mstrDBName));
			if (lblRet == false)
				break;
		}

		lblRet = false;
		lstrSQL = getSQLCreateTable(astrTableName, alstColumn, alstKey);

		// Create table
		if(!mpQuery->exec(lstrSQL))
			break;

		lstrSQL = getSQLInsertPrepared(astrTableName, alstColumn, alstBoundKey);
		lblRet = mpQuery->prepare(lstrSQL);
		// Prepare the SQL query
		if(!lblRet)
			break;

		mblPrepareSQL = lblRet;
	} while(false);

	// ERROR ?
	if (lblRet == false)
	{
		QString error = mpQuery->lastError().text();
		DEF_LOG(mstrErrMsgTemplate.arg(mpQuery->lastError().text()));
		qDebug() << error;
	}

	return lblRet;
}

QString MySQLHandler::getSQLCreateTable(const QString &astrTableName,
										const QStringList &alstColumn,
										const INT_LIST &alstKey)
{
	QString lstrRet = "";
	QString lstrDefaultCharset = QLatin1String("ENGINE=InnoDB DEFAULT CHARSET=utf8;");
	QString lstrKey = "";
	QString lstrTmp = "";
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
	lstrRet = QString("CREATE TABLE IF NOT EXISTS `%1`(")
			  .arg(astrTableName);
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

	return lstrRet;
}

// Create the bound id list from the column name
QStringList MySQLHandler::createBoundKey(const QStringList &alstColumn)
{
	QStringList llstBoundId;
	QString lstrTmp;

	foreach(lstrTmp, alstColumn)
	{
		lstrTmp = lstrTmp.trimmed();
		lstrTmp = lstrTmp.remove(QLatin1String(" "));
		lstrTmp.insert(0, QLatin1String(":"));

		llstBoundId << lstrTmp;
	}

	return llstBoundId;
}

/*
 * Param In:	astrTableName
 * Param In:	alstColumn
 * Param Out:	alstBoundKey
 */
QString MySQLHandler::getSQLInsertPrepared(const QString &astrTableName,
										   const QStringList &alstColumn,
										   QStringList &alstBoundKey)
{
	QString lstrRet = QLatin1String("INSERT INTO `%1` (");
	QString lstrValue = QLatin1String("VALUES (");
	int liCnt;

	alstBoundKey = createBoundKey(alstColumn);

	lstrRet = lstrRet.arg(astrTableName);

	for(liCnt = 0; liCnt < alstColumn.size(); liCnt++)
	{
		lstrRet += QLatin1String("`") + alstColumn[ liCnt ] + QLatin1String("`, ");
		lstrValue += alstBoundKey[ liCnt ] + QLatin1String(", ");
	}

	if(lstrRet.endsWith(QLatin1String(", ")))
		lstrRet.chop(2);
	if(lstrValue.endsWith(QLatin1String(", ")))
		lstrValue.chop(2);

	lstrRet += QLatin1String(") ") + lstrValue + QLatin1String(");");

	return lstrRet;
}

/*
 * Insert list of data entry to database
 */
bool MySQLHandler::updateDatabase(const DATA_LIST &alstData,
								  const QString &astrTableName)
{
	bool lblRet = false;
	QSqlDatabase db = QSqlDatabase::database();
	int liCount = 0;

	do
	{
		if (db.isOpen() == false)
		{
			DEF_LOG(QLatin1String("updateDatabase ---> Database not open!"));
			break;
		}

		QSqlQuery query;
		foreach (DataEntry data, alstData)
		{
			query = db.exec(data.getSQLString(astrTableName));
			liCount += query.numRowsAffected();
		}

		DEF_LOG(QString("\tRequest: %1 ---> Insert: %2")
				.arg(alstData.count())
				.arg(liCount));
		lblRet = true;
	} while (false);

	return lblRet;
}

/*
 * Check to see if an entry data existed in database or not
 */
bool MySQLHandler::checkExisted(DataEntry *apData,
								const QString &astrTableName,
								const QStringList &alstKey)
{
	bool lblRet = false;
	QString error;

	do
	{
		if (apData == NULL)
		{
			qDebug() << QLatin1String("Input pointer is null");
			break;
		}

		QSqlDatabase db = QSqlDatabase::database();

		if (db.isOpen() == false)
		{
			DEF_LOG(QLatin1String("Database not open!"));
			break;
		}

		QString select = apData->getSqlSelectString(astrTableName, alstKey);
		QSqlQuery query = db.exec(select);
		lblRet = query.numRowsAffected() > 0;
		error = db.lastError().text();
		if (error.isNull() == false && error.trimmed().isEmpty() == false)
			qDebug() << QLatin1String("last error: ") << error;
	} while (false);

	return lblRet;
}
