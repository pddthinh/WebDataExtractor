#ifndef REIWA_H
#define REIWA_H

#include "real_estate.h"

class reiwa : public real_estate
{
public:
	reiwa(QString url);

protected:
	void doStart();
	void initExtWidget()
	{
		mpExtFileLoading = new ExtFileLoading(this);
	}
	void doWebLoadFinished();
	void getSearchLink();
	void getSearchResult();

	void searchOffice();
	void getOfficeDetail();
	void parseOfficeData();

	void extractData(bool ablSaveName = false);

	void loadOfficeName(QString astrPath);

private:
	QStringList		mlstOfficeName;
	QString			mstrOffice;
};

#endif // REIWA_H
