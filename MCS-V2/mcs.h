#ifndef MCS_H
#define MCS_H

#include "WDEInclude.h"

class MCS : public BaseWebEngine
{
public:
	MCS(QString baseURL,
		QString saveLinkPath,
		QWidget *apParent = 0);

protected:
	void doStart();
	void doStop(){}
	void doWebLoadFinished();
	virtual void initDatabase() = 0;
	void initExtWidget();

protected:
	bool initSearchState1();
	bool initSearchState2();
	bool goNextResultPage();
	bool loadDataFromFile();
	void getListItem();
	virtual void extractSearchResult() = 0;
	virtual void getDetailData() = 0;

protected:
	QString mMCSBaseURL;
	QString mMCSLinkFile;
};

#endif // MCS_H
