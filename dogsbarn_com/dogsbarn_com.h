#ifndef dogsbarn_com_H
#define dogsbarn_com_H

#include "WDEInclude.h"

#include <QDebug>
#include <QWebElement>
#include <QWebElementCollection>

using namespace nsBaseWebEngine;

class dogsbarn_com : public BaseWebEngine
{
	Q_OBJECT

public:
	dogsbarn_com(QWidget *apParent = 0);

protected:
	void doStart();
	void doStop(){}
	void doWebLoadFinished();
	void initDatabase();
	void initExtWidget() {
		mpExtFileLoading = new ExtFileLoading(this, mpAppPref);
	}

private:
	void getAllRescueCenter();
};

#endif // dogsbarn_com_H
