#include "architecture_base.h"

architecture_base::architecture_base(QString title, QWidget *apParent)
	: BaseWebEngine(apParent)
{
	// for using custom control, call this function then implement initExtControl()
	initCustomControl();

	setWindowTitle(title);

	if(mpDBLogger != NULL)
		mpDBLogger->setLogPath("architecture_com.sql");

	mpLog->setFilePath("architecture_com.txt");
	///////////////////////////////////////////////////////////

	miPageTotal = -1;
	miPageIdx = 1;
}

void architecture_base::initDatabase()
{
	mDbLstColName << "Name"
				  << "Address" << "Town" << "County" << "Country" << "Postcode" << "FullAddress"
				  << "Telephone" << "Fax"
				  << "Website" << "Email"
				  << "FirstName" << "LastName" << "FullName"
				  << "Item";
	mDbLstKey << eName << eEmail;
	mDbTableName = "architecture_com";
}

bool architecture_base::gotoNextResultPage()
{
	bool lblRet = false;

	do
	{
		setStatus(QString("Remain page: %1")
				  .arg(miPageTotal - miPageIdx));

		miPageIdx++;
		if (miPageIdx > miPageTotal)
			break;

		mElement = mpWebView->findFirstElement("input", "value", ">");
		mpWebView->doClick(&mElement);

		lblRet = true;
	} while (false);

	return lblRet;
}

int architecture_base::getTotalPage()
{
	QString lstrTmp;

	mElement = mpWebView->findFirstElement("input", "value", ">>");
	lstrTmp = mElement.attribute("onclick");
	lstrTmp.replace("javascript: SubmitForm('", "");
	lstrTmp.replace("')", "");
	if (lstrTmp.isEmpty() == false)
	{
		miPageTotal = lstrTmp.toInt();
		miPageIdx = 1;
	}

	return miPageTotal;
}

typedef enum {
	E_DATA_ADDRESS = 0,
	E_DATA_TEL,
	E_DATA_FAX,
	E_DATA_EMAIL,
	E_DATA_WEB,
	E_DATA_CONTACT,
	E_DATA_MAX
} ENU_DATA_TYPE;

QString gstrDataTypeKeyword[E_DATA_MAX] = {
	"Address",
	"Telephone",
	"Fax",
	"Email",
	"Website",
	"Contact"
};

void architecture_base::getDetailData(bool ablCompany)
{
	QString lstrTmp;
	QWebElement leTmp, leCheck;
	DataEntry ldata(mDbLstColName);
	TAG_ADDRESS ltAdd;
	TAG_PERSON_NAME ltName;

	mElement = mpWebView->getElementById("ribacontent");

	ldata.updateProperty(mDbLstColName[eItemLink], getURL());

	leTmp = mElement.findFirst("h4");
	if (ablCompany == false)
	{
		ltName = Utilities::parsePersonName(leTmp.toPlainText());
		ldata.updateProperty(mDbLstColName[eName], ltName.strFullname);
		ldata.updateProperty(mDbLstColName[eFirstname], ltName.strFirstname);
		ldata.updateProperty(mDbLstColName[eLastname], ltName.strLastName);
	}
	else
		updateData(&ldata, eName, &leTmp);

	mElement = mpWebView->getElementById("form1");
	mCol = mElement.findAll("tr");
	int i;
	int size = mCol.count();
	int key;
	QString strKey = "img[title=\"%1\"]";

	for (i = 0; i < size; i++)
	{
		leTmp = mCol[i];

		// find the appropriate data type
		for (key = 0; key < E_DATA_MAX; key++)
		{
			leCheck = leTmp.findFirst(strKey
										.arg(gstrDataTypeKeyword[key]));
			if (leCheck.isNull() == false)
				break;
		}

		if (key == E_DATA_MAX)
			continue;

		switch (key)
		{
		case E_DATA_ADDRESS:
		{
			lstrTmp = leTmp.toPlainText();
			int liTmp = lstrTmp.indexOf("\nUnited Kingdom:");
			if (liTmp > 0)
				lstrTmp.chop(lstrTmp.length() - liTmp);
			ltAdd = Utilities::parseUK_Add(lstrTmp, false);
			ldata.updateProperty(mDbLstColName[eAdd], ltAdd.strAddress);
			ldata.updateProperty(mDbLstColName[eCountry], ltAdd.strCountry);
			ldata.updateProperty(mDbLstColName[eCounty], ltAdd.strCounty);
			ldata.updateProperty(mDbLstColName[eFullAdd], ltAdd.strFullAddress);
			ldata.updateProperty(mDbLstColName[ePostCode], ltAdd.strPostcode);
			ldata.updateProperty(mDbLstColName[eTown], ltAdd.strTown);
			break;
		}

		case E_DATA_TEL:
			updateData(&ldata, eTel, &leTmp);
			break;

		case E_DATA_FAX:
			updateData(&ldata, eFax, &leTmp);
			break;

		case E_DATA_EMAIL:
			updateData(&ldata, eEmail, &leTmp);
			break;

		case E_DATA_WEB:
			updateData(&ldata, eWeb, &leTmp);
			break;

		case E_DATA_CONTACT:
			lstrTmp = leTmp.toPlainText();
			lstrTmp.replace("Mr", "");
			lstrTmp.replace("Ms", "");
			ltName = Utilities::parsePersonName(lstrTmp);
			ldata.updateProperty(mDbLstColName[eFirstname], ltName.strFirstname);
			ldata.updateProperty(mDbLstColName[eLastname], ltName.strLastName);
			ldata.updateProperty(mDbLstColName[eFullName], ltName.strFullname);
			break;
		} // switch
	} // for

	mlstData << ldata;

	if (mlstData.count() == 10)
	{
		DEF_LOG(QString("Save: %1")
				.arg(mlstData.count()));
		mpDBLogger->logDataList(mlstData);
		mlstData.clear();
	}

	if (gotoNextDataLink() == false)
	{
		if (mlstData.count() > 0)
		{
			DEF_LOG(QString("Save: %1")
					.arg(mlstData.count()));
			mpDBLogger->logDataList(mlstData);
			mlstData.clear();
		}

		showFinishMsg("Finished extract data");
	}
}
