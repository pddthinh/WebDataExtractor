#ifndef yellowpage_au_H
#define yellowpage_au_H

#include "WDEInclude.h"

#include <QDebug>
#include <QWebElement>
#include <QWebElementCollection>
#include <QTimer>


using namespace nsBaseWebEngine;

class yellowpage_au : public BaseWebEngine
{
public:
	yellowpage_au(QWidget *apParent = 0);

	Q_OBJECT

protected:
	void doStart();
	void doStop() {}
	void doWebLoadFinished();
	void initDatabase();
	void initExtWidget() { mpExtFileLoading = new ExtFileLoading(this); }

protected:
	bool loadStates();
	bool loadKeyword();
	bool gotoNextState();
	bool gotoNextKeyword();

	void inputSearchKeyword();

	void extractData();

	void getMainCategoryLink();

	bool gotoNextResultPage();

	void doWait();

private Q_SLOTS:
	void sltLoadTimeout();

private:
	Logger		*mpDataLinkLog;
	Logger		*mpSkippedLinkLog;

	QLineEdit	*mpTxtNumberOfSkipItem;
	QLineEdit	*mpTxtLoadTimeout;

	QStringList	mlstState;
	int			miCurState;

	QStringList	mlstKeyword;
	int			miCurKeyword;
	int			miPageCount;

	Logger		*mpYPLogger;
	QTimer		*mpLoadTimer;

	QString		mcurState;
};

#endif // yellowpage_au_H
