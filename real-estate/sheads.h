#ifndef SHEADS_H
#define SHEADS_H

#include "real_estate.h"

class sheads : public real_estate
{
public:
	sheads(QString url);

protected:
	void doWebLoadFinished();
	void getOfficeContact();
	void getTeamInfoLink();
	void getAgentList();
	void parseData();
};

#endif // SHEADS_H
