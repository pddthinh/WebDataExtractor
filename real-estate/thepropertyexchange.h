#ifndef THEPROPERTYEXCHANGE_H
#define THEPROPERTYEXCHANGE_H

#include "real_estate.h"

class thepropertyexchange : public real_estate
{
public:
	thepropertyexchange(QString url, QWidget *apParent = 0);

protected:
	void doWebLoadFinished();
	void getAgentList();
	void parseData();
};

#endif // THEPROPERTYEXCHANGE_H
