#ifndef CONDUITAGENCIES_H
#define CONDUITAGENCIES_H

#include "real_estate.h"

class conduitagencies : public real_estate
{
public:
	conduitagencies(QString url);

protected:
	void doWebLoadFinished();
	void parseData();
};

#endif // CONDUITAGENCIES_H
