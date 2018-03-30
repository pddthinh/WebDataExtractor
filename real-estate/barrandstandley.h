#ifndef BARRANDSTANDLEY_H
#define BARRANDSTANDLEY_H

#include "real_estate.h"

class barrandstandley : public real_estate
{
public:
	barrandstandley(QString url);

protected:
	void doWebLoadFinished();
	void getOfficeContact();
	void getAgentProfile();
	void parseData();
};

#endif // BARRANDSTANDLEY_H
