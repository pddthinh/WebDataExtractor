#ifndef MCSMAINWND_H
#define MCSMAINWND_H

#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>

#include "mcsproduct.h"
#include "mcsinstaller.h"

class MCSMainWnd : public QWidget
{
	Q_OBJECT

public:
	MCSMainWnd(QWidget *apParent = 0);

protected:
	void initMCSInstaller();
	void initMCSProduct();

protected slots:
	void sltSelectItem(int aiIdx);

protected:
	QLabel			*mpLabel;
	QComboBox		*mpCombo;
	QPushButton		*mpBtnExit;

	MCSProduct		*mpMCSProduct;
	MCSInstaller	*mpMCSInstaller;

	LoggerTask		*mpLoggerTaskInstaller;
	LoggerTask		*mpLoggerTaskProduct;
};

#endif // MCSMAINWND_H
