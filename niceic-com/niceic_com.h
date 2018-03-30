#ifndef __NICEIC_H__
#define __NICEIC_H__

#include "WDEInclude.h"

#include <QDebug>
#include <QWebElement>
#include <QWebElementCollection>
#include <QTimer>

/* Max wait time: 2m */
#define AJAX_MAX_TRY				40
#define AJAX_MONITOR_INTERVAL		3000	// 3s

class Niceic : public BaseWebEngine
{
	Q_OBJECT

public:
	Niceic(QWidget *apParent = 0);

protected:
	void doStart();
	void doStop(){}
	void doWebLoadFinished();
	void initDatabase();
	void initExtWidget();

/* Internal Methods */
private:
	void initSearching();
	void prepareStarting();
	bool gotoNextPostcode();
	void getDetailData();

	void extract1Row(QWebElement *apElement, DataEntry *apData);
	bool requestDetailEx();
	void extractDetailEx();

	bool requestNextPage();

/* AJAX monitor */
private:
	void initAJAXMonitor();
	void startMonitorAJAX(const QString &astrElementId);
	void stopMonitorAJAX();
public slots:
	void sltAJAXTimeout();

private:
	QStringList	mlstPostcode;
	QString		mstrPostcode;
	QTimer		*mpAJAXMonitor;
	bool		mblTimeout;
	QString		mstrMonitorElementId;
	int			miNumTry;
	int			miCurrentEntry;
	int			miCurrentPage;
	DATA_LIST	mLogDataList;
	QStringList	mlstDBKey;
};
#endif // __NICEIC_H__
