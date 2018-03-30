#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>

#include "../WDELib/WDEInclude.h"

using namespace nsDatabase;
using namespace nsBaseWebEngine;
using namespace nsWDEDef;

Q_DECLARE_METATYPE(INT_LIST);

class UtWDELibTest : public QObject
{
	Q_OBJECT

public:
	UtWDELibTest();

private Q_SLOTS:

	void testParseUKAdd_data();
	void testParseUKAdd();

	void testParseName_data();
	void testParseName();

	void testDBCreateTable_data();
	void testDBCreateTable();

	void testDataEntry();
	void testDataEntry_SqlSelect();

	void testParseYPauEmail_data();
	void testParseYPauEmail();
};

UtWDELibTest::UtWDELibTest()
{
}

void UtWDELibTest::testParseUKAdd_data()
{
	QTest::addColumn<QString>("iStrData");
	QTest::addColumn<QString>("istrInSep");
	QTest::addColumn<QString>("istrOutSep");
	QTest::addColumn<QString>("estrAdd");
	QTest::addColumn<QString>("estrTown");
	QTest::addColumn<QString>("estrCounty");
	QTest::addColumn<QString>("estrCountry");
	QTest::addColumn<QString>("estrPostcode");
	QTest::addColumn<QString>("estrFullAdd");

	QTest::newRow("normal") << "Unit 4 Maverick Business Centre\n292 Monkmoor Road,\nShrewsbury ,\nShropshire ,\nUK,\nSY2 5TF"
							<< "," << ", "
							<< "Unit 4 Maverick Business Centre, 292 Monkmoor Road"
							<< "Shrewsbury"
							<< "Shropshire"
							<< "UK"
							<< "SY2 5TF"
							<< "Unit 4 Maverick Business Centre, 292 Monkmoor Road, Shrewsbury, Shropshire, UK, SY2 5TF";
	QTest::newRow("data1") << "Unit H18\nThe Old Gas Works ,\n2 Michael Road ,\nLondon ,\nUK,\nSW6 2AD"
						   << "," << ", "
						   << "Unit H18, The Old Gas Works, 2 Michael Road"
						   << ""
						   << "London"
						   << "UK"
						   << "SW6 2AD"
						   << "Unit H18, The Old Gas Works, 2 Michael Road, London, UK, SW6 2AD";
	QTest::newRow("data2") << "21 Davids Close\nPETERBOROUGH,\nEngland,\nPE4 5AN"
						   << "," << ", "
						   << "21 Davids Close"
						   << ""
						   << "PETERBOROUGH"
						   << "England"
						   << "PE4 5AN"
						   << "21 Davids Close, PETERBOROUGH, England, PE4 5AN";
	QTest::newRow("data3") << "37-39 Shakespeare Street\nSouthport,\nMerseyside,\nEngland,\nPR8 5AB"
						   << "," << ", "
						   << "37-39 Shakespeare Street"
						   << "Southport"
						   << "Merseyside"
						   << "England"
						   << "PR8 5AB"
						   << "37-39 Shakespeare Street, Southport, Merseyside, England, PR8 5AB";

}

void UtWDELibTest::testParseUKAdd()
{
	//	TAG_ADDRESS ltAdd;

	//	QFETCH(QString, iStrData);
	//	QFETCH(QString, istrInSep);
	//	QFETCH(QString, istrOutSep);
	//	QFETCH(QString, estrAdd);
	//	QFETCH(QString, estrTown);
	//	QFETCH(QString, estrCounty);
	//	QFETCH(QString, estrCountry);
	//	QFETCH(QString, estrPostcode);
	//	QFETCH(QString, estrFullAdd);

	////	ltAdd = Utilities::parseUK_Add(iStrData, istrInSep, istrOutSep);

	//	QCOMPARE(ltAdd.strAddress, estrAdd);
	//	QCOMPARE(ltAdd.strTown, estrTown);
	//	QCOMPARE(ltAdd.strCounty, estrCounty);
	//	QCOMPARE(ltAdd.strCountry, estrCountry);
	//	QCOMPARE(ltAdd.strPostcode, estrPostcode);
	//	QCOMPARE(ltAdd.strFullAddress, estrFullAdd);
}

