#ifndef GARYPEER_H
#define GARYPEER_H

#include "real_estate.h"

class garypeer : public real_estate
{
public:
	garypeer(QString url, QWidget *apParent = 0);

protected:
	void doWebLoadFinished();
	void getProfileList();
	void parseSiteContact();
	void parseData();
	void parseOfficeContact();
};

#endif // GARYPEER_H
