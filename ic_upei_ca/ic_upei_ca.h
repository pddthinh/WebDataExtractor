#ifndef ic_upei_ca_H
#define ic_upei_ca_H

#include "WDEInclude.h"

#include <QDebug>
#include <QWebElement>
#include <QWebElementCollection>

using namespace nsBaseWebEngine;

class ic_upei_ca : public BaseWebEngine
{
	Q_OBJECT

public:
	ic_upei_ca(QWidget *apParent = 0);

protected:
	void doStart();
	void doStop(){}
	void doWebLoadFinished();
	void initDatabase();
	void initExtWidget() {
		mpExtFileLoading = new ExtFileLoading(this, mpAppPref);
	}

private:
	void getBreedsList();
	void parseBreedDetail();
};

#endif // ic_upei_ca_H
