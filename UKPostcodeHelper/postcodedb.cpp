#include "postcodedb.h"
#include <QtSql>
#include <QRegExp>
#include <QTextStream>

#define UK_PC_REGION_PATTERN		QLatin1String("[A-Z]{1,2}[0-9R][0-9A-Z]?")

PostcodeDB::PostcodeDB(QObject *parent) :
	QObject(parent)
{
}

bool PostcodeDB::initDB()
{
	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName(mDBPath);

	return db.open();
}

bool PostcodeDB::updateRegionCode(QString tblName)
{
	bool blRet = false;

	do
	{
		if (initDB() == false)
		{
			qDebug() << QLatin1String("Failed to open database: ") << mDBPath;
			break;
		}

		QSqlQuery query;

		// First create new table
		{
			if (tblName.isEmpty() == true)
				tblName = DB_POSTCODE_TBL;

			query.exec(QString("DROP TABLE %1").arg(DB_POSTCODE_TBL));

			QString sql;
			sql = QString("CREATE TABLE \"%1\" ("
						  "\"pc\" VARCHAR(20) PRIMARY KEY,"
						  "\"rc\" VARCHAR(10)"
						  ")")
					.arg(DB_POSTCODE_TBL);
			if (query.exec(sql) == false)
			{
				qDebug() << QLatin1String("Failed to create table: ") << tblName;
				break;
			}
		}

		query.clear();
		query.prepare(QString("SELECT * FROM \"%1\"").arg(DB_POSTCODE_TBL_TMP));

		if (query.exec() == false)
		{
			qDebug() << QLatin1String("Failed to select data from: ")
					 << QLatin1String(DB_POSTCODE_TBL_TMP);
			break;
		}

		int idx = query.record().indexOf("pc");

		QSqlQuery insert;
		insert.prepare(QString("INSERT INTO \"%1\"(\"%2\", \"%3\") VALUES(?, ?)")
					   .arg(tblName)
					   .arg("pc")
					   .arg("rc"));

		int total = 0;
		int counter = 0;
		int bRows = 0;

		QString postcode;
		QString rcode;
		QRegExp	regEx;
		QVariantList pcs;
		QVariantList rcs;
		while (query.next())
		{
			postcode = query.value(idx).toString();
			pcs << postcode;

			// find and extract the region code
			do
			{
				rcode.clear();

				regEx.setPattern(UK_PC_REGION_PATTERN);
				if (regEx.indexIn(postcode) == -1)
				{
					qDebug() << QLatin1String("Failed to search region code for ") << postcode;
					break;
				}

				rcode = regEx.cap();
			} while (0);
			rcs << rcode;

			bRows ++;
			if (bRows > 1024)
			{
				bRows = 0;

				insert.clear();
				insert.prepare(QString("INSERT INTO \"%1\"(\"%2\", \"%3\") VALUES(?, ?)")
							   .arg(tblName)
							   .arg("pc")
							   .arg("rc"));
				insert.addBindValue(pcs);
				insert.addBindValue(rcs);

				if (insert.execBatch() == false)
					qDebug() << insert.lastError() << " at index: " << total;

				pcs.clear();
				rcs.clear();
			}

			counter++;
			if (counter > 500)
			{
				total += counter;
				qDebug() << QLatin1String("Processing: ") << total;
				counter = 0;
			}
		}

		blRet = true;
	} while (0);

	return blRet;
}

