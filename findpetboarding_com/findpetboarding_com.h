#ifndef findpetboarding_com_H
#define findpetboarding_com_H

#include "WDEInclude.h"

#include <QDebug>
#include <QWebElement>
#include <QWebElementCollection>

using namespace nsBaseWebEngine;

class findpetboarding_com : public BaseWebEngine
{
	Q_OBJECT

public:
	findpetboarding_com(QWidget *apParent = 0);

protected:
	void doStart();
	void doStop(){}
	void doWebLoadFinished();
	void initDatabase();
	void initExtWidget() {
		mpExtFileLoading = new ExtFileLoading(this, mpAppPref);
	}

private:
	QStringList		mCategoryIDs;
	QStringList		mSubData;

private:
	void getBoardingCagetogyList();
	void searchCategory();
	QString parseCategory();

	void getCategoryLink();
	void getDetailInfo1();

	void getDetailInfo2();
	bool gotoNextSubDataLink();
};

#endif // findpetboarding_com_H
