#ifndef homecheck_co_uk_H
#define homecheck_co_uk_H

#include "WDEInclude.h"

#include <QDebug>
#include <QMap>
#include <QWebElement>
#include <QWebElementCollection>

using namespace nsBaseWebEngine;

class homecheck_co_uk : public BaseWebEngine
{
	Q_OBJECT

public:
	homecheck_co_uk(QWidget *apParent = 0);

protected:
	void doStart();
	void doStop(){}
	void doWebLoadFinished();
	void initDatabase();
	void initExtWidget() {
		mpExtFileLoading = new ExtFileLoading(this, mpAppPref);
	}

private:
	bool loadRegionCodeData();
	bool updateRegionCodeData();
	QString gotoNextPCode(bool blNextRC = false);

	void doSearchPostcode();
	void getSearchPCResult();
	void parseSearchResult();

	bool checkConfirm();

	void doWaitFinished();

private:
	QTimer						*mpTimer;
	QMap<QString, QStringList>	mRCMap;
	QString						mRCode; // current region code
	QString						mPCode; // current postcode
	QStringList					mPCodeList; // current postcode list
};

#endif // homecheck_co_uk_H
