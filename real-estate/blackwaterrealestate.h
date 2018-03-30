#ifndef BLACKWATERREALESTATE_H
#define BLACKWATERREALESTATE_H

#include "real_estate.h"

class blackwaterrealestate : public real_estate
{
public:
	blackwaterrealestate(QString url);

protected:
	void doWebLoadFinished();
	void parseData();
};

#endif // BLACKWATERREALESTATE_H
