#ifndef MCSPRODUCT_H
#define MCSPRODUCT_H

#include "WDEInclude.h"

class MCSProduct : public BaseWebEngine
{
public:
	MCSProduct(QWidget *apParent = 0);

protected:
	void doStart();
	void doStop(){}
	void doWebLoadFinished();
	void initDatabase();
	void initExtWidget(){ mpExtFileLoading = new ExtFileLoading(this); }

protected:
	bool initSearchState1();
	bool initSearchState2();
//	void initSearch();
	void getProductListInfo();
	bool parseData(const QWebElement &aTR, DataEntry *apData);
//	QString parseProKey(QString astrURL);
	bool startExtractDetail();
	void extractSearchResult();
	void getDetailData();

private:
	bool loadSavedData(QString path);
	void startFromSavedData();
	bool gotoNextSavedEntry();
	void debugDataList();

private:
	DATA_LIST	mSavedDataList;
	DATA_LIST	mFinalDataList;
	DataEntry	mCurrentEntry;
//	XMLUtil		*mpXMLUtil;
};

#endif // MCSPRODUCT_H
