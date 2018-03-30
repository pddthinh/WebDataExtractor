#ifndef NOELJONES_H
#define NOELJONES_H

#include "real_estate.h"

class noeljones : public real_estate
{
public:
	noeljones(QString url);

protected:
	void doWebLoadFinished();
	void parseData();

	void getOfficeList();
	void getOfficeInfo();
};

#endif // NOELJONES_H
