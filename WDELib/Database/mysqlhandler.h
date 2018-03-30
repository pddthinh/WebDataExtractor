#ifndef MYSQLHANDLER_H
#define MYSQLHANDLER_H

#include "WDE.h"
#include "WDEDef.h"
#include "Logger/logger.h"
#include <QString>
#include <QStringList>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>

using namespace nsLogger;
using namespace nsWDEDef;

namespace nsDatabase
{

class WDE_LIB_EXPORT MySQLHandler
{
public:
	MySQLHandler();
	MySQLHandler(Logger *apLog);

	bool initDatabase(const QString &astrTableName,
					  const QStringList &alstColumn,
					  const INT_LIST &alstKey,
					  QStringList &alstBoundKey);
	bool updateDatabase(const DATA_LIST &alstData, const QString &astrTableName);
	bool checkExisted(DataEntry *apData,
					  const QString &astrTableName,
					  const QStringList &alstKey);

public:	// Set-get properties
	void setHostname(QString astrHost) { mstrHostname = astrHost; }
	QString getHostname() const { return mstrHostname; }

	void setDBName(QString astrDBName) { mstrDBName = astrDBName; }
	QString getDBName() const { return mstrDBName; }

	void setUsername(QString astrUsername) { mstrUsername = astrUsername; }
	QString getUsername() const { return mstrUsername; }

	void setPassword(QString astrPassword) { mstrPassword = astrPassword; }
	QString getPassword() const { return mstrPassword; }

	void setCommentStart(QString astr) { mstrCommentStart = astr; }
	QString getCommentStart() const { return mstrCommentStart; }

	inline void setLogger(Logger *apLog) { mpLog = apLog; }

protected:
	void init();
	bool createConnection();
	void closeConnection();
	QString getSQLCreateTable(const QString &astrTableName,
							  const QStringList &alstColumn,
							  const INT_LIST &alstKey);
	QStringList createBoundKey(const QStringList &alstColumn);
	QString getSQLInsertPrepared(const QString &astrTableName,
								 const QStringList &alstColumn,
								 QStringList &alstBoundKey);

protected:
	QSqlQuery		*mpQuery;

// Database properties
private:
	QString			mstrHostname;
	QString			mstrDBName;
	QString			mstrUsername;
	QString			mstrPassword;
	QString			mstrCommentStart;
	QString			mstrErrMsgTemplate;

private:
	Logger			*mpLog;
	bool			mblCreateConnection;
	bool			mblPrepareSQL;
};

} // namespace nsDatabase

#endif // MYSQLHANDLER_H
