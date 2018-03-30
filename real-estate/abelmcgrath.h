#ifndef ABELMCGRATH_H
#define ABELMCGRATH_H

#include "real_estate.h"

class abelmcgrath : public real_estate
{
public:
	abelmcgrath(QString url, QWidget *apParent = 0);

protected:
	void doWebLoadFinished();
	void getProfileList();
	void parseData();
};

#endif // ABELMCGRATH_H
