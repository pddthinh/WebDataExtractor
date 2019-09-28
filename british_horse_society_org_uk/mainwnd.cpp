#include "mainwnd.h"
#include "british_horse_society_org_uk.h"

MainWnd::MainWnd(QWidget *parent)
	: QWidget(parent)
{
	///////////////////////////////////////////////////////////
	QString homeDir(::getenv("HOME"));
	mpDBManager = new DBManager(homeDir + "/tmp/british_horse_society_org_uk.db");
	///////////////////////////////////////////////////////////

	initWndDataInfo();

	QHBoxLayout *lpLayout = new QHBoxLayout();

	mpLabel = new QLabel("Select sub item web page");
	mpBtnExit = new QPushButton("Exit");
	mpCombo = new QComboBox();

	WND_INFO* item;
	foreach (item, mLstWndInfo) {
		mpCombo->addItem(item->name);
	}

	lpLayout->addWidget(mpLabel);
	lpLayout->addWidget(mpCombo);
	lpLayout->addWidget(mpBtnExit);

	setLayout(lpLayout);

	connect(mpBtnExit, SIGNAL(clicked()), this, SLOT(close()));
	connect(mpCombo, SIGNAL(activated(int)), this, SLOT(sltSelectItem(int)));

	foreach (item, mLstWndInfo)
		initEngine(item->engine);
}

MainWnd::~MainWnd() {
	if (mpDBManager != NULL)
		mpDBManager->close();

	WND_INFO *item;
	foreach (item, mLstWndInfo) {
		if (item->engine != NULL)
			delete item->engine;
		delete item;
	}
}

void MainWnd::initWndDataInfo() {
	WND_INFO *pInf;

	pInf = new WND_INFO();
	pInf->name = QLatin1String("Riding Clubs");
	pInf->url = QLatin1String("http://www.bhs.org.uk/enjoy-riding/british-riding-clubs/find-a-riding-club/results?&perPage=25&page=1");
	pInf->tableName = QLatin1String("british-riding-clubs");
	pInf->engine = new british_horse_society_org_uk(this);
	((british_horse_society_org_uk *)pInf->engine)->setWndInfo(pInf);
	((british_horse_society_org_uk *)pInf->engine)->setDBManager(mpDBManager);
	mLstWndInfo << pInf;

	pInf = new WND_INFO();
	pInf->name = QLatin1String("Place to ride");
	pInf->url = QLatin1String("https://www.bhs.org.uk/enjoy-riding/find-a-place-to-ride/results?&perPage=25&page=1");
	pInf->tableName = QLatin1String("place-to-ride");
	pInf->engine = new british_horse_society_org_uk(this);
	((british_horse_society_org_uk *)pInf->engine)->setWndInfo(pInf);
	((british_horse_society_org_uk *)pInf->engine)->setDBManager(mpDBManager);
	mLstWndInfo << pInf;

	pInf = new WND_INFO();
	pInf->name = QLatin1String("Professinal coach");
	pInf->url = QLatin1String("https://www.bhs.org.uk/enjoy-riding/find-an-accredited-professional--coach/results?&perPage=25&page=1");
	pInf->tableName = QLatin1String("professional-coach");
	pInf->engine = new british_horse_society_org_uk(this);
	((british_horse_society_org_uk *)pInf->engine)->setWndInfo(pInf);
	((british_horse_society_org_uk *)pInf->engine)->setDBManager(mpDBManager);
	mLstWndInfo << pInf;

	pInf = new WND_INFO();
	pInf->name = QLatin1String("Livery yard");
	pInf->url = QLatin1String("http://www.bhs.org.uk/advice-and-information/find-a-livery-yard/results?&perPage=25&page=1");
	pInf->tableName = QLatin1String("place-to-ride");
	pInf->engine = new british_horse_society_org_uk(this);
	((british_horse_society_org_uk *)pInf->engine)->setWndInfo(pInf);
	((british_horse_society_org_uk *)pInf->engine)->setDBManager(mpDBManager);
	mLstWndInfo << pInf;

	mpSelectedEngine = NULL;
}

void MainWnd::initEngine(BaseWebEngine *based) {
	british_horse_society_org_uk *engine = (british_horse_society_org_uk*)based;

	connect(engine, SIGNAL(finished(int)), this, SLOT(sltEngineExit()));

	engine->initConfiguration();
}

void MainWnd::sltSelectItem(int aiIdx) {
	this->hide();

	if (mpSelectedEngine != NULL)
		mpSelectedEngine->hide();

	mpSelectedEngine = (british_horse_society_org_uk *)mLstWndInfo.at(aiIdx)->engine;
	mpSelectedEngine->showNormal();
}

void MainWnd::sltEngineExit() {
	this->show();
}
