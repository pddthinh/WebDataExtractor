#ifndef the_good_dog_guide_com_H
#define the_good_dog_guide_com_H

#include "WDEInclude.h"

#include <QDebug>
#include <QWebElement>
#include <QWebElementCollection>

using namespace nsBaseWebEngine;

class the_good_dog_guide_com : public BaseWebEngine
{
	Q_OBJECT

public:
	the_good_dog_guide_com(QWidget *apParent = 0);

protected:
	void doStart();
	void doStop(){}
	void doWebLoadFinished();
	void initDatabase();
	void initExtWidget() {
		mpExtFileLoading = new ExtFileLoading(this, mpAppPref);
	}

private:
	void getCountyLinks();
	void getGroomerLinks();
	void getGroomerDetail();
	void getGroomerDetailWebLink();
};

#endif // the_good_dog_guide_com_H
