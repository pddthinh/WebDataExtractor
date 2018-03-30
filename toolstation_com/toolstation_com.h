#ifndef toolstation_com_H
#define toolstation_com_H

#include "WDEInclude.h"

#include <QDebug>
#include <QWebElement>
#include <QWebElementCollection>

using namespace nsBaseWebEngine;

class toolstation_com : public BaseWebEngine
{
	Q_OBJECT

public:
	toolstation_com(QWidget *apParent = 0);

protected:
	void doStart();
	void doStop(){}
	void doWebLoadFinished();
	void initDatabase();
	void initExtWidget()
	{
		//mpExtFileLoading = new ExtFileLoading(this);
	}

private:
	void getDepartments();
	bool gotoNextDepartment();
	void getItemCategory();
	void getItemList();

private:
	QStringList mDepartments;
	QWebElement mElement;
	QWebElementCollection mCol;
	Logger *mpLinkLogger;
};

#endif // toolstation_com_H
