/**
  * NOTE: Still need more implementation
  *
  */

#include "extmainwnd.h"

ExtMainWnd::ExtMainWnd(QWidget *parent)
	: QWidget(parent)
{
//	// This does not work
//	initWndDataInfo();

	QHBoxLayout *lpLayout = new QHBoxLayout();

	mpLabel = new QLabel("Select sub item web page");
	mpBtnExit = new QPushButton("Exit");
	mpCombo = new QComboBox();

	WND_INFO item;
	foreach (item, mLstWndInfo) {
		mpCombo->addItem(item.name);
	}

	lpLayout->addWidget(mpLabel);
	lpLayout->addWidget(mpCombo);
	lpLayout->addWidget(mpBtnExit);

	setLayout(lpLayout);

	connect(mpBtnExit, SIGNAL(clicked()), this, SLOT(close()));
	connect(mpCombo, SIGNAL(activated(int)), this, SLOT(sltSelectItem(int)));

//	foreach (item, mLstWndInfo)
//		initEngine(item.engine);
}

ExtMainWnd::~ExtMainWnd(){ }

void ExtMainWnd::sltSelectItem(int aiIdx) {

}

