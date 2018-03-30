#ifndef TLCREALESTATE_H
#define TLCREALESTATE_H

#include "real_estate.h"

class tlcrealestate : public real_estate
{
	Q_OBJECT

public:
	tlcrealestate(QString url, QWidget *apParent = 0);

protected:
	void doWebLoadFinished();
	void parseData();
};

#endif // TLCREALESTATE_H
