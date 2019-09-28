#ifndef EXTMAINWND_H
#define EXTMAINWND_H

#include "WDEInclude.h"

#include <QWidget>
#include <QList>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>

using namespace nsBaseWebEngine;

class ExtMainWnd : public QWidget
{
	Q_OBJECT
public:
	explicit ExtMainWnd(QWidget *parent = 0);
	~ExtMainWnd();

signals:

protected slots:
	void sltSelectItem(int aiIdx);

protected:
	virtual void initWndDataInfo() = 0;
	virtual void initEngine(BaseWebEngine *engine) = 0;

protected:
	QLabel			*mpLabel;
	QComboBox		*mpCombo;
	QPushButton		*mpBtnExit;

	QList<WND_INFO> mLstWndInfo;
};

#endif // EXTMAINWND_H
