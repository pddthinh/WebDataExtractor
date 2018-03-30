#ifndef REALMARK_H
#define REALMARK_H

#include "real_estate.h"

class realmark : public real_estate
{
public:
	realmark(QString url, QWidget *apParent = NULL);

protected:
	void doWebLoadFinished();
	void parseData();
	void getProfileList();
	void getOfficeContact();
};

#endif // REALMARK_H
