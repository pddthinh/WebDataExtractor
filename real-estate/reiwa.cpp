#include "reiwa.h"

#define DEF_OFFICE_LOG	"office-list.txt"

reiwa::reiwa(QString url)
	:real_estate(url, 0)
{
	initCustomControl();
}

void reiwa::doStart()
{
	QString lstrPath = mpExtFileLoading->getLoadPath();

	meState = E_STATE_INIT;
	if (lstrPath.isEmpty() == false)
	{
		loadOfficeName(lstrPath);

		meState = E_STATE_SUB1;
	}

	loadPage(mstrURL);
}

void reiwa::doWebLoadFinished()
{
	switch (meState)
	{
	case E_STATE_INIT:
		getSearchLink();
		break;

	case E_STATE_GET_SUB_CATEGORY:
		getSearchResult();
		break;

	case E_STATE_SUB1:
		searchOffice();
		break;

	case E_STATE_SUB2:
		getOfficeDetail();
		break;

	case E_STATE_GET_DETAIL_INFO:
		parseOfficeData();
		break;

	default:
		break;
	}
}

void reiwa::getSearchLink()
{
	QString lstrLink;
	mCol = mpWebView->findAll("a");
	foreach (mElement, mCol)
	{
		lstrLink = mpWebView->getHrefURL(&mElement);
		if (lstrLink.indexOf("?Word=") < 0)
			continue;

		mlstCategoryLink << lstrLink;
	}

	mlstCategoryLink.removeDuplicates();
	qDebug() << "Found search link: " << mlstCategoryLink.count();

//	while (mlstCategoryLink.count() > 4)
//		mlstCategoryLink.takeFirst();

	if (gotoNextCategoryLink() == false)
		showFinishMsg("Can not goto next search letter link");
}

void reiwa::getSearchResult()
{
	do
	{
		extractData(true);

		if (gotoNextCategoryLink() == false)
		{
			// start parsing office detail
			mlstOfficeName.removeDuplicates();
			qDebug() << "Office found: " << mlstOfficeName.count();

			if (mpExtFileLoading->getExtractLink() == true)
			{
				// Extract office name only
				Logger *lpLog = new Logger(DEF_OFFICE_LOG);

				lpLog->logStringList(mlstOfficeName);
				showFinishMsg();
				break;
			}

			meState = E_STATE_SUB1;
			loadPage(mstrURL);
			setStatus(QString("Remain office: %1")
					  .arg(mlstOfficeName.count()));
		}
	} while (false);
}

void reiwa::searchOffice()
{
	QString id;
	bool lblFound = false;

	do
	{
		if (mlstOfficeName.isEmpty() == true)
		{
			showFinishMsg();
			break;
		}
		mstrOffice = mlstOfficeName.takeFirst();
		DEF_LOG(QString("Start for office: %1")
				.arg(mstrOffice));

		// find the input text
		mCol = mpWebView->findAllElement("input", "class", "searchText");
		lblFound = false;
		foreach (mElement, mCol)
		{
			id = mElement.attribute("id");
			if (id.indexOf("businessName") >= 0)
			{
				lblFound = true;
				break;
			}
		}

		if (lblFound == false)
		{
			DEF_LOG(QString("Can not find business name input: %1")
					.arg(mstrOffice));
			showFinishMsg("Can not find input business textbox");
			break;
		}

		mElement.setAttribute("value", mstrOffice);

		// find the submit button
		mElement = mElement.nextSibling();
		if (mElement.isNull())
		{
			DEF_LOG(QString("Can not find go button: %1")
					.arg(mstrOffice));
			showFinishMsg("Can not find go button");
			break;
		}

		// do the search
		meState = E_STATE_SUB2;
		mpWebView->doClick(&mElement);
	} while (false);
}

