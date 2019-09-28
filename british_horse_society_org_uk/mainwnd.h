#ifndef MAINWND_H
#define MAINWND_H

#include "WDEInclude.h"
#include "british_horse_society_org_uk.h"

#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>

class MainWnd : public QWidget
{
	Q_OBJECT
public:
	explicit MainWnd(QWidget *parent = 0);
	~MainWnd();

protected slots:
	void sltSelectItem(int aiIdx);
	void sltEngineExit();

protected:
	void initWndDataInfo();
	void initEngine(BaseWebEngine *engine);

protected:
	QLabel			*mpLabel;
	QComboBox		*mpCombo;
	QPushButton		*mpBtnExit;

	british_horse_society_org_uk *mpSelectedEngine;

	QList<WND_INFO*> mLstWndInfo;
	DBManager		*mpDBManager;
};

#endif // MAINWND_H
