#include "thepropertyexchange.h"

thepropertyexchange::thepropertyexchange(QString url, QWidget *apParent)
	:real_estate(url, apParent)
{
}

void thepropertyexchange::doWebLoadFinished()
{
	switch (meState)
	{
	case E_STATE_INIT:
		getAgentList();
		break;

	case E_STATE_GET_DETAIL_INFO:
		parseData();
		break;

	default:
		break;
	}
}

void thepropertyexchange::getAgentList()
{
	QWebElement lelement;
	QString url;

	do
	{
		url = mpWebView->url().toString();
		mElement = mpWebView->getElementById("ctl00_ContentPlaceHolder1_Repeater1");
		DEF_CHECK_BREAK(mElement, QString("Can not get the container: %1")
						.arg(url));

		mCol = mpWebView->findAllElement("td", "class", "agent", &mElement);
		foreach (mElement, mCol)
		{
			lelement = mElement.findFirst("a");
			mlstDataLink << mpWebView->getHrefURL(&lelement);
		}

		qDebug() << "Found agent: " << mlstDataLink.count();
	} while (false);

	if (gotoNextDataLink() == false)
		showFinishMsg("Unknown error: agent list is empty!");
}

void thepropertyexchange::parseData()
{
	QWebElement lelement;
	QString lstrTmp, url;
	DataEntry ldata(mDbLstColName);

	do
	{
		url = mpWebView->url().toString();

		ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
		ldata.updateProperty(mDbLstColName[eItemLink], url);

		mElement = mpWebView->findFirst("h1");
		DEF_CHECK_BREAK(mElement, QString("Can not find name: %1")
						.arg(url));

		lstrTmp = "name:" + mElement.toPlainText() + "\n";
		Utilities::extractAndUpdate(&ldata, mDbLstColName[eName], lstrTmp, "name:", "-");
		Utilities::extractAndUpdate(&ldata, mDbLstColName[eBusinessName], lstrTmp, "-");

		lelement = mElement.nextSibling();
		DEF_CHECK_BREAK(lelement, QString("Can not find container: %1")
						.arg(url));

		mElement = mpWebView->findFirstElement("table", "width", "100%", &lelement);
		DEF_CHECK_BREAK(lelement, QString("Can not find contact: %1")
						.arg(url));

		lstrTmp = mElement.toPlainText() + "\n";
		Utilities::extractAndUpdate(&ldata, mDbLstColName[ePhone], lstrTmp, "P:");
		Utilities::extractAndUpdate(&ldata, mDbLstColName[eMobile], lstrTmp, "M:");
		Utilities::extractAndUpdate(&ldata, mDbLstColName[eEmail], lstrTmp, "E:");

		mlstData << ldata;
	} while (false);

	saveData();

	if (gotoNextDataLink() == false)
	{
		// parse the contact info
		{
			ldata.updateProperty(mDbLstColName[eName], "THE PROPERTY EXCHANGE");
			ldata.updateProperty(mDbLstColName[eBusinessName], "");
			ldata.updateProperty(mDbLstColName[eAddress], "212 Nicholson Road, Subiaco WA 6008");
			ldata.updateProperty(mDbLstColName[ePhone], "9388 3988");
			ldata.updateProperty(mDbLstColName[eFax], " 9388 3987");
			ldata.updateProperty(mDbLstColName[eEmail], "info@thepropertyexchange.com.au");
			mlstData << ldata;
		}

		saveData(&mlstData, true);
		showFinishMsg();
	}
}
