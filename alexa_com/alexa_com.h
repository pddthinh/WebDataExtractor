#ifndef alexa_com_H
#define alexa_com_H

#include "WDEInclude.h"

#include <QDebug>
#include <QWebElement>
#include <QWebElementCollection>

using namespace nsBaseWebEngine;

class alexa_com : public BaseWebEngine
{
	Q_OBJECT

public:
	alexa_com(QWidget *apParent = 0);

protected:
	void doStart();
	void doStop(){}
	void doWebLoadFinished();
	void initDatabase();
	void initExtWidget();

private:
	void getDataLinkFromFile();
	void initSearchCategory();
	bool gotoNextSearchCategory();

	void browseResultUrl();
	void getResultWebList();
	void parseDetailData();

private:
	int			miCurPage;
	int			miCurCategory;
	QStringList	mlstSearchCategory;
};

#endif // alexa_com_H
