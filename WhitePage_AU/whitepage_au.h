#ifndef WHITEPAGE_AU_H
#define WHITEPAGE_AU_H

#include "WDEInclude.h"

#include <QDebug>
#include <QWebElement>
#include <QWebElementCollection>

using namespace nsBaseWebEngine;

class whitepage_au : public BaseWebEngine
{
public:
	whitepage_au(QWidget *apParent = 0);

protected:
	void doStart();
	void doStop() {}
	void doWebLoadFinished();
	void initDatabase();
	void initExtWidget(){ mpExtFileLoading = new ExtFileLoading(this); }

protected:
	bool gotoNextKeyword();
	bool loadStates();
	bool gotoNextState();

	void inputSearchKeyword();

	bool gotoNextSearchResultPage();
	void getDetailData();

private:
	QStringList		mlstState;
	int				miCurState;
};

#endif // WHITEPAGE_AU_H
