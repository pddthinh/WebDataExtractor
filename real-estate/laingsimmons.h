#ifndef LAINGSIMMONS_H
#define LAINGSIMMONS_H

#include "real_estate.h"

class laingsimmons : public real_estate
{
public:
	laingsimmons(QString url);
protected:
	void doWebLoadFinished();
	void parseData();
};

#endif // LAINGSIMMONS_H
