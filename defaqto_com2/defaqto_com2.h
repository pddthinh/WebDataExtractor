#ifndef defaqto_com2_H
#define defaqto_com2_H

#include "WDEInclude.h"

#include <QDebug>
#include <QWebElement>
#include <QWebElementCollection>

using namespace nsBaseWebEngine;

class defaqto_com2 : public BaseWebEngine
{
	Q_OBJECT

public:
	defaqto_com2(QWidget *apParent = 0);

protected:
	void doStart();
	void doStop(){}
	void doWebLoadFinished();
	void initDatabase();
	void initExtWidget() {
		mpExtFileLoading = new ExtFileLoading(this, mpAppPref);
	}

private:
	void getProductList();
	void getCompanyList();
	void getCompanyDetail();
};

#endif // defaqto_com2_H
