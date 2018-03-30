#ifndef erkendhypotheekadviseur_H
#define erkendhypotheekadviseur_H

#include "WDEInclude.h"

#include <QDebug>
#include <QWebElement>
#include <QWebElementCollection>

using namespace nsBaseWebEngine;

class erkendhypotheekadviseur : public BaseWebEngine
{
public:
	erkendhypotheekadviseur(QWidget *apParent = 0);

protected:
	void doStart();
	void doStop();
	void doWebLoadFinished();
	void initDatabase();

private:
	int		miCurPage;

protected:
	bool gotoNextZipCode();
	void inputZipCode();

	void getMainCategory();
	bool gotoNextResultLink();
};

#endif // erkendhypotheekadviseur_H
