#ifndef POSTCODEDB_H
#define POSTCODEDB_H

#include <QObject>

#define DB_POSTCODE_TBL			"pc"
#define DB_POSTCODE_TBL_TMP		"pc_tmp"

class PostcodeDB : public QObject
{
	Q_OBJECT
public:
	explicit PostcodeDB(QObject *parent = 0);
	PostcodeDB(QString path){ mDBPath = path; }

	bool updateRegionCode(QString tblName = QLatin1String(""));
	bool generateCSVCode(QString tblName = QLatin1String(""));
	bool generateCSVCode2(QString tblName = QLatin1String(""), int extractedCount = 3,
						  QString pcSep = QLatin1String("::"));

protected:
	bool initDB();

	QString mDBPath;

signals:

public slots:

};

#endif // POSTCODEDB_H
