#include "mcsmainwnd.h"

#include "mcsproduct.h"
#include <QHBoxLayout>
#include <QDebug>

MCSMainWnd::MCSMainWnd(QWidget *apParent)
	:QWidget(apParent)
{
	QHBoxLayout *lpLayout = new QHBoxLayout();

	mpLabel = new QLabel("Select MCS web page");
	mpBtnExit = new QPushButton("Exit");
	mpCombo = new QComboBox();

	QStringList llstItem;
	llstItem << "MCS Installer" << "MCS Product";
	mpCombo->insertItems(0, llstItem);

	lpLayout->addWidget(mpLabel);
	lpLayout->addWidget(mpCombo);
	lpLayout->addWidget(mpBtnExit);

	setLayout(lpLayout);

	connect(mpBtnExit, SIGNAL(clicked()), this, SLOT(close()));
	connect(mpCombo, SIGNAL(activated(int)), this, SLOT(sltSelectItem(int)));

	initMCSInstaller();
	initMCSProduct();
}

void MCSMainWnd::sltSelectItem(int aiIdx)
{
	switch(aiIdx)
	{
	case 0:
		mpMCSInstaller->showMaximized();
		break;

	case 1:
		mpMCSProduct->showMaximized();
		break;
	}
}

void MCSMainWnd::initMCSInstaller()
{
	mpMCSInstaller = new MCSInstaller(this);
//	mpLoggerTaskInstaller = new LoggerTask(NULL, "MCSInstaller.sql");

//	connect(mpMCSInstaller, SIGNAL(sigDBSaveData(QString)), mpLoggerTaskInstaller, SLOT(sltDBSaveData(QString)), Qt::QueuedConnection );
//	connect(mpMCSInstaller, SIGNAL(sigDBLogString(QString)), mpLoggerTaskInstaller, SLOT(sltDBLogString(QString)), Qt::QueuedConnection);
////	connect(mpMCSInstaller, SIGNAL(sigSaveDataItem(DataEntry)), mpLoggerTaskInstaller, SLOT(sltSaveDataItem(DataEntry)), Qt::QueuedConnection);

//	DataWorkerThread *lpThread = new DataWorkerThread();
//	mpLoggerTaskInstaller->moveToThread( lpThread );
//	lpThread->start();

//	mpMCSInstaller->setLoggerTask(mpLoggerTaskInstaller);
}

void MCSMainWnd::initMCSProduct()
{
	mpMCSProduct = new MCSProduct(this);
//	mpLoggerTaskProduct = new LoggerTask(NULL, "MCSProduct.sql");

//	connect(mpMCSProduct, SIGNAL(sigDBSaveData(QString)), mpLoggerTaskProduct, SLOT(sltDBSaveData(QString)), Qt::QueuedConnection);
//	connect(mpMCSProduct, SIGNAL(sigDBLogString(QString)), mpLoggerTaskProduct, SLOT(sltDBLogString(QString)), Qt::QueuedConnection);
////	connect(mpMCSProduct, SIGNAL(sigSaveDataItem(DataEntry)), mpLoggerTaskProduct, SLOT(sltSaveDataItem(DataEntry)), Qt::QueuedConnection);

//	DataWorkerThread *lpThread = new DataWorkerThread();
//	mpLoggerTaskProduct->moveToThread( lpThread );
//	lpThread->start();

//	mpMCSProduct->setLoggerTask(mpLoggerTaskProduct);
}