bool PostcodeDB::generateCSVCode(QString tblName)
{
	bool blRet = false;

	do
	{
		if (initDB() == false)
		{
			qDebug() << QLatin1String("Failed to open database: ") << mDBPath;
			break;
		}

		if (tblName.isEmpty() == true)
			tblName = DB_POSTCODE_TBL;

		QFile file(QString("/tmp/%1.csv").arg(tblName));
		if (file.open(QFile::Truncate |
					  QFile::Text |
					  QFile::WriteOnly) == false)
		{
			qDebug() << QLatin1String("Failed to create CSV file!");
			break;
		}

		QTextStream csvFile(&file);

		// write titles
		csvFile << QLatin1String("\"pc\", \"rc\"\n");

		QSqlQuery query;
		query.prepare(QString("SELECT * FROM \"%1\"").arg(DB_POSTCODE_TBL_TMP));

		if (query.exec() == false)
		{
			qDebug() << QLatin1String("Failed to select data from: ")
					 << QLatin1String(DB_POSTCODE_TBL_TMP);
			break;
		}

		int idx = query.record().indexOf("pc");
		int total = 0;
		int counter = 0;

		QString postcode;
		QString rcode;
		QRegExp	regEx;

		while (query.next())
		{
			postcode = query.value(idx).toString();

			// find and extract the region code
			do
			{
				rcode.clear();

				regEx.setPattern(UK_PC_REGION_PATTERN);
				if (regEx.indexIn(postcode) == -1)
				{
					qDebug() << QLatin1String("Failed to search region code for ") << postcode;
					break;
				}

				rcode = regEx.cap();
			} while (0);

			csvFile << postcode << ", " << rcode << "\n";

			counter ++;
			if (counter >= 1024)
			{
				total += counter;
				counter = 0;

				qDebug() << QLatin1String("Exported: ") << total;
			}
		} // while

		if (counter > 0)
		{
			total += counter;
			qDebug() << QLatin1String("Exported: ") << total;
		}

		blRet = true;
	} while (0);

	return blRet;
}

bool PostcodeDB::generateCSVCode2(QString tblName, int extractedCount, QString pcSep)
{
	bool blRet = false;

	do
	{
		if (initDB() == false)
		{
			qDebug() << QLatin1String("Failed to open database: ") << mDBPath;
			break;
		}

		if (tblName.isEmpty() == true)
			tblName = DB_POSTCODE_TBL;

		QFile file(QString("/tmp/%1.csv").arg(tblName));
		if (file.open(QFile::Truncate |
					  QFile::Text |
					  QFile::WriteOnly) == false)
		{
			qDebug() << QLatin1String("Failed to create CSV file!");
			break;
		}

		QTextStream csvFile(&file);

		// write titles
		csvFile << QLatin1String("\"rc\", \"pc list\"\n");

		QSqlQuery query;
		query.prepare(QString("SELECT * FROM \"%1\" ORDER BY \"pc\"").arg(DB_POSTCODE_TBL_TMP));

		if (query.exec() == false)
		{
			qDebug() << QLatin1String("Failed to select data from: ")
					 << QLatin1String(DB_POSTCODE_TBL_TMP);
			break;
		}

		int idx = query.record().indexOf(QLatin1String("pc"));
		int total = 0;
		int counter = 0;

		QString postcode;
		QString rcode;
		QRegExp	regEx;
		QString pcList;
		QString preRC = "";
		int exIdx = extractedCount;

		while (query.next())
		{
			postcode = query.value(idx).toString();

			// find and extract the region code
			do
			{
				rcode.clear();

				regEx.setPattern(UK_PC_REGION_PATTERN);
				if (regEx.indexIn(postcode) == -1)
				{
					qDebug() << QLatin1String("Failed to search region code for ") << postcode;
					break;
				}

				rcode = regEx.cap();
			} while (0);

			// check if rc is changed
			if (preRC == "")
				preRC = rcode;

			if (preRC != rcode)
			{
				if (pcList.length() > 0)
				{
					// write out the data
					if (pcList.endsWith(pcSep))
						pcList.chop(pcSep.length());

					csvFile << preRC << QLatin1String(", ") << pcList << QLatin1String("\n");

					counter ++;
					if (counter >= 1024)
					{
						total += counter;
						counter = 0;

						qDebug() << QLatin1String("Exported: ") << total;
					}
				}

				exIdx = extractedCount;
				pcList.clear();

				preRC = rcode;
			}

			do
			{
				if (exIdx <= 0)
					break;

				// collect the postcode
				pcList += postcode + pcSep;
				exIdx --;
			} while (0);
		} // while

		if (pcList.length() > 0)
		{
			// write out the data
			if (pcList.endsWith(pcSep))
				pcList.chop(pcSep.length());

			csvFile << preRC << QLatin1String(", ") << pcList << QLatin1String("\n");

			counter ++;
			if (counter >= 1024)
			{
				total += counter;
				counter = 0;

				qDebug() << QLatin1String("Exported: ") << total;
			}
		}

		if (counter > 0)
		{
			total += counter;
			qDebug() << QLatin1String("Exported: ") << total;
		}

		blRet = true;
	} while (0);

	return blRet;
}
