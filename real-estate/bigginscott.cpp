#include "bigginscott.h"

bigginscott::bigginscott(QString url, QWidget *apParent)
	: real_estate(url, apParent)
{
	mlstOffice << "http://www.bigginscott.com.au//consultants/office/21/altona"
			   << "http://www.bigginscott.com.au//consultants/office/2/aspendale"
			   << "http://www.bigginscott.com.au//consultants/office/61/brighton"
			   << "http://www.bigginscott.com.au//consultants/office/6/carlton"
			   << "http://www.bigginscott.com.au//consultants/office/70/carnegie"
			   << "http://www.bigginscott.com.au//consultants/office/63/deer-park"
			   << "http://www.bigginscott.com.au//consultants/office/8/elsternwick"
			   << "http://www.bigginscott.com.au//consultants/office/64/kensington"
			   << "http://www.bigginscott.com.au//consultants/office/3/knox"
			   << "http://www.bigginscott.com.au//consultants/office/10/maribyrnong"
			   << "http://www.bigginscott.com.au//consultants/office/12/melbourne-commercial-land"
			   << "http://www.bigginscott.com.au//consultants/office/13/melbourne-residential"
			   << "http://www.bigginscott.com.au//consultants/office/14/port-melbourne"
			   << "http://www.bigginscott.com.au//consultants/office/15/prahran"
			   << "http://www.bigginscott.com.au//consultants/office/11/retail-business-brokers"
			   << "http://www.bigginscott.com.au//consultants/office/71/richmond"
			   << "http://www.bigginscott.com.au//consultants/office/17/st-kilda"
			   << "http://www.bigginscott.com.au//consultants/office/18/yarraville"
			   << "http://www.bigginscott.com.au//consultants/office/1/head-office";

	// skip the first entry because it is initialized from main screen
	mlstOffice.takeFirst();
}

void bigginscott::doWebLoadFinished()
{
	switch (meState)
	{
	case E_STATE_INIT:
		collectProfile();
		break;

	case E_STATE_GET_DETAIL_INFO:
		parseData();
		break;

	default:
		break;
	}
}

bool bigginscott::gotoNextOffice()
{
	bool lblRet = false;
	QString url;

	do
	{
		if (mlstOffice.isEmpty() == true)
			break;
		url = mlstOffice.takeFirst();
		loadPage(url);

		lblRet = true;
	} while (false);

	return lblRet;
}

void bigginscott::collectProfile()
{
	QWebElement lelement;
	QWebElementCollection lcol;

	do
	{
		lcol = mpWebView->findAll("a[class=\"button consultantdetails\"]");
		foreach (lelement, lcol)
			mlstDataLink << mpWebView->getHrefURL(&lelement);

		if (gotoNextOffice() == false)
		{
			// start parsing
			if (gotoNextDataLink() == false)
				showFinishMsg();
		}
	} while (false);
}

void bigginscott::parseData()
{
	QWebElement leDetail, lelement;
	QString lstrTmp;
	DataEntry ldata(mDbLstColName);

	do
	{
		leDetail = mpWebView->findFirst("div[class=\"consultant-details-text\"]");
		DEF_CHECK_BREAK(leDetail, QString("Can not find the details text element: %1")
						.arg(mpWebView->url().toString()));

		ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
		ldata.updateProperty(mDbLstColName[eItemLink], mpWebView->url().toString());

		lstrTmp.clear();
		lelement = leDetail.findFirst("div[class=\"consultant-details-name\"]");
		if (lelement.isNull() == false)
			lstrTmp = lelement.toPlainText();
		ldata.updateProperty(mDbLstColName[eName], lstrTmp);

		lstrTmp.clear();
		lelement = leDetail.findFirst("p");
		if (lelement.isNull() == false)
			lstrTmp = lelement.toPlainText();
		ldata.updateProperty(mDbLstColName[eBusinessName], lstrTmp);

		lstrTmp = leDetail.toPlainText() + "\n";
		Utilities::extractAndUpdate(&ldata, mDbLstColName[ePhone], lstrTmp, "Phone");
		Utilities::extractAndUpdate(&ldata, mDbLstColName[eFax], lstrTmp, "Fax");
		Utilities::extractAndUpdate(&ldata, mDbLstColName[eMobile], lstrTmp, "Mobile");
		Utilities::extractAndUpdate(&ldata, mDbLstColName[eEmail], lstrTmp, "Email");

		mlstData << ldata;
	} while (false);

	if (mlstData.count() == 10)
	{
		mpDBLogger->logDataList(mlstData);
		DEF_LOG(QString("Save %1").arg(mlstData.count()));
		mlstData.clear();
	}

	if (gotoNextDataLink() == false)
	{
		if (mlstData.count() > 0)
		{
			mpDBLogger->logDataList(mlstData);
			DEF_LOG(QString("Save %1").arg(mlstData.count()));
			mlstData.clear();
		}

		collectOfficeContact();
		showFinishMsg();
	}
}

