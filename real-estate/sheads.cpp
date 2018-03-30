#include "sheads.h"

sheads::sheads(QString url)
	:real_estate(url, 0)
{
}

void sheads::doWebLoadFinished()
{
	switch (meState)
	{
	case E_STATE_INIT:
		getTeamInfoLink();
		break;

	case E_STATE_GET_SUB_CATEGORY:
		getAgentList();
		break;

	case E_STATE_GET_DETAIL_INFO:
		parseData();
		break;

	default:
		break;
	}
}

void sheads::getTeamInfoLink()
{
	QString lstrLink;
	mCol = mpWebView->findAll("a");
	foreach (mElement, mCol)
	{
		lstrLink = mpWebView->getHrefURL(&mElement);
		if (lstrLink.indexOf("staff.asp?f_HeadOfficeID=") < 0)
			continue;

		mlstCategoryLink << lstrLink;
	}

	mlstCategoryLink.removeDuplicates();
	qDebug() << "Found team: " << mlstCategoryLink.count();

	if (gotoNextCategoryLink() == false)
		showFinishMsg("Can not goto next team link!");
}

void sheads::getAgentList()
{
	QString lstrLink;
	mCol = mpWebView->findAll("a");
	foreach (mElement, mCol)
	{
		lstrLink = mpWebView->getHrefURL(&mElement);
		if (lstrLink.indexOf("staff.asp?f_AgentID=") < 0)
			continue;

		mlstDataLink << lstrLink;
	}

	if (gotoNextCategoryLink() == false)
	{
		mlstDataLink.removeDuplicates();
		qDebug() << "Found agent: " << mlstDataLink.count();

		if (gotoNextDataLink() == false)
			showFinishMsg("Can not goto next data link!");
	}
}

void sheads::parseData()
{
	QWebElement lelement;
	QString lstrTmp;
	DataEntry ldata(mDbLstColName);

	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], itemURL());

	do
	{
		mElement = mpWebView->getElementById("title_h1");
		updateData(&ldata, eName, &mElement);

		mElement = mpWebView->getElementById("wrap_profile");
		DEF_CHECK_BREAK(mElement, QString("Can not find wrap_profile: %1")
						.arg(itemURL()));

		lelement = mElement.findFirst("strong");
		updateData(&ldata, eBusinessName, &lelement);

		lstrTmp = mElement.toPlainText();

		Utilities::extractAndUpdate(&ldata, mDbLstColName[ePhone], lstrTmp, "Phone:");
		Utilities::extractAndUpdate(&ldata, mDbLstColName[eMobile], lstrTmp, "Mobile:");
		Utilities::extractAndUpdate(&ldata, mDbLstColName[eEmail], lstrTmp, "Email:");

		mlstData << ldata;
	} while (false);

	saveData();

	if (gotoNextDataLink() == false)
	{
		getOfficeContact();
		saveData(&mlstData, true);

		showFinishMsg();
	}
}

void sheads::getOfficeContact()
{
	DataEntry ldata(mDbLstColName);

	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], itemURL());
	ldata.updateProperty(mDbLstColName[eName], "Shead Property");
	ldata.updateProperty(mDbLstColName[eAddress], "75 Archer Street, Chatswood NSW 2067");
	ldata.updateProperty(mDbLstColName[ePhone], " 02 8448 1200");
	ldata.updateProperty(mDbLstColName[eFax], "02 9411 2765");
	ldata.updateProperty(mDbLstColName[eEmail], "realestate@sheads.com.au");
	mlstData << ldata;
}
