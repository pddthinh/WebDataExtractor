#ifndef inca_ltd_org_uk_H
#define inca_ltd_org_uk_H

#include "WDEInclude.h"

#include <QDebug>
#include <QTimer>
#include <QWebElement>
#include <QWebElementCollection>

using namespace nsBaseWebEngine;

class inca_ltd_org_uk : public BaseWebEngine
{
	Q_OBJECT

public:
	inca_ltd_org_uk(QWidget *apParent = 0);

protected:
	void doStart();
	void doStop(){}
	void doWebLoadFinished();
	void initDatabase();
	void initExtWidget(){}

private Q_SLOTS:
	void sltWaitTimeout();

private:
	bool initSearching();
	void parseDetail();

private:
	int     miCurProject;
	int     miCurRegion;

	int		miMaxProject;
	int		miMaxRegion;

	QTimer	*mpTimer;
};

#endif // inca_ltd_org_uk_H
