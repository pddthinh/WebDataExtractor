#include "USGreenBuildingCouncil.h"

#define BASEURL		"http://www.usgbc.org/myUSGBC/Members/MembersDirectory.aspx"

usgbc::usgbc(QWidget *apParent)
	: BaseWebEngine(apParent)
{
	setWindowTitle("usgbc");
	setBaseURL(BASEURL);

	if(mpDBLogger != NULL)
		mpDBLogger->setLogPath("usgbc.sql");

	mpLog->setFilePath("usgbc.txt");

	///////////////////////////////////////////////////
	mlstCategoryName << "Landscape Architects"
					 << "Architects"
					 << "Architects/Engineers"
					 << "Engineers"
					 << "Contractors and Builders";
	miCurCategory = -1;

	mlstDetailElementId << "lblOrganizationName"
						<< "lnkWebsite"
						<< "lblMembershipCategory" << "rptCategories_ctl00_lblSearchCategoryName"
						<< "lblAddress" << "lblCity" << "lblState" << "lblZip"
						<< "lblContactName" << "lblContactTitle"
						<< "lblContactPhone"
						<< "lnkContactEmail";

//	mpXMLLog = new XMLLogger(this->mpLog, "usgbc.xls");
}

enum enuDatabase
{
	eName = 0,
	eWebsite,
	eMembershipCategory, eCompanyCategories,
	eAddress, eCity, eState, eZipCode,
	eContactName, eContactTitle,
	ePhone,
	eEmail,
	eDatabaseMax
};

void usgbc::initDatabase()
{
	mDbLstColName << "Organization Name"
				  << "Website"
				  << "Membership Category" << "Company Categories"
				  << "Address" << "City" << "State" << "ZipCode"
				  << "Contact Name" << "Contact Title"
				  << "Phone"
				  << "Email";
	mDbLstKey << eName << ePhone << eEmail;
	mDbTableName = "usgbc";
}

void usgbc::doStart()
{
	meState = E_STATE_INIT;
	loadPage(BASEURL);
//	mpXMLLog->finishXMLFile();
}

void usgbc::doStop()
{}

void usgbc::doWebLoadFinished()
{
	switch(meState)
	{
	case E_STATE_INIT:
		qDebug() << "E_STATE_INIT";
		goNextCategory();
		break;

	case E_STATE_GET_MAIN_CATEGORY:
		qDebug() << "E_STATE_GET_MAIN_CATEGORY";
		getMainCategoryList();
		break;

	case E_STATE_GET_SUB_CATEGORY:
		qDebug() << "E_STATE_GET_SUB_CATEGORY";
		break;

	case E_STATE_GET_DETAIL_INFO:
		qDebug() << "E_STATE_GET_DETAIL_INFO";
		getDetailData();
		break;

	default:
		qDebug() << "default";
		break;
	}
}

bool usgbc::goNextCategory()
{
	bool lblRet = false;
	QWebElement lelement;

	do
	{
		miCurCategory ++;
		if(miCurCategory >= mlstCategoryName.size())
		{
			DEF_LOG("goNextCategory --- Go to end of category");
			break;
		}

		lelement = mpWebView->getElementById("ddlCategory");
		DEF_CHECK_BREAK(lelement, "goNextCategory --- Can not find the ddlCategory");

		if(mpWebView->changeCmbSelectedElement("#ddlCategory",
											   mlstCategoryName[miCurCategory]) == false)
		{
			DEF_LOG("Can not set the selected index of ddlCategory");
			break;
		}

		if(mpWebView->changeCmbSelectedElement("#ddlCountry", 1) == false)
		{
			DEF_LOG("Can not set the selected index of ddlCountry");
			break;
		}

		meState = E_STATE_GET_MAIN_CATEGORY;
		mpWebView->doClick("#btnSearch");
	}while(false);

	return lblRet;
}

void usgbc::getMainCategoryList()
{
	QWebElement				lelement, leTmp;
	QWebElementCollection	leCol;
	QString					lstrLink;

	do
	{
		lelement = mpWebView->getElementById("dgMembers");
		DEF_CHECK_BREAK(lelement, "getMainCategoryList --- Can not get dgMembers");

		leCol = lelement.findAll("a");
		foreach(leTmp, leCol)
		{
			lstrLink = leTmp.attribute("href");
			if(lstrLink.startsWith("MemberDetails"))
				mlstDataLink << mstrBaseRootURL + lstrLink;
		}

		qDebug() << "Get: " << mlstDataLink.count();

	// TODO: Go to next search result page
		if(gotoNextDataLink(E_STATE_GET_DETAIL_INFO) == false)
		{
	// TODO: go to next category
			showFinishMsg();
			break;
		}
	}while(false);
}

void usgbc::getDetailData()
{
	QWebElement			lelement;
	DataEntry			ldata(mDbLstColName);
	int					liIdx;

	for(liIdx = eName; liIdx < eDatabaseMax; liIdx++)
	{
		lelement = mpWebView->getElementById(mlstDetailElementId[liIdx]);
		if(lelement.isNull())
			continue;

		ldata.updateProperty(mDbLstColName[liIdx], lelement.toPlainText());
	}

	mlstData << ldata;
	if(mlstData.count() == 10)
	{
		mpDBLogger->logDataList(mlstData);
		DEF_LOG(QString("Save %1 dataentry").arg(mlstData.count()));

		mlstData.clear();
	}

	if(gotoNextDataLink(E_STATE_GET_DETAIL_INFO) == false)
	{
		if(mlstData.count() > 0 )
		{
			mpDBLogger->logDataList(mlstData);
			DEF_LOG(QString("Save %1 dataentry").arg(mlstData.count()));

			mlstData.clear();
		}

// TODO: go to next category

		showFinishMsg();
	}
}

WebEngine_Main(usgbc)
