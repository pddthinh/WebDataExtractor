#include "architecture_gui.h"

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>

QString gSites[] = {
	"UK Directory of RIBA Chartered Practices",
	"RIBA Chartered Members Directory",
	""
};

architecture_gui::architecture_gui(QWidget *parent) :
	QWidget(parent)
{
	init();
	initList();
}

void architecture_gui::init()
{
	QGridLayout *lpMainLayout = new QGridLayout(this);
	lpMainLayout->addWidget(new QLabel("Site list:"), 0, 0, 1, 2);

	mpList = new QListWidget(this);
	connect(mpList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(sltItemDoubleClick(QListWidgetItem*)));
	lpMainLayout->addWidget(mpList, 1, 0, 1, 2);

	QPushButton *lpBtnExit = new QPushButton("Exit", this);
	connect(lpBtnExit, SIGNAL(clicked()), this, SLOT(close()));
	lpMainLayout->addWidget(lpBtnExit, 2, 1);

	QPushButton *lpBtnExe = new QPushButton("Run", this);
	connect(lpBtnExe, SIGNAL(clicked()), this, SLOT(sltRun()));
	lpMainLayout->addWidget(lpBtnExe, 2, 0);

	this->setLayout(lpMainLayout);
}

void architecture_gui::initList()
{
	QListWidgetItem *lpItem = NULL;
	int i = 0;
	int iType = 0;

	while (gSites[i].isEmpty() == false)
	{
		lpItem = new QListWidgetItem(gSites[i], mpList, iType);
		mpList->addItem(lpItem);

		iType ++;
		i ++;
	}
}

void architecture_gui::sltItemDoubleClick(QListWidgetItem *apItem)
{
	do
	{
		if (apItem == NULL)
			break;

		int iType = apItem->type();
		architecture_base *lpEngine = NULL;

		switch (iType)
		{
		case ENU_SITE_chartered_practice:
			lpEngine = new chartered_practice(this);
			break;

		case ENU_SITE_member_practice:
			lpEngine = new chartered_member(this);
			break;

		default:
			break;
		}

		if (lpEngine == NULL)
			break;

		connect(lpEngine, SIGNAL(finished(int)), this, SLOT(show()));

		this->hide();
#ifdef DEBUG
		lpEngine->showMaximized();
#else
		lpEngine->showNormal();
#endif
	} while (false);
}

void architecture_gui::sltRun()
{
	sltItemDoubleClick(mpList->currentItem());
}
