#include "mcsinstaller.h"

#include <QWebElement>
#include <QWebElementCollection>

#define BASEURL		"http://www.microgenerationcertification.org/consumers/installer-search?searchPaginationPage=0"
#define LINK_FILE	"installer-link.txt"

MCSInstaller::MCSInstaller(QWidget *apParent)
	: MCS(BASEURL, LINK_FILE, apParent)
{
	setWindowTitle("Installer - [microgenerationcertification.org]");

	if(mpDBLogger != NULL)
		mpDBLogger->setLogPath("MCSInstaller.sql");
	mpLog->setFilePath("MCSInstaller.txt");
}

void MCSInstaller::initDatabase()
{
	mDbLstColName << "Category"
				  << "Firm Name" << "Cer No"
				  << "Address" << "Town" << "County" << "Country" << "Postcode" << "FullAddress"
				  << "Telephone" << "Website" << "Email"
				  << "FirstName" << "LastName" << "FullName" << "Position";
	mDbLstKey << 1 << 2;
	mDbTableName = "MCS_Installer";
}

/**
 * Get searched items data list
 *
 * @brief MCSInstaller::extractSearchResult
 */
void MCSInstaller::extractSearchResult()
{
	QWebElement				leTable;
	QWebElementCollection	leLstResult;

	do
	{
		leTable = mpWebView->document().findFirst("table[class=\"mcsResultsTable\"]");
		DEF_CHECK_BREAK(leTable, "Can not get data result table");

		leLstResult = leTable.findAll("td[class=\"mcsResultsMoreInfo\"]");
		if(leLstResult.count() <= 0)
		{
			DEF_LOG("Can not get list of ResultMoreInfo elements");
			break;
		}

		foreach(QWebElement lElement, leLstResult)
			mlstDataLink << mstrBaseRootURL + lElement.findFirst("a").attribute("href");

		qDebug() << "Save " << mlstDataLink.count() << " items";
		DEF_LOG(QString("Save %2 items").arg(mlstDataLink.count()));
	} while(false);
}

enum enuDatabase
{
	eCateogry = 0,
	eFirmName, eCerNo,
	eAdd, eTown, eCounty, eCountry, ePostcode, eFullAdd,
	ePhone, eWeb, eEmail,
	eFirstName, eLastName, eFullName, ePosition
};

/**
 * Get item detail data
 *
 * @brief MCSInstaller::getDetailData
 */
void MCSInstaller::getDetailData()
{
	DataEntry		ldata(mDbLstColName);
	QWebElement		leElement;
	QString			lstrTmp, lstrDataTmp;
	TAG_ADDRESS		ltAdd;
	TAG_PERSON_NAME	ltName;

	QWebElementCollection leLst;

	do
	{
		// Category
		leElement = mpWebView->document().findFirst("div[class=\"mcsInstallerProductsBoxBottom\"]");
		DEF_CHECK_BREAK(leElement, QString("Can not find Category list element of [%1]").arg(getURL()));

		lstrTmp = "";
		leLst = leElement.findAll("li");
		foreach(leElement, leLst)
			lstrTmp += leElement.toPlainText() + ", ";

		if(lstrTmp.endsWith(", "))
			lstrTmp.chop(2);
		ldata.updateProperty(mDbLstColName[eCateogry], lstrTmp);

		///////////////////////////
		leElement = mpWebView->document().findFirst("div[class=\"mcsColumnsTwoOne\"]");
		DEF_CHECK_BREAK(leElement, QString("Can not find Column info element of [%1]").arg(getURL()));

		lstrTmp = leElement.findFirst("h1").toPlainText();
		ldata.updateProperty(mDbLstColName[eFirmName], lstrTmp);

		lstrTmp = leElement.toPlainText();

		Utilities::extractAndUpdate2(&ldata, mDbLstColName[eCerNo], lstrTmp, "Certificate Number:", "\n");

		lstrDataTmp = Utilities::extractValue2(lstrTmp, "Company Address\n", "\n");
		ltAdd = Utilities::parseUK_Add(lstrDataTmp);
		ldata.updateProperty(mDbLstColName[eFullAdd], ltAdd.strFullAddress);
		ldata.updateProperty(mDbLstColName[eAdd], ltAdd.strAddress);
		ldata.updateProperty(mDbLstColName[eTown], ltAdd.strTown);
		ldata.updateProperty(mDbLstColName[eCounty], ltAdd.strCounty);
		ldata.updateProperty(mDbLstColName[eCountry], ltAdd.strCountry);
		ldata.updateProperty(mDbLstColName[ePostcode], ltAdd.strPostcode);

		Utilities::extractAndUpdate2(&ldata, mDbLstColName[ePhone], lstrTmp, "Telephone:", "\n");
		Utilities::extractAndUpdate2(&ldata, mDbLstColName[eWeb], lstrTmp, "Website:", "\n");
		Utilities::extractAndUpdate2(&ldata, mDbLstColName[eEmail], lstrTmp, "Email:", "\n");

		lstrDataTmp = Utilities::extractValue2(lstrTmp, "Contact:", "\n");
		ltName = Utilities::parsePersonName(lstrDataTmp);
		ldata.updateProperty(mDbLstColName[eFirstName], ltName.strFirstname);
		ldata.updateProperty(mDbLstColName[eLastName], ltName.strLastName);
		ldata.updateProperty(mDbLstColName[eFullName], ltName.strFullname);

		Utilities::extractAndUpdate2(&ldata, mDbLstColName[ePosition], lstrTmp, "Contact Position:", "\n");

		mlstData << ldata;

		if(mlstData.count() == 20)
		{
			mpDBLogger->logDataList(mlstData);
			DEF_LOG(QString("Save %1 dataentry").arg(mlstData.count()));

			mlstData.clear();
		}

		// Go to next link
		if(gotoNextDataLink(E_STATE_GET_DETAIL_INFO) == false)
		{
			if(mlstData.count() > 0 )
			{
				mpDBLogger->logDataList(mlstData);
				DEF_LOG(QString("Save %1 dataentry").arg(mlstData.count()));
				mlstData.clear();
			}

			showFinishMsg();
		}
	}while(false);
}
