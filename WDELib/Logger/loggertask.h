#ifndef LOGGERTASK_H
#define LOGGERTASK_H

#include <QObject>
#include <QStringList>
#include <QMutex>
#include <QThread>
#include <QDebug>

//#include "WDE.h"
#include "WDEDef.h"
#include "logger.h"

using namespace nsWDEDef;

namespace nsLogger
{

class WDE_LIB_EXPORT DataWorkerThread : public QThread
{
protected:
	void run()
	{
		qDebug() << "DataWorkerThread running ..., id: " << currentThreadId();
		exec();
		qDebug() << "DataWorkerThread stop";
	}
};

class LoggerTask : public QObject
{
	Q_OBJECT
public:
	explicit LoggerTask(QObject *parent = 0, const QString &astrDBLogPath = "", Logger *apLogger = 0);
	void addData(const DATA_LIST &aList);

	inline void requestStop(){ mblStopFlag = true; }
	inline bool isSaving(){ return mblSaving; }
	inline void setWaitForStop(bool ablFlag){ mblWaitForStop = ablFlag; }

	void setLogger(Logger *apLogger){ mpLog = apLogger; }
	Logger *getLogger(){ return mpLog; }

public Q_SLOTS:
	void sltDBSaveData(const QString &astrTableName/*Table name*/);
	void sltDBLogString(const QString &astrData);
//	void sltSaveDataItem(const DataEntry &aData);
//	void sltLogDataList(const QString &astrTableName, const DATA_LIST &aLstData);

private:
	mutable QMutex	*mpMutex;
	Logger			*mpLog;
	Logger			*mpDBLogger;
	DATA_LIST       *mplstData;

	bool			mblStopFlag;		// Stop saving a group of items
	bool			mblSaving;			// Thread is saving a group of items
	bool			mblWaitForStop;		// Wait for saving all remain items and stop this thread
};

} // namespace nsLogger

#endif // LOGGERTASK_H
