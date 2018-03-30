#ifndef tica_acad_co_uk_H
#define tica_acad_co_uk_H

#include "WDEInclude.h"

#include <QDebug>
#include <QTimer>
#include <QWebElement>
#include <QWebElementCollection>

using namespace nsBaseWebEngine;

class tica_acad_co_uk : public BaseWebEngine
{
	Q_OBJECT

public:
	tica_acad_co_uk(QWidget *apParent = 0);

protected:
	void doStart();
	void doStop(){}
	void doWebLoadFinished();
	void initDatabase();
	void initExtWidget(){}

private:
    void getMainList();
	void getDetailData();

private Q_SLOTS:
	void sltWaitTimeout();

private:
	QTimer      *mpTimer;
};

#endif // tica_acad_co_uk_H
