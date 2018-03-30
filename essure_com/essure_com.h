#ifndef essure_com_H
#define essure_com_H

#include "WDEInclude.h"

#include <QDebug>
#include <QWebElement>
#include <QWebElementCollection>

using namespace nsBaseWebEngine;

class essure_com : public BaseWebEngine
{
	Q_OBJECT

public:
	essure_com(QWidget *apParent = 0);

protected:
	void doStart();
	void doStop(){}
	void doWebLoadFinished();
	void initDatabase();
	void initExtWidget(){}

private:
	int		miCurState;

private:
	void initSearch();
	void selectAllResult();
	void getDetailData();
};

#endif // essure_com_H
