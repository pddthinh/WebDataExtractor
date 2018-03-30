#include "toolstation_com.h"

#define BASEURL "http://www.toolstation.com/"
#define LOGGER_FILENAME "item-link.txt"

toolstation_com::toolstation_com(QWidget *apParent)
	: BaseWebEngine(apParent)
{
	// for using custom control, call this function then implement initExtControl()
	//	initCustomControl();

	setWindowTitle("toolstation_com");
	setBaseURL(BASEURL);

	if(mpDBLogger != NULL)
		mpDBLogger->setLogPath("toolstation_com.sql");

	mpLog->setFilePath("toolstation_com.txt");
	///////////////////////////////////////////////////////////
	mpLinkLogger = new Logger(LOGGER_FILENAME);
}

enum enuDatabase
{
	eName = 0,
	eDatabaseMax
};

void toolstation_com::initDatabase()
{
	mDbLstColName << "Name";
	mDbLstKey << eName;
	mDbTableName = "toolstation_com";
}

void toolstation_com::doStart()
{
//	meState = E_STATE_INIT;
	meState = E_STATE_SUB1;

	loadPage(BASEURL);
}

void toolstation_com::doWebLoadFinished()
{
	switch(meState)
	{
	case E_STATE_INIT:
		break;

	case E_STATE_GET_MAIN_CATEGORY:
		qDebug() << "E_STATE_GET_MAIN_CATEGORY";
		break;

	case E_STATE_GET_SUB_CATEGORY:
		qDebug() << "E_STATE_GET_SUB_CATEGORY";
		break;

	case E_STATE_GET_DETAIL_INFO:
		qDebug() << "E_STATE_GET_DETAIL_INFO";
		break;

	case E_STATE_SUB1:
		getDepartments();
		break;

	case E_STATE_SUB2:
		getItemCategory();
		break;

	case E_STATE_SUB3:
		getItemList();
		break;

	default:
		qDebug() << "default";
		break;
	}
}

void toolstation_com::getDepartments()
{
	QString lstrTmp;

	mElement = mpWebView->getElementById("department_navigation");
	mCol = mElement.findAll("a");
	foreach (mElement, mCol)
	{
		lstrTmp = mElement.attribute("href");
		if (lstrTmp.startsWith("http://www.toolstation.com") == false)
			continue;

		mDepartments << lstrTmp;
	}

	if (gotoNextDepartment() == false)
		showFinishMsg("Can not goto next Department");
}

bool toolstation_com::gotoNextDepartment()
{
	bool lblRet = false;

	do
	{
		if (mDepartments.count() == 0)
			break;

		meState = E_STATE_SUB2;
		loadPage(mDepartments.takeFirst());
		lblRet = true;
	} while (false);

	setStatus(QString("Remain departments: %1")
			  .arg(mDepartments.count()));

	return lblRet;
}

void toolstation_com::getItemCategory()
{
	mElement = mpWebView->findFirstElement("ul", "class", "subdepartment_list");
	mCol = mElement.findAll("a");
	foreach (mElement, mCol)
		mlstCategoryLink << mpWebView->getHrefURL(&mElement);

	if (gotoNextCategoryLink(E_STATE_SUB3) == false)
	{
		if (gotoNextDepartment() == false)
			showFinishMsg("Extract link finished!");
	}
}

void toolstation_com::getItemList()
{
	mElement = mpWebView->findFirstElement("ul", "class", "parts_subdeparment_list");
	mCol = mElement.findAll("a");
	foreach (mElement, mCol)
		mlstDataLink << mpWebView->getHrefURL(&mElement);

	if (mlstDataLink.count() > 0)
	{
		mpLinkLogger->logStringList(mlstDataLink);
		DEF_LOG(QString("Save link: %1")
				.arg(mlstDataLink.count()));
		mlstDataLink.clear();
	}

	if (gotoNextCategoryLink(E_STATE_SUB3) == false)
	{
		if (gotoNextDepartment() == false)
			showFinishMsg("Extract link finished!");
	}
}



WebEngine_Main(toolstation_com)
