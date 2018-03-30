#ifndef CRAWFORDREALTY_H
#define CRAWFORDREALTY_H

#include "real_estate.h"

class crawfordrealty : public real_estate
{
public:
	crawfordrealty(QString url);

protected:
	void doWebLoadFinished();
	void parseData();

	void getAgentLink();
	void getOfficeContact();
};

#endif // CRAWFORDREALTY_H