void UtWDELibTest::testParseName_data()
{
	QTest::addColumn<QString>("istrIn");
	QTest::addColumn<QString>("estrFirst");
	QTest::addColumn<QString>("estrLast");
	QTest::addColumn<QString>("estrFull");

	QTest::newRow("normal") << "Charles Dearlove" << "Charles" << "Dearlove" << "Charles Dearlove";
	QTest::newRow("data1") << "S Pearce" << "S" << "Pearce" << "S Pearce";
	QTest::newRow("data2") << "J D Blackhall" << "J D" << "Blackhall" << "J D Blackhall";
	QTest::newRow("data3") << "Griffiths" << "" << "Griffiths" << "Griffiths";
	QTest::newRow("data4") << "Marie-Louise Wilkins" << "Marie-Louise" << "Wilkins" << "Marie-Louise Wilkins";
	QTest::newRow("data5") << "Richard Van de Peer" << "Richard Van de" << "Peer" << "Richard Van de Peer";
}

void UtWDELibTest::testParseName()
{
	TAG_PERSON_NAME ltName;

	QFETCH(QString, istrIn);
	QFETCH(QString, estrFirst);
	QFETCH(QString, estrLast);
	QFETCH(QString, estrFull);

	ltName = Utilities::parsePersonName(istrIn);

	QCOMPARE(ltName.strFirstname, estrFirst);
	QCOMPARE(ltName.strLastName, estrLast);
	QCOMPARE(ltName.strFullname, estrFull);
}

void UtWDELibTest::testDBCreateTable_data()
{
	QTest::addColumn<QString>("istrTableName");
	QTest::addColumn<QStringList>("ilstColumn");
	QTest::addColumn<INT_LIST>("ilstKey");
	QTest::addColumn<QString>("estrSQLQuery");

	QTest::newRow("normal") << "testdatabase"
							<< (QStringList() << "key" << "value")
							<< (QList<int>() << 0)
							<< "CREATE TABLE `testdatabase`(`key` VARCHAR(200), `value` LONGTEXT, PRIMARY KEY (`key`)) ENGINE=InnoDB DEFAULT CHARSET=utf8;";

	QTest::newRow("normal2") << "www_microgenerationcertification_org_dummy"
							 << (QStringList() << "Category" << "Firm name" << "Certificate No" << "Address" << "Town" << "County" << "Country" << "Postcode" << "Full Address" << "Telephone" << "Fax" << "Email" << "Website" << "Contact Firstname" << "Contact Surname" << "Full name" << "Position")
							 << (QList<int>() << 0 << 1 << 8 << 9)
							 << "CREATE TABLE `www_microgenerationcertification_org_dummy`(`Category` VARCHAR(200), `Firm name` VARCHAR(200), `Certificate No` LONGTEXT, `Address` LONGTEXT, `Town` LONGTEXT, `County` LONGTEXT, `Country` LONGTEXT, `Postcode` LONGTEXT, `Full Address` VARCHAR(200), `Telephone` VARCHAR(200), `Fax` LONGTEXT, `Email` LONGTEXT, `Website` LONGTEXT, `Contact Firstname` LONGTEXT, `Contact Surname` LONGTEXT, `Full name` LONGTEXT, `Position` LONGTEXT, PRIMARY KEY (`Category`, `Firm name`, `Full Address`, `Telephone`)) ENGINE=InnoDB DEFAULT CHARSET=utf8;";
}

void UtWDELibTest::testDBCreateTable()
{
	SQLQueryBuilder	lBuilder;

	QFETCH(QString, istrTableName);
	QFETCH(QStringList, ilstColumn);
	QFETCH(INT_LIST, ilstKey);
	QFETCH(QString, estrSQLQuery);

	QCOMPARE(lBuilder.getSQLCreateTable(istrTableName, ilstColumn, ilstKey), estrSQLQuery);
}

