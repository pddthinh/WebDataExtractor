#ifndef Paginasamarillas_es_H
#define Paginasamarillas_es_H

#include "WDEInclude.h"

#include <QDebug>
#include <QWebElement>
#include <QWebElementCollection>

using namespace nsBaseWebEngine;

class Paginasamarillas : public BaseWebEngine
{
public:
		Paginasamarillas(QWidget *apParent = 0);

protected:
	void doStart();
	void doStop(){}
	void doWebLoadFinished();
	void initDatabase();
	void initExtWidget();
///////////////////////////////////////////////////////////

protected:
	void inputSearchKeyword();
	bool gotoNextState();

	void getItemList();
	void getDetailData();

private:
	Logger		*mpDataLinkLog;

	QCheckBox	*mpChkBoxGetLink;
	bool		mblGetDataLinkOnly;

	QCheckBox	*mpChkBoxStartFromFile;
	bool		mblStartFromFile;

	QLineEdit	*mpTxtNumberOfSkipItem;

	QStringList	mlstState;
	int			miCurState;
};

#endif // Paginasamarillas_es_H
