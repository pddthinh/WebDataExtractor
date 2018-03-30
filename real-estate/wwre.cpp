#include "wwre.h"

wwre::wwre(QString url, QWidget *apParent)
	:real_estate(url, apParent)
{
	mstrOtherContact = "http://www.wwre.com.au/Contact/Tradespeople";
}

void wwre::doStart()
{
	mlstCategoryLink << "http://www.wwre.com.au/About/Our-Team"
					 << "http://www.wwrewp.com.au/About/Our-Team";

	if (gotoNextCategoryLink(E_STATE_INIT) == false)
		showFinishMsg("Unknown error!");
}

void wwre::doWebLoadFinished()
{
	switch (meState)
	{
	case E_STATE_INIT:
		getProfileList();
		break;

	case E_STATE_GET_DETAIL_INFO:
		parseData();
		break;

	case E_STATE_SUB1:
		parseOtherContact();
		break;

	default:
		break;
	}
}

void wwre::getProfileList()
{
	mCol = mpWebView->findAllElement("a", "class", "click-thru");
	foreach (mElement, mCol)
		mlstDataLink << mpWebView->getHrefURL(&mElement);

	if (gotoNextCategoryLink(E_STATE_INIT) == false)
	{
		qDebug() << "Found profile: " << mlstDataLink.count();

		// start parsing
		if (gotoNextDataLink() == false)
			showFinishMsg();
	}
}

void wwre::parseData()
{
	QWebElement lelement;
	QString url, lstrTmp;
	DataEntry ldata(mDbLstColName);

	do
	{
		url = mpWebView->url().toString();
		mElement = mpWebView->findFirstDiv("class", "section-b");
		DEF_CHECK_BREAK(mElement, QString("Can not get data container: %1")
						.arg(url));

		ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
		ldata.updateProperty(mDbLstColName[eItemLink], url);

		lelement = mElement.findFirst("h3");
		updateData(&ldata, eName, &lelement);

		lelement = mElement.findFirst("strong");
		updateData(&ldata, eBusinessName, &lelement);

		lstrTmp = mElement.toPlainText() + "\n";
		Utilities::extractAndUpdate(&ldata, mDbLstColName[ePhone], lstrTmp, "Phone");
		Utilities::extractAndUpdate(&ldata, mDbLstColName[eMobile], lstrTmp, "Mobile");
		Utilities::extractAndUpdate(&ldata, mDbLstColName[eEmail], lstrTmp, "Email");

		mlstData << ldata;
	} while (false);

	saveData();

	if (gotoNextDataLink() == false)
	{
		saveData(&mlstData, true);
		getSiteContact();

		// start parsing TradeContact
		meState = E_STATE_SUB1;
		loadPage(mstrOtherContact);
	}
}

void wwre::getSiteContact()
{
	DataEntry ldata(mDbLstColName);

	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], "http://www.wwre.com.au/Contact-Us");
	ldata.updateProperty(mDbLstColName[eName], "Drummoyne Office");
	ldata.updateProperty(mDbLstColName[eAddress], "200 Victoria Rd, Drummoyne NSW 2047");
	ldata.updateProperty(mDbLstColName[ePhone], "(02) 9719 8288");
	ldata.updateProperty(mDbLstColName[eFax], "(02) 9819 6692");
	ldata.updateProperty(mDbLstColName[eEmail], "contact@wwre.com.au");
	mlstData << ldata;

	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], "http://www.wwrewp.com.au/Contact-Us");
	ldata.updateProperty(mDbLstColName[eName], "Wentworth Point");
	ldata.updateProperty(mDbLstColName[eAddress], "21 Hill Road, Wentworth Point NSW 2127");
	ldata.updateProperty(mDbLstColName[ePhone], "(02) 9648 2005");
	ldata.updateProperty(mDbLstColName[eFax], "(02) 9648 1577");
	ldata.updateProperty(mDbLstColName[eEmail], "enquiries@wwrewp.com.au");
	mlstData << ldata;

	saveData(&mlstData, true);
}

void wwre::parseOtherContact()
{
	QWebElement lelement;
	QString lstrTmp, url, lstrEmail;
	DataEntry ldata(mDbLstColName);

	do
	{
		url = mpWebView->url().toString();
		mElement = mpWebView->findFirstDiv("class", "pageText");
		DEF_CHECK_BREAK(mElement, QString("Can not get element: %1")
						.arg(url));

		mCol = mElement.findAll("p");
		foreach (lelement, mCol)
		{
			lstrTmp = lelement.toPlainText() + "\n";
			lstrEmail = Utilities::extractValue(lstrTmp, "Email: ", "\n");
			if (lstrEmail.isEmpty())
				continue;

			ldata.clearData();

			ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
			ldata.updateProperty(mDbLstColName[eItemLink], url);

			ldata.updateProperty(mDbLstColName[eEmail], lstrEmail);
			Utilities::extractAndUpdate(&ldata, mDbLstColName[ePhone], lstrTmp, "Bus: ");
			Utilities::extractAndUpdate(&ldata, mDbLstColName[eMobile], lstrTmp, "Mobile : ");
			Utilities::extractAndUpdate(&ldata, mDbLstColName[eFax], lstrTmp, "Fax: ");

			lstrTmp.replace("Email: " + lstrEmail, "");
			lstrTmp.replace("Bus: " + ldata.getProperty(mDbLstColName[ePhone]), "");
			lstrTmp.replace("Mobile : " + ldata.getProperty(mDbLstColName[eMobile]), "");
			lstrTmp.replace("Fax: " + ldata.getProperty(mDbLstColName[eFax]), "");
			lstrTmp.replace(lelement.findFirst("strong").toPlainText(), "");
			ldata.updateProperty(mDbLstColName[eName], lstrTmp);

			mlstData << ldata;
		}

		saveData(&mlstData, true);
	} while (false);

	showFinishMsg();
}
