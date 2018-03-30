#ifndef MAGAINAGAIN_H
#define MAGAINAGAIN_H

#include "real_estate.h"

class magainagain : public real_estate
{
public:
	magainagain(QString url);

protected:
	void doWebLoadFinished();
	void parseData();
};

#endif // MAGAINAGAIN_H
