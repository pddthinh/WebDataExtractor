#ifndef JENNYANDREWS_H
#define JENNYANDREWS_H

#include "real_estate.h"

class jennyandrews : public real_estate
{
public:
	jennyandrews(QString url, QWidget *apParent = 0);

protected:
	void doWebLoadFinished();
	void parseData();
};

#endif // JENNYANDREWS_H