void UtWDELibTest::testDataEntry()
{
	//    {
	//        DataEntry lData(QStringList());

	//        lData.addProperty("Key1", "Val1");
	//        lData.addProperty("Key2", "Val2");
	//        lData.addProperty("Key3", "Val3");

	//        QVERIFY(lData.getProperty("Key1") == "Val1");
	//        QVERIFY(lData.getProperty("Key2") == "Val2");
	//        QVERIFY(lData.getProperty("Key3") == "Val3");
	//    }

	{
		DataEntry lData(QStringList() << "Col1" << "Col2" << "Col3");

		lData.updateProperty("Col1", "Val1");
		lData.updateProperty("Col2", "Val2");
		lData.updateProperty("Col3", "Val3, val3.1; val3'3,");

		QVERIFY(lData.getProperty("Col1") == "Val1");
		QVERIFY(lData.getProperty("Col2") == "Val2");
		QVERIFY(lData.getProperty("Col3") == "Val3, val3.1; val3''3,");
		//        QWARN(lData.getCSVString().toLatin1());
		QVERIFY(lData.getCSVString() == "\"Val1\", \"Val2\", \"Val3, val3.1; val3''3,\"");
	}
}

void UtWDELibTest::testDataEntry_SqlSelect()
{
	{
		DataEntry lData(QStringList() << "0Name" << "1PostTown" << "2PostCode");

		lData.updateProperty("0Name", "Val1");
		lData.updateProperty("1PostTown", "Val2");
		lData.updateProperty("2PostCode", "Val3, val3.1; val3'3,");

		//		qDebug() << lData.getSqlSelectString("table");

		//		QVERIFY(lData.getSqlSelectString("table") ==
		//				"SELECT * FROM `table` WHERE"
		//				"(`0Name` LIKE 'Val1') AND"
		//				"(`1PostTown` LIKE 'Val2') AND"
		//				"(`2PostCode` LIKE 'Val3, val3.1; val3''3,')");
	}
}

void UtWDELibTest::testParseYPauEmail_data()
{
	QTest::addColumn<QString>("iData");
	QTest::addColumn<QString>("eEmail");

	QTest::newRow("r1") << "mailto:info%40mmj.com.au?subject=Enquiry%2C%20sent%20from%20yellowpages.com.au&body=%0A%0A%0A%0A%0A------------------------------------------%0AEnquiry%20via%20yellowpages.com.au%0Ahttp%3A%2F%2Fyellowpages.com.au%2Fnsw%2Fwollongong%2Fmmj-real-estate-wollongong-14892546-listing.html%3Fcontext%3DbusinessTypeSearch"
						<< "info@mmj.com.au";
	QTest::newRow("r2") << "mailto:vanessajeffcoat%40mcgrath.com.au?subject=Enquiry%2C%20sent%20from%20yellowpages.com.au&body=%0A%0A%0A%0A%0A------------------------------------------%0AEnquiry%20via%20yellowpages.com.au%0Ahttp%3A%2F%2Fyellowpages.com.au%2Fnsw%2Fwollongong%2Fmcgrath-estate-agents-14647043-listing.html%3Fcontext%3DbusinessTypeSearch"
						<< "vanessajeffcoat@mcgrath.com.au";
	QTest::newRow("r3") << "mailto:sales%40pfrealestate.com.au?subject=Enquiry%2C%20sent%20from%20yellowpages.com.au&body=%0A%0A%0A%0A%0A------------------------------------------%0AEnquiry%20via%20yellowpages.com.au%0Ahttp%3A%2F%2Fyellowpages.com.au%2Fnsw%2Fwollongong%2Fpeter-fitzgerald-real-estate-12973703-listing.html%3Fcontext%3DbusinessTypeSearch"
						<< "sales@pfrealestate.com.au";
	QTest::newRow("r4") << "mailto:nataliec%40cooperpropertymanagement.com.au?subject=Enquiry%2C%20sent%20from%20yellowpages.com.au&body=%0A%0A%0A%0A%0A------------------------------------------%0AEnquiry%20via%20yellowpages.com.au%0Ahttp%3A%2F%2Fyellowpages.com.au%2Fnsw%2Fwollongong%2Fcooper-cooper-property-management-14031895-listing.html%3Fcontext%3DbusinessTypeSearch"
						<< "nataliec@cooperpropertymanagement.com.au";
	QTest::newRow("r5") << "mailto:micalekosprosiz@gmail.com"
						<< "micalekosprosiz@gmail.com";
}

void UtWDELibTest::testParseYPauEmail()
{
	QFETCH(QString, iData);
	QFETCH(QString, eEmail);

	QString lstrEmail;
	QUrl url(iData);
	lstrEmail = url.path().replace("%40", "@");

	QCOMPARE(lstrEmail, eEmail);
}

QTEST_MAIN(UtWDELibTest);

#include "utwdelibtest.moc"
