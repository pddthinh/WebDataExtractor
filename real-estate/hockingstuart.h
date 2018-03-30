#ifndef HOCKINGSTUART_H
#define HOCKINGSTUART_H

#include "real_estate.h"

class hockingstuart : public real_estate
{
public:
	hockingstuart(QString url, QWidget *apParent = 0);

protected:
	void doWebLoadFinished();
	void parseData();
	void parseOfficeContact();

	void getOfficeLink();
	bool gotoNextOffice();

private:
	QStringList mlstOffice;
};

#endif // HOCKINGSTUART_H
