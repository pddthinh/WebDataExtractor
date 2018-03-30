#include "TestContactSeeker.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFileDialog>

TestContactSeeker::TestContactSeeker(QWidget *apParent)
	: QWidget(apParent)
{
	initControl();
	initSeeker();
}

void TestContactSeeker::initControl()
{
	QGridLayout		*lpMainLayout = new QGridLayout();

	mpTxtInputPath = new QLineEdit();
	mpBtnBrowse = new QPushButton("...");
	lpMainLayout->addWidget(new QLabel("Input File"), 0, 0);
	lpMainLayout->addWidget(mpTxtInputPath, 0, 1);
	lpMainLayout->addWidget(mpBtnBrowse, 0, 2);

	mpTxtStatus = new QLineEdit();
	lpMainLayout->addWidget(new QLabel("Status"), 1, 0);
	lpMainLayout->addWidget(mpTxtStatus, 1, 1, 1, 2);
	mpTxtStatus->setReadOnly(true);

	QHBoxLayout		*lpBtnLayout = new QHBoxLayout();
	mpBtnStart = new QPushButton("Start");
	mpBtnStop = new QPushButton("Stop");
	mpBtnExit = new QPushButton("Exit");
	lpBtnLayout->addWidget(mpBtnStart);
	lpBtnLayout->addWidget(mpBtnStop);
	lpBtnLayout->addWidget(mpBtnExit);
	lpMainLayout->addLayout(lpBtnLayout, 2, 0, 1, 3);

	connect(mpBtnExit, SIGNAL(clicked()), this, SLOT(close()));
	connect(mpBtnStart, SIGNAL(clicked()), this, SLOT(sltBtnStartClick()));
	connect(mpBtnStop, SIGNAL(clicked()), this, SLOT(sltBtnStopClick()));
	connect(mpBtnBrowse, SIGNAL(clicked()), this, SLOT(sltBtnBrowseClick()));

	setLayout(lpMainLayout);
}

void TestContactSeeker::initSeeker()
{
	mpSeeker = new ContactSeeker(this);
	connect(mpSeeker, SIGNAL(sigFinishGetOneContact(ContactSeeker::TAG_CONTACT_INFO)), this, SLOT(sltSeekerFinishedGet1Contact(ContactSeeker::TAG_CONTACT_INFO)));
	connect(mpSeeker, SIGNAL(sigStatusMsgChanged(QString)), this, SLOT(sltSeekerStatusChanged(QString)));
	connect(mpSeeker, SIGNAL(sigProcessFinished(ContactSeeker::ProcessStatus)), this, SLOT(sltSeekerProcessFinished(ContactSeeker::ProcessStatus)));
}

void TestContactSeeker::sltBtnBrowseClick()
{
	QString lstrPath = QFileDialog::getOpenFileName(this, "Select file for load");

	mpTxtInputPath->clear();

	if(lstrPath != NULL)
		mpTxtInputPath->setText(lstrPath);
}

void TestContactSeeker::sltBtnStartClick()
{
	do
	{
		if (mpSeeker == NULL)
			break;

		mpSeeker->stop();
		mpSeeker->loadInputWebsiteList(mpTxtInputPath->text());
		mpSeeker->start();
	} while(false);
}

void TestContactSeeker::sltBtnStopClick()
{
	do
	{
		if (mpSeeker == NULL)
			break;

		mpSeeker->stop();
	} while(false);
}

void TestContactSeeker::sltSeekerStatusChanged(QString astrStatus)
{
	mpTxtStatus->setText(astrStatus);
}

void TestContactSeeker::sltSeekerProcessFinished(ContactSeeker::ProcessStatus aStatus)
{
	qDebug() << "Seeker process finished, status: " << aStatus;
}

void TestContactSeeker::sltSeekerFinishedGet1Contact(ContactSeeker::TAG_CONTACT_INFO atInfo)
{
	qDebug() << "Finished get 1 contact, info: " << atInfo.toString();
}
