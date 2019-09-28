#ifndef british_horse_society_org_uk_H
#define british_horse_society_org_uk_H

#include "WDEInclude.h"

#include <QDebug>
#include <QWebElement>
#include <QWebElementCollection>

using namespace nsBaseWebEngine;

typedef enum ENU_PAGE {
	PAGE_RIDING_CLUB,
	PAGE_PLACE_2_RIDE,

} PAGE_TYPE;

class british_horse_society_org_uk : public BaseWebEngine
{
	Q_OBJECT

public:
	british_horse_society_org_uk(QWidget *apParent = 0);
	void setWndInfo(WND_INFO *pInfo) {
		mpWndInfo = pInfo;
	}
	void initConfiguration();
	inline void setDBManager(DBManager *pManager) {
		this->mpDBManager = pManager;
	}

protected:
	void doStart();
	void doStop(){}
	void doWebLoadFinished();
	void initDatabase();
	inline void initExtWidget() {
		mpExtFileLoading = new ExtFileLoading(this, mpAppPref);
	}

private:
	void getSearchClubLink();
	void getClubDetail();

private Q_SLOTS:
	void stopped();

private:
	WND_INFO		*mpWndInfo;
	QString			mTmpTableName;
};

#endif // british_horse_society_org_uk_H
