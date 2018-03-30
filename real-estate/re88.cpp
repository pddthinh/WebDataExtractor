#include "re88.h"

re88::re88(QString url, QWidget *apParent)
	:real_estate(url, apParent)
{
}

void re88::doWebLoadFinished()
{
	switch (meState)
	{
	case E_STATE_INIT:
		getAgentProfile();
		break;

	case E_STATE_GET_DETAIL_INFO:
		parseData();
		break;

	default:
		break;
	}
}

void re88::getAgentProfile()
{
	QWebElement lelement;
	QString lstrTmp;

	mCol = mpWebView->findAll("a");
	foreach (lelement, mCol)
	{
		lstrTmp = mpWebView->getHrefURL(&lelement);
		if (lstrTmp.indexOf("agent.aspx?aid=") < 0)
			continue;

		mlstDataLink << lstrTmp;
	}

	mlstDataLink.removeDuplicates();
	qDebug() << "Found agent: " << mlstDataLink.count();
	if (gotoNextDataLink() == false)
		showFinishMsg("Data list is empty");
}

void re88::parseData()
{
	QWebElement lelement;
	DataEntry ldata(mDbLstColName);
	QString lstrTmp;
	QString url;

	do
	{
		url = mpWebView->url().toString();

		ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
		ldata.updateProperty(mDbLstColName[eItemLink], url);

		lelement = mpWebView->findFirst("h1");
		lstrTmp.clear();
		if (lelement.isNull() == false)
			lstrTmp = lelement.toPlainText().replace("'S PROFILE", "");
		ldata.updateProperty(mDbLstColName[eName], lstrTmp);

		lelement = lelement.nextSibling();
		DEF_CHECK_BREAK(lelement, QString("Can not find contact container: %1")
						.arg(url));

		lstrTmp = lelement.toPlainText() + "\n";
		Utilities::extractAndUpdate(&ldata, mDbLstColName[eBusinessName], lstrTmp, "\t\t\n");
		Utilities::extractAndUpdate(&ldata, mDbLstColName[ePhone], lstrTmp, "P:");
		Utilities::extractAndUpdate(&ldata, mDbLstColName[eMobile], lstrTmp, "M:");
		Utilities::extractAndUpdate(&ldata, mDbLstColName[eEmail], lstrTmp, "E:");

		mlstData << ldata;
	} while (false);

	saveData();

	if (gotoNextDataLink() == false)
	{
		saveData(&mlstData, true);
		showFinishMsg();
	}
}
