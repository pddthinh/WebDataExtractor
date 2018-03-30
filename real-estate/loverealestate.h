#ifndef LOVEREALESTATE_H
#define LOVEREALESTATE_H

#include "real_estate.h"

class loverealestate : public real_estate
{
public:
	loverealestate(QString url, QWidget *apParent = 0);

protected:
	void doWebLoadFinished();
	void parseData();
	void getOfficeContact();
};

#endif // LOVEREALESTATE_H
