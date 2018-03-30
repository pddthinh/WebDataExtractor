#ifndef BIGGINSCOTT_H
#define BIGGINSCOTT_H

#include "real_estate.h"

class bigginscott : public real_estate
{
public:
	bigginscott(QString url, QWidget *apParent = 0);

protected:
	void doWebLoadFinished();
	bool gotoNextOffice();
	void collectProfile();
	void parseData();
	void collectOfficeContact();

private:
	QStringList mlstOffice;
};

#endif // BIGGINSCOTT_H
