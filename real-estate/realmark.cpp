#include "realmark.h"

realmark::realmark(QString url, QWidget *apParent)
	:real_estate(url, apParent)
{
}

void realmark::doWebLoadFinished()
{
	switch (meState)
	{
	case E_STATE_INIT:
//		getProfileList();
		getOfficeContact();
		break;

	case E_STATE_GET_DETAIL_INFO:
		parseData();
		break;

	default:
		break;
	}
}

void realmark::getProfileList()
{
	QWebElement lelement;
	QString lstrLink;

	mCol = mpWebView->findAllDiv("class", "staffBox");
	foreach (mElement, mCol)
	{
		lelement = mElement.findFirst("a");
		if (lelement.isNull() == true)
			continue;

		lstrLink = mpWebView->getHrefURL(&lelement);
		if (lstrLink.indexOf("staffMember.cfm") < 0)
			continue;

		mlstDataLink << lstrLink;
	}

	qDebug() << "Found profile: " << mlstDataLink.count();

	if (gotoNextDataLink() == false)
		showFinishMsg();
}

void realmark::parseData()
{
	QWebElement lelement;
	QString lstrTmp, url;
	DataEntry ldata(mDbLstColName);

	do
	{
		url = mpWebView->url().toString();
		ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
		ldata.updateProperty(mDbLstColName[eItemLink], url);

		mElement = mpWebView->findFirstDiv("class", "main-column page2");
		DEF_CHECK_BREAK(mElement, QString("Can not get detail info: %1")
						.arg(url));

		lelement = mElement.findFirst("h1");
		updateData(&ldata, eName, &lelement);

		lelement = mElement.findFirst("h3");
		updateData(&ldata, eBusinessName, &lelement);

		mElement = mpWebView->findFirstDiv("class", "sidebar");
		DEF_CHECK_BREAK(mElement, QString("Can not get sidebar: %1")
						.arg(url));
		lelement = mElement.findFirst("ul");
		DEF_CHECK_BREAK(lelement, QString("Can not get email: %1")
						.arg(url));
		lelement = lelement.findFirst("a");
		lstrTmp.clear();
		if (lelement.isNull() == false)
			lstrTmp = lelement.attribute("href").replace("mailto:", "");
		ldata.updateProperty(mDbLstColName[eEmail], lstrTmp);

		lelement = mElement.findFirst("p");
		lstrTmp = lelement.toPlainText() + "\n";

		Utilities::extractAndUpdate(&ldata, mDbLstColName[eMobile], lstrTmp, "M. ");
		Utilities::extractAndUpdate(&ldata, mDbLstColName[ePhone], lstrTmp, "Ph. ");
		Utilities::extractAndUpdate(&ldata, mDbLstColName[eFax], lstrTmp, "Fax. ");

		lstrTmp.replace("M. " + ldata.getProperty(mDbLstColName[eMobile]), "");
		lstrTmp.replace("Ph. " + ldata.getProperty(mDbLstColName[ePhone]), "");
		lstrTmp.replace("Fax. " + ldata.getProperty(mDbLstColName[eFax]), "");

		ldata.updateProperty(mDbLstColName[eAddress], lstrTmp);

		mlstData << ldata;
	} while (false);

	saveData();

	if (gotoNextDataLink() == false)
	{
		saveData(&mlstData, true);
		getOfficeContact();
		showFinishMsg();
	}
}

