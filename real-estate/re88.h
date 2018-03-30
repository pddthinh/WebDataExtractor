#ifndef RE88_H
#define RE88_H

#include "real_estate.h"

class re88 : public real_estate
{
public:
	re88(QString url, QWidget *apParent = 0);

protected:
	void doWebLoadFinished();
	void getAgentProfile();
	void parseData();
};

#endif // RE88_H
