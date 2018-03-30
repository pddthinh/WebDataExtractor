#include "hockingstuart.h"

hockingstuart::hockingstuart(QString url, QWidget *apParent)
	:real_estate(url, apParent)
{
}

void hockingstuart::doWebLoadFinished()
{
	switch (meState)
	{
	case E_STATE_INIT:
		getOfficeLink();
		break;

	case E_STATE_GET_DETAIL_INFO:
		parseData();
		break;

	default:
		break;
	}
}

void hockingstuart::getOfficeLink()
{
	QWebElement leTmp;
	QString lstrLink;

	do
	{
		mElement = mpWebView->findFirst("div[class=\"greyBoxSmall noTopMargin\"]");
		DEF_CHECK_BREAK(mElement, QString("Can not find office list: %1")
						.arg(mpWebView->url().toString()));
		mCol = mElement.findAll("a");
		foreach(leTmp, mCol)
		{
			lstrLink = mpWebView->getHrefURL(&leTmp);

			mlstOffice << lstrLink + "&group=1";
			mlstOffice << lstrLink + "&group=2";
		}
	} while (false);

	qDebug() << "Office links: " << mlstOffice.count();

	if (gotoNextOffice() == false)
		showFinishMsg();
}

bool hockingstuart::gotoNextOffice()
{
	bool lblRet = false;

	do
	{
		if (mlstOffice.isEmpty() == true)
			break;

		lblRet = true;
		meState = E_STATE_GET_DETAIL_INFO;
		loadPage(mlstOffice.takeFirst());
	} while (false);

	return lblRet;
}

void hockingstuart::parseOfficeContact()
{
	QWebElement lelement;
	DataEntry ldata(mDbLstColName);
	QString lstrTmp, lstrEmail;

	do
	{
		mElement = mpWebView->findFirst("div[class=\"greyBoxBig officeContacts\"]");
		DEF_CHECK_BREAK(mElement, QString("Can not find office contact: %1")
						.arg(mpWebView->url().toString()));
		mCol = mElement.findAll("div[class=\"contact  agent\"]");
		foreach (mElement, mCol)
		{
			ldata.clearData();

			ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
			ldata.updateProperty(mDbLstColName[eItemLink], mpWebView->url().toString());

			lelement = mElement.findFirst("h4");
			updateData(&ldata, eName, &lelement);

			lstrEmail.clear();
			lelement = mElement.findFirst("a");
			if (lelement.isNull() == false)
				lstrEmail = lelement.attribute("href").replace("mailto:", "");
			ldata.updateProperty(mDbLstColName[eEmail], lstrEmail);

			lstrTmp= mElement.toPlainText() + "\n";
			lstrTmp.replace(lstrEmail, "");
			lstrTmp.replace(ldata.getProperty(mDbLstColName[eName]), "");

			Utilities::extractAndUpdate(&ldata, mDbLstColName[ePhone], lstrTmp, "Telephone: ");
			Utilities::extractAndUpdate(&ldata, mDbLstColName[eFax], lstrTmp, "Facsimile: ");

			lstrTmp.replace("Telephone: " + ldata.getProperty(mDbLstColName[ePhone]), "");
			lstrTmp.replace("Facsimile: " + ldata.getProperty(mDbLstColName[eFax]), "");

			ldata.updateProperty(mDbLstColName[eAddress], lstrTmp);

			mlstData << ldata;
		}

		if (mlstData.count() > 0)
		{
			mpDBLogger->logDataList(mlstData);
			DEF_LOG(QString("Save: %1").arg(mlstData.count()));
			mlstData.clear();
		}
	} while (false);
}

void hockingstuart::parseData()
{
	QWebElement lelement;
	DataEntry ldata(mDbLstColName);
	QString lstrTmp;

	do
	{
		parseOfficeContact();

		// parse contact
		mCol = mpWebView->findAll("div[class*=\"staff \"]");
		foreach (mElement, mCol)
		{
			ldata.clearData();

			ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
			ldata.updateProperty(mDbLstColName[eItemLink], mpWebView->url().toString());

			lstrTmp.clear();
			lelement = mElement.findFirst("strong");
			if (lelement.isNull() == false)
				lstrTmp = lelement.toPlainText().replace("\n", " ");
			ldata.updateProperty(mDbLstColName[eName], lstrTmp);

			lelement = mElement.findFirst("span[class=\"pos\"]");
			updateData(&ldata, eBusinessName, &lelement);

			lstrTmp.clear();
			lelement = mElement.findFirst("a[class=\"emailAgent\"]");
			if (lelement.isNull() == false)
				lstrTmp = Utilities::extractValue(lelement.attribute("href"),
												  "mailto:", "?");
			ldata.updateProperty(mDbLstColName[eEmail], lstrTmp);

			lstrTmp = mElement.toPlainText();
			Utilities::extractAndUpdate(&ldata, mDbLstColName[ePhone], lstrTmp, "Mob:");

			mlstData << ldata;
		}
	} while (false);

	if (mlstData.count() == 10)
	{
		mpDBLogger->logDataList(mlstData);
		DEF_LOG(QString("Save: %1").arg(mlstData.count()));
		mlstData.clear();
	}

	if (gotoNextOffice() == false)
	{
		if (mlstData.count() > 0)
		{
			mpDBLogger->logDataList(mlstData);
			DEF_LOG(QString("Save: %1").arg(mlstData.count()));
			mlstData.clear();
		}

		showFinishMsg();
	}
}
