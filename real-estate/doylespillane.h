#ifndef DOYLESPILLANE_H
#define DOYLESPILLANE_H

#include "real_estate.h"

class doylespillane : public real_estate
{
public:
	doylespillane(QString url);

protected:
	void doWebLoadFinished();
	void getOfficeContact();
	void getAgentProfile();
	void parseData();
};

#endif // DOYLESPILLANE_H
