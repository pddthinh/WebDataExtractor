#include "paginasamarillas.es.h"

#include <QCheckBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>

#define BASEURL			"http://www.paginasamarillas.es/"
#define SEARCH_URL		"http://www.paginasamarillas.es/search/reformas/all-ma/all-pr/all-is/all-ci/all-ba/all-pu/all-nc/1?cu=%1"
#define DATA_LINK_FILE	"paginasamarillas-link-list.txt"

QString gstrKeyword[] =
{
	"reformas",
	"humedades",
};
int miCurKeyword = -1;

Paginasamarillas::Paginasamarillas(QWidget *apParent)
	: BaseWebEngine(apParent)
{
	setWindowTitle("Paginasamarillas");
	setBaseURL(BASEURL);

	if(mpDBLogger != NULL)
		mpDBLogger->setLogPath("Paginasamarillas.sql");

	mpLog->setFilePath("Paginasamarillas.txt");
	///////////////////////////////////////////////////////////

	setWebLoadTimeout(25000);
	miCurKeyword = 0;
	miCurState = -1;
	initCustomControl();
}

void Paginasamarillas::initExtWidget()
{
	QGridLayout	*lpMainLayout = (QGridLayout *)this->layout();

	int	liRow;
	int	liCol;

	do
	{
		if(lpMainLayout == NULL)
		{
			qDebug() << "Can not get main layout";
			break;
		}

		liRow = lpMainLayout->rowCount();
		liCol = lpMainLayout->columnCount();

		mpChkBoxGetLink = new QCheckBox("Get Data Link Only", this);
		lpMainLayout->addWidget(mpChkBoxGetLink, liRow, 0);

		QGroupBox	*lpGrpLoadInfo = new QGroupBox("Load Info");
		QGridLayout	*lpGrpLayout = new QGridLayout();

		mpChkBoxStartFromFile = new QCheckBox("Star from file", this);
		mpTxtNumberOfSkipItem = new QLineEdit(this);
		lpGrpLayout->addWidget(mpChkBoxStartFromFile);

		lpGrpLayout->addWidget(new QLabel("Skip item"), 1, 0);
		lpGrpLayout->addWidget(mpTxtNumberOfSkipItem, 1, 1);

		lpGrpLoadInfo->setLayout(lpGrpLayout);
		lpMainLayout->addWidget(lpGrpLoadInfo, liRow + 1, 0);

		mlstCtrlWidget << mpChkBoxGetLink;
		mlstCtrlWidget << lpGrpLoadInfo;
	}while(false);
}

enum enuDatabase
{
	eName = 0,
	eFullAdd, ePostCode, eLocality, eRegion,
	eWebsite, ePhone, eEmail,
	eDatabaseMax
};

void Paginasamarillas::initDatabase()
{
	mDbLstColName << "Name"
				  << "Full Address" << "Postcode" << "Locality" << "Region"
				  << "Website" << "Phone" << "Email";
	mDbLstKey << eName << eWebsite << ePhone;
	mDbTableName = "Paginasamarillas";
}

void Paginasamarillas::doStart()
{
	do
	{
		mblGetDataLinkOnly = mpChkBoxGetLink->isChecked();
		qDebug() << "GetDataLink only checked = " << mblGetDataLinkOnly;

		mblStartFromFile = mpChkBoxStartFromFile->isChecked();
		qDebug() << "Start from file checked = " << mblStartFromFile;

//		if(mblGetDataLinkOnly == true)
//		{
//			inputSearchKeyword();
//			break;
//		}

		if(mblStartFromFile == true)
		{
//			if(loadDataLinkFromFile() == false)
//			{
//				qDebug() << "Unknown error";
//				showFinishMsg("Finshed b/c of un-known error");
//				break;
//			}

			setWebLoadTimeout(20000);

			qDebug() << "Start extract data...";
			DEF_LOG("Start extract data...");

			if(gotoNextDataLink() == false)
			{
				qDebug() << "Finished all data links";
				DEF_LOG("Finished all data links");

				showFinishMsg("Finished all data links");
				break;
			}
		}

		inputSearchKeyword();
	} while(false);
}

