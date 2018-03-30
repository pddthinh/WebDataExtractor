#ifndef ROBERTCRAWFORDREALESTATE_H
#define ROBERTCRAWFORDREALESTATE_H

#include "real_estate.h"

class robertcrawfordrealestate : public real_estate
{
public:
	robertcrawfordrealestate(QString url);

protected:
	void doWebLoadFinished();
	void parseData();
};

#endif // ROBERTCRAWFORDREALESTATE_H