void realmark::getOfficeContact()
{
	DataEntry ldata(mDbLstColName);

	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], "http://realmark.com.au/contact.cfm");
	ldata.updateProperty(mDbLstColName[eName], "Realmark Real Estate Group");
	ldata.updateProperty(mDbLstColName[eAddress], "658 Newcastle Street, LEEDERVILLE WA 6903");
	ldata.updateProperty(mDbLstColName[ePhone], "08 9328 0999");
	ldata.updateProperty(mDbLstColName[eFax], "08 9227 9229");
	ldata.updateProperty(mDbLstColName[eEmail], "reception@realmark.com.au");
	mlstData << ldata;
	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], "http://realmark.com.au/contact.cfm");
	ldata.updateProperty(mDbLstColName[eName], "Realmark Coastal - North Beach");
	ldata.updateProperty(mDbLstColName[eAddress], "117a Flora Tce, North Beach WA 6020");
	ldata.updateProperty(mDbLstColName[ePhone], "08 9400 2929");
	ldata.updateProperty(mDbLstColName[eFax], " 08 9246 9370");
	ldata.updateProperty(mDbLstColName[eEmail], "northbeach@realmark.com.au");
	mlstData << ldata;
	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], "http://realmark.com.au/contact.cfm");
	ldata.updateProperty(mDbLstColName[eName], "Realmark Coastal - Scarborough");
	ldata.updateProperty(mDbLstColName[eAddress], "257 West Coast Highway, SCARBOROUGH WA 6019");
	ldata.updateProperty(mDbLstColName[ePhone], "08 9428 2828");
	ldata.updateProperty(mDbLstColName[eFax], "08 9245 4291");
	ldata.updateProperty(mDbLstColName[eEmail], "scarborough@realmark.com.au ");
	mlstData << ldata;
	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], "http://realmark.com.au/contact.cfm");
	ldata.updateProperty(mDbLstColName[eName], "Realmark Commercial");
	ldata.updateProperty(mDbLstColName[eAddress], "Level 1, 658 Newcastle Street, Leederville WA 6903 ");
	ldata.updateProperty(mDbLstColName[ePhone], "08 9328 0909");
	ldata.updateProperty(mDbLstColName[eFax], "08 9308 0908");
	ldata.updateProperty(mDbLstColName[eEmail], "commercial@realmark.com.au ");
	mlstData << ldata;
	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], "http://realmark.com.au/contact.cfm");
	ldata.updateProperty(mDbLstColName[eName], "Realmark Floreat");
	ldata.updateProperty(mDbLstColName[eAddress], "Suite 1, 440 Cambridge Street, Floreat WA 6014 ");
	ldata.updateProperty(mDbLstColName[ePhone], "08 6382 6100");
	ldata.updateProperty(mDbLstColName[eFax], "08 6382 6106");
	ldata.updateProperty(mDbLstColName[eEmail], "floreat@realmark.com.au ");
	mlstData << ldata;
	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], "http://realmark.com.au/contact.cfm");
	ldata.updateProperty(mDbLstColName[eName], "Realmark Hillarys ");
	ldata.updateProperty(mDbLstColName[eAddress], "Cnr Whitfords Ave & Endeavour Rd, HILLARYS WA 6025 ");
	ldata.updateProperty(mDbLstColName[ePhone], "08 9307 0999");
	ldata.updateProperty(mDbLstColName[eFax], "08 9307 0900");
	ldata.updateProperty(mDbLstColName[eEmail], "whitfords@realmark.com.au ");
	mlstData << ldata;
	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], "http://realmark.com.au/contact.cfm");
	ldata.updateProperty(mDbLstColName[eName], "Realmark Leederville");
	ldata.updateProperty(mDbLstColName[eAddress], "Ground Floor, 658 Newcastle Street, Leederville WA 6903 ");
	ldata.updateProperty(mDbLstColName[ePhone], "08 9328 0939");
	ldata.updateProperty(mDbLstColName[eFax], "08 9227 9229");
	ldata.updateProperty(mDbLstColName[eEmail], "leederville@realmark.com.au ");
	mlstData << ldata;
	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], "http://realmark.com.au/contact.cfm");
	ldata.updateProperty(mDbLstColName[eName], "Realmark Premier");
	ldata.updateProperty(mDbLstColName[eAddress], "707 North Beach Road, GWELUP WA 6018");
	ldata.updateProperty(mDbLstColName[ePhone], "08 9242 8200");
	ldata.updateProperty(mDbLstColName[eFax], "08 9288 1533");
	ldata.updateProperty(mDbLstColName[eEmail], "mtravia@realmark.com.au ");
	mlstData << ldata;
	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], "http://realmark.com.au/contact.cfm");
	ldata.updateProperty(mDbLstColName[eName], "Realmark South ");
	ldata.updateProperty(mDbLstColName[eAddress], "11a Riseley Street, APPLECROSS WA 6153");
	ldata.updateProperty(mDbLstColName[ePhone], "08 9428 2888");
	ldata.updateProperty(mDbLstColName[eFax], "08 9316 1660");
	ldata.updateProperty(mDbLstColName[eEmail], "applecross@realmark.com.au ");
	mlstData << ldata;
	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], "http://realmark.com.au/contact.cfm");
	ldata.updateProperty(mDbLstColName[eName], "Realmark Strata");
	ldata.updateProperty(mDbLstColName[eAddress], "Level 1, 658 Newcastle Street, Leederville WA 6903");
	ldata.updateProperty(mDbLstColName[ePhone], "08 9328 0999");
	ldata.updateProperty(mDbLstColName[eFax], " 08 9227 9229");
	ldata.updateProperty(mDbLstColName[eEmail], "dharwood@realmark.com.au ");
	mlstData << ldata;

	saveData(&mlstData, true);
}