void bigginscott::collectOfficeContact()
{
	DataEntry ldata(mDbLstColName);

	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], "http://www.bigginscott.com.au/content/show/7571");
	ldata.updateProperty(mDbLstColName[eName], "Deer Park");
	ldata.updateProperty(mDbLstColName[eAddress], "817a Ballarat Road, Deer Park VIC");
	ldata.updateProperty(mDbLstColName[ePhone], "03 9363 0388");
	ldata.updateProperty(mDbLstColName[eFax], "03 9363 0399");
	ldata.updateProperty(mDbLstColName[eEmail], "deerpark@bigginscott.com.au");
	mlstData << ldata;
	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], "http://www.bigginscott.com.au/content/show/7571");
	ldata.updateProperty(mDbLstColName[eName], "Altona");
	ldata.updateProperty(mDbLstColName[eAddress], "66-70 Railway Street South, Altona VIC");
	ldata.updateProperty(mDbLstColName[ePhone], "03 9398 8400");
	ldata.updateProperty(mDbLstColName[eFax], "03 9398 8477");
	ldata.updateProperty(mDbLstColName[eEmail], "altona@bigginscott.com.au");
	mlstData << ldata;
	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], "http://www.bigginscott.com.au/content/show/7571");
	ldata.updateProperty(mDbLstColName[eName], "Maribyrnong");
	ldata.updateProperty(mDbLstColName[eAddress], "93 Rosamond Road, Maribyrnong VIC");
	ldata.updateProperty(mDbLstColName[ePhone], "03 9317 5577");
	ldata.updateProperty(mDbLstColName[eFax], "");
	ldata.updateProperty(mDbLstColName[eEmail], "maribyrnong@bigginscott.com.au");
	mlstData << ldata;
	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], "http://www.bigginscott.com.au/content/show/7571");
	ldata.updateProperty(mDbLstColName[eName], "Yarraville");
	ldata.updateProperty(mDbLstColName[eAddress], "39 Anderson Street, Yarraville VIC");
	ldata.updateProperty(mDbLstColName[ePhone], "03 9687 8899");
	ldata.updateProperty(mDbLstColName[eFax], "");
	ldata.updateProperty(mDbLstColName[eEmail], "yarraville@bigginscott.com.au");
	mlstData << ldata;
	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], "http://www.bigginscott.com.au/content/show/7571");
	ldata.updateProperty(mDbLstColName[eName], "Kensington");
	ldata.updateProperty(mDbLstColName[eAddress], "178 Bellair Street, Kensington VIC");
	ldata.updateProperty(mDbLstColName[ePhone], "03 9376 8000");
	ldata.updateProperty(mDbLstColName[eFax], "");
	ldata.updateProperty(mDbLstColName[eEmail], "kensington@bigginscott.com.au");
	mlstData << ldata;
	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], "http://www.bigginscott.com.au/content/show/7571");
	ldata.updateProperty(mDbLstColName[eName], "Carlton");
	ldata.updateProperty(mDbLstColName[eAddress], "130 Elgin Street, Carlton VIC");
	ldata.updateProperty(mDbLstColName[ePhone], "03 9347 3700");
	ldata.updateProperty(mDbLstColName[eFax], "");
	ldata.updateProperty(mDbLstColName[eEmail], "carlton@bigginscott.com.au");
	mlstData << ldata;
	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], "http://www.bigginscott.com.au/content/show/7571");
	ldata.updateProperty(mDbLstColName[eName], "Richmond");
	ldata.updateProperty(mDbLstColName[eAddress], "28 Bridge Road, Richmond VIC");
	ldata.updateProperty(mDbLstColName[ePhone], "03 9426 4000");
	ldata.updateProperty(mDbLstColName[eFax], "03 9428 6558");
	ldata.updateProperty(mDbLstColName[eEmail], "richmond@bigginscott.com.au");
	mlstData << ldata;
	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], "http://www.bigginscott.com.au/content/show/7571");
	ldata.updateProperty(mDbLstColName[eName], "Retail & Business Brokers");
	ldata.updateProperty(mDbLstColName[eAddress], "L3, 448 St Kilda Road, Melbourne VIC");
	ldata.updateProperty(mDbLstColName[ePhone], "03 9866 7771");
	ldata.updateProperty(mDbLstColName[eFax], "03 9866 7172");
	ldata.updateProperty(mDbLstColName[eEmail], "retail@bigginscott.com.au");
	mlstData << ldata;
	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], "http://www.bigginscott.com.au/content/show/7571");
	ldata.updateProperty(mDbLstColName[eName], "Melbourne Commercial & Land");
	ldata.updateProperty(mDbLstColName[eAddress], "448 St Kilda Road, Melbourne VIC");
	ldata.updateProperty(mDbLstColName[ePhone], "03 9867 2144");
	ldata.updateProperty(mDbLstColName[eFax], "03 9867 7119");
	ldata.updateProperty(mDbLstColName[eEmail], "melbourne@bigginscott.com.au");
	mlstData << ldata;
	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], "http://www.bigginscott.com.au/content/show/7571");
	ldata.updateProperty(mDbLstColName[eName], "Melbourne Residential");
	ldata.updateProperty(mDbLstColName[eAddress], "Ground Level 448 St Kilda Road, Melbourne VIC");
	ldata.updateProperty(mDbLstColName[ePhone], "03 9867 1444");
	ldata.updateProperty(mDbLstColName[eFax], "03 98671444");
	ldata.updateProperty(mDbLstColName[eEmail], "melbourneresidential@bigginscott.com.au");
	mlstData << ldata;
	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], "http://www.bigginscott.com.au/content/show/7571");
	ldata.updateProperty(mDbLstColName[eName], "St Kilda");
	ldata.updateProperty(mDbLstColName[eAddress], "8 Fitzroy Street, St Kilda VIC");
	ldata.updateProperty(mDbLstColName[ePhone], "03 9534 0241");
	ldata.updateProperty(mDbLstColName[eFax], "");
	ldata.updateProperty(mDbLstColName[eEmail], "stkilda@bigginscott.com.au");
	mlstData << ldata;
	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], "http://www.bigginscott.com.au/content/show/7571");
	ldata.updateProperty(mDbLstColName[eName], "Prahran");
	ldata.updateProperty(mDbLstColName[eAddress], "305 High Street, Prahran VIC");
	ldata.updateProperty(mDbLstColName[ePhone], "03 9520 9000");
	ldata.updateProperty(mDbLstColName[eFax], "03 9529 8049");
	ldata.updateProperty(mDbLstColName[eEmail], "prahran@bigginscott.com.au");
	mlstData << ldata;
	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], "http://www.bigginscott.com.au/content/show/7571");
	ldata.updateProperty(mDbLstColName[eName], "Elsternwick");
	ldata.updateProperty(mDbLstColName[eAddress], "1st Floor 376 Glenhuntly Road, Elsternwick VIC");
	ldata.updateProperty(mDbLstColName[ePhone], "03 9523 9444");
	ldata.updateProperty(mDbLstColName[eFax], "");
	ldata.updateProperty(mDbLstColName[eEmail], "elsternwick@bigginscott.com.au");
	mlstData << ldata;
	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], "http://www.bigginscott.com.au/content/show/7571");
	ldata.updateProperty(mDbLstColName[eName], "Brighton");
	ldata.updateProperty(mDbLstColName[eAddress], "142 Church Street, Brighton VIC");
	ldata.updateProperty(mDbLstColName[ePhone], "03 9592 4300");
	ldata.updateProperty(mDbLstColName[eFax], "");
	ldata.updateProperty(mDbLstColName[eEmail], "brighton@bigginscott.com.au");
	mlstData << ldata;
	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], "http://www.bigginscott.com.au/content/show/7571");
	ldata.updateProperty(mDbLstColName[eName], "Carnegie");
	ldata.updateProperty(mDbLstColName[eAddress], "42 Koornang Road, Carnegie VIC");
	ldata.updateProperty(mDbLstColName[ePhone], "03 9563 1666");
	ldata.updateProperty(mDbLstColName[eFax], "03 9563 1369");
	ldata.updateProperty(mDbLstColName[eEmail], "carnegie@bigginscott.com.au");
	mlstData << ldata;
	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], "http://www.bigginscott.com.au/content/show/7571");
	ldata.updateProperty(mDbLstColName[eName], "Aspendale");
	ldata.updateProperty(mDbLstColName[eAddress], "132 Station Street, Aspendale VIC");
	ldata.updateProperty(mDbLstColName[ePhone], "03 9580 3888");
	ldata.updateProperty(mDbLstColName[eFax], "03 9580 9933");
	ldata.updateProperty(mDbLstColName[eEmail], "aspendale@bigginscott.com.au");
	mlstData << ldata;
	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], "http://www.bigginscott.com.au/content/show/7571");
	ldata.updateProperty(mDbLstColName[eName], "Knox");
	ldata.updateProperty(mDbLstColName[eAddress], "88 Boronia Road, Boronia VIC");
	ldata.updateProperty(mDbLstColName[ePhone], "03 9761 2277");
	ldata.updateProperty(mDbLstColName[eFax], "03 9761 2140");
	ldata.updateProperty(mDbLstColName[eEmail], "knox@bigginscott.com.au");
	mlstData << ldata;

	saveData(&mlstData, true);
}
