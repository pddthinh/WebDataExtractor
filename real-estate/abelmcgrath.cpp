#include "abelmcgrath.h"

abelmcgrath::abelmcgrath(QString url, QWidget *apParent)
	:real_estate(url, apParent)
{
}

void abelmcgrath::doWebLoadFinished()
{
	switch (meState)
	{
	case E_STATE_INIT:
		getProfileList();
		break;

	case E_STATE_GET_DETAIL_INFO:
		parseData();
		break;

	default:
		break;
	}
}

void abelmcgrath::getProfileList()
{
	QString lstrLink;

	mCol = mpWebView->findAll("a");
	foreach (mElement, mCol)
	{
		lstrLink = mpWebView->getHrefURL(&mElement);
		if (lstrLink.indexOf("agent.aspx?aid=") < 0)
			continue;

		mlstDataLink << lstrLink;
	}

	qDebug() << "Found profile: " << mlstDataLink.size();

	if (gotoNextDataLink() == false)
		showFinishMsg("Unknown error: can not goto next data link");
}

void abelmcgrath::parseData()
{
	DataEntry ldata(mDbLstColName);
	QWebElement lelement;
	QString lstrTmp, url;

	do
	{
		url = mpWebView->url().toString();

		ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
		ldata.updateProperty(mDbLstColName[eItemLink], url);

		mElement = mpWebView->getElementById("ctl00_lblheading");
		updateData(&ldata, eName, &mElement);

		lelement = mElement.parent().nextSibling().nextSibling();
		if (lelement.tagName().toLower() != "table")
		{
			DEF_LOG(QString("Can not find contact container: %1")
					.arg(url));
			break;
		}
		lstrTmp = "title" + lelement.toPlainText() + "\n";

		Utilities::extractAndUpdate(&ldata, mDbLstColName[eBusinessName], lstrTmp, "title\t\t");
		Utilities::extractAndUpdate(&ldata, mDbLstColName[ePhone], lstrTmp, "Phone");
		Utilities::extractAndUpdate(&ldata, mDbLstColName[eMobile], lstrTmp, "Mobile");
		Utilities::extractAndUpdate(&ldata, mDbLstColName[eEmail], lstrTmp, "Email");

		mlstData << ldata;
	} while (false);

	saveData();

	if (gotoNextDataLink() == false)
	{
		saveData(&mlstData, true);
		showFinishMsg();
	}
}
