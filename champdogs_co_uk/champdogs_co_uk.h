#ifndef champdogs_co_uk_H
#define champdogs_co_uk_H

#include "WDEInclude.h"

#include <QDebug>
#include <QWebElement>
#include <QWebElementCollection>

using namespace nsBaseWebEngine;

class champdogs_co_uk : public BaseWebEngine
{
	Q_OBJECT

public:
	champdogs_co_uk(QWidget *apParent = 0);

protected:
	void doStart();
	void doStop(){}
	void doWebLoadFinished();
	void initDatabase();
	void initExtWidget() {
		mpExtFileLoading = new ExtFileLoading(this, mpAppPref);
	}

private:
	void getBreederCategories();
	void getBreederList();
	void getBreederDetail();
	void getBreederWebsite();

private:
	QStringList mBreederCategories;
};

#endif // champdogs_co_uk_H