void reiwa::getOfficeDetail()
{
	bool lblNextOffice = true;

	do
	{
		mElement = mpWebView->findFirstElement("a", "class", "agentstaff");
		DEF_CHECK_BREAK(mElement, QString("Can not find agent: %1")
						.arg(mstrOffice));

		lblNextOffice = false;
		meState = E_STATE_GET_DETAIL_INFO;
		mpWebView->doClick(&mElement);
	} while (false);

	// goto next office
	if (lblNextOffice == true)
	{
		DEF_LOG(QString("End office: %1")
				.arg(mstrOffice));

		meState = E_STATE_SUB1;
		loadPage(mstrURL);
		setStatus(QString("Remain office: %1")
				  .arg(mlstOfficeName.count()));
	}
}

void reiwa::parseOfficeData()
{
	extractData();

	// goto next office
	meState = E_STATE_SUB1;
	loadPage(mstrURL);
	setStatus(QString("Remain office: %1")
			  .arg(mlstOfficeName.count()));
}

void reiwa::extractData(bool ablSaveName)
{
	QWebElement lelement;
	QWebElementCollection lCol;
	QString lstrTmp, lstrName, id;
	DataEntry ldata(mDbLstColName);

	do
	{
		mElement = mpWebView->getElementById("findagent");
		mCol = mpWebView->findAllDiv("class", "wrap", &mElement);
		if (mCol.count() == 0)
		{
			lstrTmp = QString("Can not find any data of: %1");
			if (ablSaveName == true)
				lstrTmp = lstrTmp.arg(itemURL());
			else
				lstrTmp = lstrTmp.arg(mstrOffice);

			DEF_LOG(lstrTmp);
			break;
		}

		foreach (mElement, mCol)
		{
			ldata.clearData();
			ldata.updateProperty(mDbLstColName[eItemLink], itemURL());

			lelement = mElement.findFirst("h3");
			updateData(&ldata, eName, &lelement);
			lstrName = lelement.toPlainText();
			if (lstrName.isEmpty() == false && ablSaveName == true)
				mlstOfficeName << lstrName;

			lelement = mpWebView->findFirstDiv("class", "addressAgent", &mElement);
			lstrTmp = lelement.toPlainText() + "\n";

			Utilities::extractAndUpdate(&ldata, mDbLstColName[ePhone], lstrTmp, "Phone: ");
			Utilities::extractAndUpdate(&ldata, mDbLstColName[eMobile], lstrTmp, "Mobile: ");
			Utilities::extractAndUpdate(&ldata, mDbLstColName[eFax], lstrTmp, "Fax: ");

			lstrTmp.replace(lstrName, "");
			lstrTmp.replace("Phone: " + ldata.getProperty(mDbLstColName[ePhone]), "");
			lstrTmp.replace("Mobile: " + ldata.getProperty(mDbLstColName[eMobile]), "");
			lstrTmp.replace("Fax: " + ldata.getProperty(mDbLstColName[eFax]), "");
			lstrTmp.replace("\240", "");
			ldata.updateProperty(mDbLstColName[eAddress], lstrTmp);

			lCol = mElement.findAll("a");
			foreach (lelement, lCol)
			{
				id = lelement.attribute("id");
				lstrTmp = lelement.attribute("href").toLower();

				if (id.indexOf("lnkEmail") >= 0)
					ldata.updateProperty(mDbLstColName[eEmail], lstrTmp.replace("mailto:", ""));
				if (id.indexOf("linkWebsite") >= 0)
					ldata.updateProperty(mDbLstColName[eWebsite], lstrTmp);
			}

			mlstData << ldata;
			saveData();
		}

		saveData(&mlstData, true);
	} while(false);
}

void reiwa::loadOfficeName(QString astrPath)
{
	int count = 0;
	int skip = mpExtFileLoading->getNumberOfSkipItem();

	mlstOfficeName = Utilities::loadTextData(astrPath);

	count = 0;
	while (count < skip)
	{
		mlstOfficeName.takeFirst();
		count ++;
	}

	DEF_LOG(QString("Init office size: %1")
			.arg(mlstOfficeName.count()));
}
