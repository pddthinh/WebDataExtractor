#include "loverealestate.h"

loverealestate::loverealestate(QString url, QWidget *apParent)
	: real_estate(url, apParent)
{
	init("http://www.loverealestate.com.au/About/Our-Team/Profile.aspx");
}

void loverealestate::doWebLoadFinished()
{
	switch (meState)
	{
	case E_STATE_INIT:
		parseData();
		break;

	default:
		break;
	}
}

void loverealestate::parseData()
{
	QWebElement lelement, lContainer;
	QWebElementCollection lcol;
	QString lstrTmp;
	DataEntry ldata(mDbLstColName);

	do
	{
		lcol = mpWebView->findAll("div[class*=\"section-\"]");
		foreach (lContainer, lcol)
		{
			if (lContainer.firstChild().tagName().toLower() == "img")
				continue;

			// start parsing
			ldata.clearData();

			lstrTmp.clear();
			lelement = lContainer.findFirst("h3");
			if (lelement.isNull() == false)
				lstrTmp = lelement.toPlainText();
			ldata.updateProperty(mDbLstColName[eName], lstrTmp);

			lstrTmp.clear();
			lelement = lContainer.findFirst("p");
			if (lelement.isNull() == false)
				lstrTmp = lelement.toPlainText();
			ldata.updateProperty(mDbLstColName[eBusinessName], lstrTmp);

			lstrTmp = lContainer.toPlainText() + "\n";

			Utilities::extractAndUpdate(&ldata, mDbLstColName[eEmail], lstrTmp, "Email\n");
			Utilities::extractAndUpdate(&ldata, mDbLstColName[ePhone], lstrTmp, "Phone\n");
			Utilities::extractAndUpdate(&ldata, mDbLstColName[eMobile], lstrTmp, "Mobile\n");

			ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
			ldata.updateProperty(mDbLstColName[eItemLink], mpWebView->url().toString());

			mlstData << ldata;
		}

		if (mlstData.count() > 0)
		{
			mpDBLogger->logDataList(mlstData);
			DEF_LOG(QString("Save %1").arg(mlstData.count()));
			mlstData.clear();
		}

		getOfficeContact();
		showFinishMsg();
	} while (false);
}

