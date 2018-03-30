#ifndef RTFORSYTH_H
#define RTFORSYTH_H

#include "real_estate.h"

class rtforsyth : public real_estate
{
public:
	rtforsyth(QString url);

protected:
	void doWebLoadFinished();
	void parseData();
};

#endif // RTFORSYTH_H
