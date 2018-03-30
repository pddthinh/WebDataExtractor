#ifndef PETERBLACKSHAW_H
#define PETERBLACKSHAW_H

#include "real_estate.h"

class peterblackshaw : public real_estate
{
public:
	peterblackshaw(QString url, QWidget *apParent = NULL);

protected:
	void doWebLoadFinished();
	void parseData();
	void parseOfficeContact();
};

#endif // PETERBLACKSHAW_H
