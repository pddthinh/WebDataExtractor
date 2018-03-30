#ifndef icatcare_org_H
#define icatcare_org_H

#include "WDEInclude.h"

#include <QDebug>
#include <QWebElement>
#include <QWebElementCollection>

using namespace nsBaseWebEngine;

class icatcare_org : public BaseWebEngine
{
	Q_OBJECT

public:
	icatcare_org(QWidget *apParent = 0);

protected:
	void doStart();
	void doStop(){}
	void doWebLoadFinished();
	void initDatabase();
	void initExtWidget() {
		mpExtFileLoading = new ExtFileLoading(this, mpAppPref);
	}

private:
	QStringList		mEmptyDataLinks;
	QString			mNameSaved;

private:
	void getCatBreeds();
	void getCatDetail();
};

#endif // icatcare_org_H
