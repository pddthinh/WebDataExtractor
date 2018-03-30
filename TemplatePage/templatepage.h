#ifndef TEMPLATEPAGE_H
#define TEMPLATEPAGE_H

#include "WDEInclude.h"

#include <QDebug>
#include <QWebElement>
#include <QWebElementCollection>

using namespace nsBaseWebEngine;

class templatepage : public BaseWebEngine
{
	Q_OBJECT

public:
	templatepage(QWidget *apParent = 0);

protected:
	void doStart();
	void doStop(){}
	void doWebLoadFinished();
	void initDatabase();
	void initExtWidget() {
		mpExtFileLoading = new ExtFileLoading(this, mpAppPref);
	}

private:

};

#endif // TEMPLATEPAGE_H
