#include "mainwnd.h"

#include <QDebug>
#include <QLineEdit>
#include <QPushButton>
#include <QGroupBox>
#include <QGridLayout>
#include <QFileDialog>

MainWnd::MainWnd(QWidget *parent) :
	QWidget(parent)
{
	initControl();

	mpMediaFire = new MediaFire();
}

void MainWnd::initControl()
{
	QGridLayout	*lpMainLayout = new QGridLayout();

	QGridLayout	*lpURLLayout = new QGridLayout();
	QGroupBox	*lpURLGroup = new QGroupBox("URL Info");
	mpTxtURL = new QLineEdit();
	mpTxtLoadPath = new QLineEdit();
	mpBtnBrowseOpen = new QPushButton("...");
	lpURLLayout->addWidget(new QLabel("URL"), 0, 0);
	lpURLLayout->addWidget(mpTxtURL, 0, 1, 1, 2);
	lpURLLayout->addWidget(new QLabel("Load path"), 1, 0);
	lpURLLayout->addWidget(mpTxtLoadPath, 1, 1);
	lpURLLayout->addWidget(mpBtnBrowseOpen, 1, 2, 1, 1, Qt::AlignCenter);
	lpURLGroup->setLayout(lpURLLayout);

	QGridLayout	*lpPwdLayout = new QGridLayout();
	QGroupBox	*lpPwdGrp = new QGroupBox("Password Info");
	mpTxtPwdDownload = new QLineEdit();
	mpTxtPwdExtract = new QLineEdit();
	lpPwdLayout->addWidget(new QLabel("Password Download"), 0, 0);
	lpPwdLayout->addWidget(mpTxtPwdDownload, 0, 1);
	lpPwdLayout->addWidget(new QLabel("Password Extract"), 1, 0);
	lpPwdLayout->addWidget(mpTxtPwdExtract, 1, 1);
	lpPwdGrp->setLayout(lpPwdLayout);

	QGridLayout	*lpSaveLayout = new QGridLayout();
	QGroupBox	*lpSaveGrp = new QGroupBox("Save Info");
	mpTxtSavePath = new QLineEdit();
	mpBtnBrowseSave = new QPushButton("...");
	lpSaveLayout->addWidget(new QLabel("Save to"), 0, 0);
	lpSaveLayout->addWidget(mpTxtSavePath, 0, 1);
	lpSaveLayout->addWidget(mpBtnBrowseSave, 0, 2);
	lpSaveGrp->setLayout(lpSaveLayout);

	mpBtnDownload = new QPushButton("Download");
	mpBtnSaveInfo = new QPushButton("Save");
	mpBtnReset = new QPushButton("Reset");
	mpBtnCancel = new QPushButton("Cancel");


	lpMainLayout->addWidget(lpURLGroup, 0, 0, 1, 3);
	lpMainLayout->addWidget(lpPwdGrp, 1, 0, 1, 3);
	lpMainLayout->addWidget(lpSaveGrp, 2, 0, 1, 3);
	lpMainLayout->addWidget(mpBtnDownload, 3, 0);
	lpMainLayout->addWidget(mpBtnSaveInfo, 3, 1);
	lpMainLayout->addWidget(mpBtnReset, 3, 2);
	lpMainLayout->addWidget(mpBtnCancel, 4, 2);
	setLayout(lpMainLayout);

	connect(mpBtnCancel, SIGNAL(clicked()), this, SLOT(close()));
	connect(mpBtnBrowseOpen, SIGNAL(clicked()), this, SLOT(sltClickBrowseOpen()));
	connect(mpBtnBrowseSave, SIGNAL(clicked()), this, SLOT(sltClickBrowseSave()));
	connect(mpBtnDownload, SIGNAL(clicked()), this, SLOT(sltClickDownload()));
	connect(mpBtnSaveInfo, SIGNAL(clicked()), this, SLOT(sltClickSave()));
	connect(mpBtnReset, SIGNAL(clicked()), this, SLOT(sltClickReset()));
}

void MainWnd::sltClickBrowseOpen()
{
	mpTxtLoadPath->setText("");
	mpTxtLoadPath->setText(QFileDialog::getOpenFileName(this, "Open data file"));

	// Clear the url text
	if(mpTxtLoadPath->text() != "")
	{
		mpTxtURL->setText("");
		mpTxtURL->setEnabled(false);
	}
}

void MainWnd::sltClickBrowseSave()
{

}

void MainWnd::sltClickDownload()
{
	MediaFire::DOWN_MODE leMode;

	if(mpTxtURL->text() != "")
	{
		// Download from url
		mpMediaFire->setBaseURL(mpTxtURL->text());
		leMode = MediaFire::E_MODE_FROM_MEDIA_FOLDER;
	} else {
		// Downlad from file
		mpMediaFire->setSavedLinkPath(mpTxtLoadPath->text());
		leMode = MediaFire::E_MODE_FROM_SAVED_FILE;
	}

	mpMediaFire->startDownload(leMode);
	mpMediaFire->showNormal();
//	mpMediaFire->exec();
}

void MainWnd::sltClickSave()
{

}

void MainWnd::sltClickReset()
{
	mpTxtURL->setEnabled(true);

	mpTxtURL->clear();
	mpTxtLoadPath->clear();
	mpTxtPwdDownload->clear();
	mpTxtSavePath->clear();
}