void loverealestate::getOfficeContact()
{
	DataEntry ldata(mDbLstColName);

	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], "http://www.loverealestate.com.au/Contact-Us");
	ldata.updateProperty(mDbLstColName[eName], "Real Estate Reservoir-Sales");
	ldata.updateProperty(mDbLstColName[eAddress], "307 Spring Street, Reservoir, 3073");
	ldata.updateProperty(mDbLstColName[ePhone], "(03) 9460 6511");
	ldata.updateProperty(mDbLstColName[eFax], "03 9460 4059");
	ldata.updateProperty(mDbLstColName[eEmail], "reservoir@loverealestate.com.au");
	mlstData << ldata;

	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], "http://www.loverealestate.com.au/Contact-Us");
	ldata.updateProperty(mDbLstColName[eName], "Real Estate Reservoir-Rentals");
	ldata.updateProperty(mDbLstColName[eAddress], "307 Spring Street, Reservoir, 3073");
	ldata.updateProperty(mDbLstColName[ePhone], "(03) 9460 6511");
	ldata.updateProperty(mDbLstColName[eFax], "03 9460 4452");
	ldata.updateProperty(mDbLstColName[eEmail], "reception.reservoir@loverealestate.com.au");
	mlstData << ldata;

	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], "http://www.loverealestate.com.au/Contact-Us");
	ldata.updateProperty(mDbLstColName[eName], "Real Estate Reservoir-Customer Service ");
	ldata.updateProperty(mDbLstColName[eAddress], "307 Spring Street, Reservoir, 3073");
	ldata.updateProperty(mDbLstColName[ePhone], "(03) 9460 6511");
	ldata.updateProperty(mDbLstColName[eFax], "");
	ldata.updateProperty(mDbLstColName[eEmail], "customerservice@loverealestate.com.au");
	mlstData << ldata;

	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], "http://www.loverealestate.com.au/Contact-Us");
	ldata.updateProperty(mDbLstColName[eName], "Real Estate Mill Park-Sales");
	ldata.updateProperty(mDbLstColName[eAddress], "Shop 45 The Stables Shopping Centre, Childs Road, Mill Park, 3082");
	ldata.updateProperty(mDbLstColName[ePhone], "03 9404 4955");
	ldata.updateProperty(mDbLstColName[eFax], "03 9436 6264");
	ldata.updateProperty(mDbLstColName[eEmail], "millpark@loverealestate.com.au");
	mlstData << ldata;

	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], "http://www.loverealestate.com.au/Contact-Us");
	ldata.updateProperty(mDbLstColName[eName], "Real Estate Mill Park-Rentals");
	ldata.updateProperty(mDbLstColName[eAddress], "Shop 45 The Stables Shopping Centre, Childs Road, Mill Park, 3082");
	ldata.updateProperty(mDbLstColName[ePhone], "03 9404 4955");
	ldata.updateProperty(mDbLstColName[eFax], "03 9436 6264");
	ldata.updateProperty(mDbLstColName[eEmail], "rentals.millpark@loverealestate.com.au");
	mlstData << ldata;

	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], "http://www.loverealestate.com.au/Contact-Us");
	ldata.updateProperty(mDbLstColName[eName], "Real Estate Epping-Sales");
	ldata.updateProperty(mDbLstColName[eAddress], "780 High Street, Epping, 3076");
	ldata.updateProperty(mDbLstColName[ePhone], "03 9401 2322");
	ldata.updateProperty(mDbLstColName[eFax], "03 9401 2055");
	ldata.updateProperty(mDbLstColName[eEmail], "epping@loverealestate.com.au");
	mlstData << ldata;

	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], "http://www.loverealestate.com.au/Contact-Us");
	ldata.updateProperty(mDbLstColName[eName], "");
	ldata.updateProperty(mDbLstColName[eAddress], "");
	ldata.updateProperty(mDbLstColName[ePhone], "");
	ldata.updateProperty(mDbLstColName[eFax], "");
	ldata.updateProperty(mDbLstColName[eEmail], "");
	mlstData << ldata;

	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], "http://www.loverealestate.com.au/Contact-Us");
	ldata.updateProperty(mDbLstColName[eName], "Real Estate Preston-Sales");
	ldata.updateProperty(mDbLstColName[eAddress], "274 High Street, Preston, 3072");
	ldata.updateProperty(mDbLstColName[ePhone], "03 9471 0233");
	ldata.updateProperty(mDbLstColName[eFax], "03 9471 0829");
	ldata.updateProperty(mDbLstColName[eEmail], "preston@loverealestate.com.au");
	mlstData << ldata;

	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], "http://www.loverealestate.com.au/Contact-Us");
	ldata.updateProperty(mDbLstColName[eName], "Real Estate Preston-Rentals");
	ldata.updateProperty(mDbLstColName[eAddress], "274 High Street, Preston, 3072");
	ldata.updateProperty(mDbLstColName[ePhone], "03 9471 0233");
	ldata.updateProperty(mDbLstColName[eFax], "03 9471 0829");
	ldata.updateProperty(mDbLstColName[eEmail], "rentals.preston@loverealestate.com.au");
	mlstData << ldata;

	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], "http://www.loverealestate.com.au/Contact-Us");
	ldata.updateProperty(mDbLstColName[eName], "Real Estate Thomastown-Sales");
	ldata.updateProperty(mDbLstColName[eAddress], "201 High Street, Thomastown, 3074");
	ldata.updateProperty(mDbLstColName[ePhone], "03 9465 2133");
	ldata.updateProperty(mDbLstColName[eFax], "03 9464 1047");
	ldata.updateProperty(mDbLstColName[eEmail], "thomastown@loverealestate.com.au");
	mlstData << ldata;

	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], "http://www.loverealestate.com.au/Contact-Us");
	ldata.updateProperty(mDbLstColName[eName], "Real Estate Thomastown-Rentals");
	ldata.updateProperty(mDbLstColName[eAddress], "201 High Street, Thomastown, 3074");
	ldata.updateProperty(mDbLstColName[ePhone], "03 9465 2133");
	ldata.updateProperty(mDbLstColName[eFax], "03 9464 1047");
	ldata.updateProperty(mDbLstColName[eEmail], "rentals.thomastown@loverealestate.com.au");
	mlstData << ldata;

	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], "http://www.loverealestate.com.au/Contact-Us");
	ldata.updateProperty(mDbLstColName[eName], "Real Estate Thornbury-Sales");
	ldata.updateProperty(mDbLstColName[eAddress], "770 High Street, Thornbury, 3071");
	ldata.updateProperty(mDbLstColName[ePhone], "03 9480 2288");
	ldata.updateProperty(mDbLstColName[eFax], "03 9484 1845");
	ldata.updateProperty(mDbLstColName[eEmail], "thornbury@loverealestate.com.au");
	mlstData << ldata;

	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], "http://www.loverealestate.com.au/Contact-Us");
	ldata.updateProperty(mDbLstColName[eName], "Commercial - Reservoir");
	ldata.updateProperty(mDbLstColName[eAddress], "Level 1, 2 Ralph Street, Reservoir 3073");
	ldata.updateProperty(mDbLstColName[ePhone], "03 9460 6220");
	ldata.updateProperty(mDbLstColName[eFax], "03 94607442");
	ldata.updateProperty(mDbLstColName[eEmail], "commercial@loverealestate.com.au");
	mlstData << ldata;

	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], "http://www.loverealestate.com.au/Contact-Us");
	ldata.updateProperty(mDbLstColName[eName], "Country Sales");
	ldata.updateProperty(mDbLstColName[eAddress], "310 Houlahans Ln, Poowong, 3988");
	ldata.updateProperty(mDbLstColName[ePhone], "03 5659 2297");
	ldata.updateProperty(mDbLstColName[eFax], "03 5659 2297");
	ldata.updateProperty(mDbLstColName[eMobile], "0428 592 297");
	ldata.updateProperty(mDbLstColName[eEmail], "motton@dcsi.net.au");
	mlstData << ldata;

	ldata.updateProperty(mDbLstColName[eWebsite], mstrURL);
	ldata.updateProperty(mDbLstColName[eItemLink], "http://www.loverealestate.com.au/Contact-Us");
	ldata.updateProperty(mDbLstColName[eName], "Commercial - Bayswater");
	ldata.updateProperty(mDbLstColName[eAddress], "Suite 12/653 Mountain Hwy, Bayswater");
	ldata.updateProperty(mDbLstColName[ePhone], "03 9720 6600");
	ldata.updateProperty(mDbLstColName[eFax], "03 9720 9070");
	ldata.updateProperty(mDbLstColName[eEmail], "bayswater@loverealestate.com.au");
	mlstData << ldata;

	saveData(&mlstData, true);
}
