#ifndef KEVINWRIGHTRE_H
#define KEVINWRIGHTRE_H

#include "real_estate.h"

class kevinwrightre : public real_estate
{
public:
	kevinwrightre(QString url);

protected:
	void doWebLoadFinished();
	void parseData();
};

#endif // KEVINWRIGHTRE_H
