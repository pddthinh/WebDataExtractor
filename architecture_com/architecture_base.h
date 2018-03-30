#ifndef ARCHITECTURE_BASE_H
#define ARCHITECTURE_BASE_H

#include "WDEInclude.h"

#include <QDebug>
#include <QWebElement>
#include <QWebElementCollection>

using namespace nsBaseWebEngine;

class architecture_base : public BaseWebEngine
{
	Q_OBJECT

public:
	architecture_base(QString title, QWidget *apParent = 0);

	enum enuDatabase
	{
		eName = 0,
		eAdd, eTown, eCounty, eCountry, ePostCode, eFullAdd,
		eTel, eFax,
		eWeb, eEmail,
		eFirstname, eLastname, eFullName,
		eItemLink,
		eDatabaseMax
	};

protected:
	virtual void doStart() = 0;
	virtual void doWebLoadFinished() = 0;

	void doStop(){}
	void initDatabase();
	void initExtWidget()
	{
		mpExtFileLoading = new ExtFileLoading(this);
	}

	bool gotoNextResultPage();
	int getTotalPage();
	void getDetailData(bool ablCompany = false);

protected:
	QWebElement					mElement;
	QWebElementCollection		mCol;
	Logger		*mpLinkLogger;
	int			miPageTotal;
	int			miPageIdx;
};

#endif // ARCHITECTURE_BASE_H
