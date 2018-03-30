#ifndef DIGNAM_H
#define DIGNAM_H

#include "real_estate.h"

class dignam : public real_estate
{
public:
	dignam(QString url);

protected:
	void doWebLoadFinished();
	void parseData();
};

#endif // DIGNAM_H
