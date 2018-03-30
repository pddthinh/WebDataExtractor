#ifndef USGBC_H
#define USGBC_H

#include "WDEInclude.h"

#include <QDebug>
#include <QWebElement>
#include <QWebElementCollection>

using namespace nsBaseWebEngine;

class usgbc : public BaseWebEngine
{
public:
	usgbc(QWidget *apParent = 0);

protected:
	void doStart();
	void doStop();
	void doWebLoadFinished();
	void initDatabase();

protected:
	bool goNextCategory();
	void getMainCategoryList();
	bool goNextSearchResult();
	void getDetailData();

private:
	int				miCurCategory;
	QStringList		mlstCategoryName;
	QStringList		mlstDetailElementId;
//	XMLLogger		*mpXMLLog;
};

#endif // USGBC_H