void Paginasamarillas::doWebLoadFinished()
{
	switch(meState)
	{
//	case E_STATE_INIT:
//		qDebug() << "E_STATE_INIT";
//		break;

	case E_STATE_GET_MAIN_CATEGORY:
//		qDebug() << "E_STATE_GET_MAIN_CATEGORY";
		getItemList();
		break;

//	case E_STATE_GET_SUB_CATEGORY:
//		qDebug() << "E_STATE_GET_SUB_CATEGORY";
//		break;

	case E_STATE_GET_DETAIL_INFO:
		qDebug() << "E_STATE_GET_DETAIL_INFO";
		break;

	default:
		qDebug() << "default";
		break;
	}
}

void Paginasamarillas::inputSearchKeyword()
{
	do
	{
		QString lstrSearchURL;
		QString lstrKeyword;

		lstrKeyword = gstrKeyword[miCurKeyword];

		lstrSearchURL = QString(SEARCH_URL).arg(lstrKeyword);
		meState = E_STATE_GET_MAIN_CATEGORY;
//		meState = E_STATE_GET_DETAIL_INFO;
		loadPage(lstrSearchURL);
	}while(false);
}

void Paginasamarillas::getItemList()
{
	QWebElement lele;
	QWebElementCollection leCol;
	QString lstrTmp;
	QStringList llstTmp;

	do
	{
		leCol = mpWebView->document().findAll("a[class=\"telefono\"]");
		foreach (lele, leCol)
		{
			// Parse item url
			lstrTmp = lele.attribute("href");
			llstTmp = lstrTmp.split(";");
			if (llstTmp.size() < 2)
				continue;

			lstrTmp = llstTmp[2];
			llstTmp = lstrTmp.split(",");
			if (llstTmp.size() < 6)
				continue;

			mlstDataLink << llstTmp[6];
		} // foreach


	} while(false);
}

//bool Paginasamarillas::gotoNextState()
//{
//	bool	lblRet = false;

//	do
//	{
//		miCurState++;
//		if(miCurState >= mlstState.size())
//		{
//			qDebug() << "Reaches end of State list";
//			DEF_LOG("Reaches end of State list");
//			break;
//		}

//		setStatus(QString("Current state [%1]").arg(mlstState[miCurState]));
//		DEF_LOG(QString("Current state [%1]").arg(mlstState[miCurState]));

//		lblRet = true;
//	}while(false);

//	return lblRet;
//}

void Paginasamarillas::getDetailData()
{
	QWebElement	lele, leSub1, leSub2, leTmp;
	QWebElementCollection	leCol, leSubCol;
	QString lstrTmp;

	do
	{
		leCol = mpWebView->document().findAll("li[class=\"whitebox vcard\"]");
		foreach (lele, leCol)
		{
			mData.clearData();

			leSub1 = lele.findFirst("div[class=\"microficha\"]");

			// Name
			leSub2 = leSub1.findFirst("a");
			lstrTmp = "";
			if (leSub2.isNull() == false)
				lstrTmp = leSub2.toPlainText();
			mData.updateProperty(mDbLstColName[eName], lstrTmp);

			// Address
			leSub2 = leSub1.findFirst("p[class=\"adr\"]");
			lstrTmp = "";
			if (leSub2.isNull() == false)
			{
				lstrTmp = leSub2.toPlainText();
				mData.updateProperty(mDbLstColName[eFullAdd], lstrTmp);

				leTmp = leSub2.findFirst("span[class=\"postal-code\"]");
				lstrTmp = "";
				if (leTmp.isNull() == false)
					lstrTmp = leTmp.toPlainText();
				mData.updateProperty(mDbLstColName[ePostCode], lstrTmp);

				lstrTmp = "";
				leTmp = leSub2.findFirst("span[class=\"locality\"]");
				if (leTmp.isNull() == false)
					lstrTmp = leTmp.toPlainText();
				mData.updateProperty(mDbLstColName[eLocality], lstrTmp);

				lstrTmp = "";
				leTmp = leSub2.findFirst("span[class=\"region\"]");
				if (leTmp.isNull() == false)
					lstrTmp = leTmp.toPlainText();
				mData.updateProperty(mDbLstColName[eRegion], lstrTmp);
			}

			// Website
			lstrTmp = "";
			leSub1 = lele.findFirst("*#u3");
			if (leSub1.isNull() == false)
			{
				lstrTmp = "";
			}
			mData.updateProperty(mDbLstColName[eWebsite], lstrTmp);
		}
	} while(false);
}

WebEngine_Main(Paginasamarillas)
