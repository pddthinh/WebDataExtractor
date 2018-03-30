#ifndef STARRPARTNERS_H
#define STARRPARTNERS_H

#include "real_estate.h"

class starrpartners : public real_estate
{
public:
	starrpartners(QString url);

protected:
	void doWebLoadFinished();
	void parseData();
	void getAgentList();

	void getOfficeInfo();
	void getOfficeContact();
	bool gotoNextOffice();

private:
	QStringList mlstOffice;
};

#endif // STARRPARTNERS_H
