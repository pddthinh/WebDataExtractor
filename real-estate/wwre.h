#ifndef WWRE_H
#define WWRE_H

#include "real_estate.h"

class wwre : public real_estate
{
public:
	wwre(QString url, QWidget *apParent = 0);

protected:
	void doStart();
	void doWebLoadFinished();
	void getSiteContact();
	void getProfileList();
	void parseData();
	void parseOtherContact();

private:
	QString mstrOtherContact;
};

#endif // WWRE_H
