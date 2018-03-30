#ifndef inc_com_H
#define inc_com_H

#include "WDEInclude.h"

#include <QDebug>
#include <QWebElement>
#include <QWebElementCollection>

using namespace nsBaseWebEngine;

class inc_com : public BaseWebEngine
{
	Q_OBJECT

public:
	inc_com(QWidget *apParent = 0);

protected:
	void doStart();
	void doStop(){}
	void doWebLoadFinished();
	void initDatabase();
	void initExtWidget(){}

private:
	void getAllCompanyList();
	bool gotoNextResultPage();
	void getDetailCompanyInfo();
};

#endif // inc_com_H
