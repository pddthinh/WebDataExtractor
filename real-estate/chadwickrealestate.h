#ifndef CHADWICKREALESTATE_H
#define CHADWICKREALESTATE_H

#include "real_estate.h"

class chadwickrealestate : public real_estate
{
public:
	chadwickrealestate(QString url);

protected:
	void doWebLoadFinished();
	void parseData();
	void getOfficeContact();
};

#endif // CHADWICKREALESTATE_H
