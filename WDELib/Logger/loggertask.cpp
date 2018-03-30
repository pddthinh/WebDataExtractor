#include "loggertask.h"
//#include "../WDEDef.h"

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#include <QMutexLocker>

using namespace nsLogger;

LoggerTask::LoggerTask(QObject *parent, const QString &astrDBLogPath, Logger *apLogger)
	: QObject(parent)
{
	if(astrDBLogPath.isEmpty() == true)
		mpDBLogger = new Logger(DEF_STR_DEFAULT_DATA_LOG_NAME);
	else
		mpDBLogger = new Logger(astrDBLogPath);

	mpLog = apLogger;

	mpMutex = new QMutex(QMutex::Recursive);
	mplstData = new DATA_LIST();

	mblStopFlag = false;
	mblSaving = false;
	mblWaitForStop = false;
}

void LoggerTask::addData(const DATA_LIST &aList)
{
	QMutexLocker locker(mpMutex);

	mplstData->append(aList);
	qDebug() << "LoggerTask::addData, data count: " << mplstData->count();
}

void LoggerTask::sltDBSaveData(const QString &astrData/*Table name*/)
{
	Q_UNUSED(astrData)
	qDebug() << "sltSaveData, start ..., Thread id: " << QThread::currentThreadId();

//    while(mblStopFlag == false &&
//          !mlstData.isEmpty())
//    {
//        mpMutex->lock();
//        mblSaving = true;

//        DEF_LOG_DATA_LIST(mlstData, astrData);
//        mlstData.clear();

//        mpMutex->unlock();

//#ifdef Q_WINS
//        Sleep(80);
//#endif
//    }

//    if(mblStopFlag)
//        DEF_LOG(QString("%1 items un-saved!").arg(mlstData.size()));

//    mblSaving = false;

//    if(mblWaitForStop)
//        this->thread()->quit();

////	qDebug() << "sltSaveData, stop!";

	//qDebug() << "sltSaveData, start ..., Thread id: " << QThread::currentThreadId();
//    mpMutex->lock();
	QMutexLocker locker(mpMutex);
	mblSaving = true;

//    DEF_LOG_DATA_LIST(mlstData, astrData);
//    mlstData.clear();

//    mpMutex->unlock();

	qDebug() << "save finished...";

//    if(mblStopFlag)
//        DEF_LOG(QString("%1 items un-saved!").arg(mlstData.size()));

	mblSaving = false;

	if(mblWaitForStop)
		this->thread()->quit();
	qDebug() << "sltSaveData, stop!";
}

void LoggerTask::sltDBLogString(const QString &astrData)
{
	qDebug() << "sltLogString --> enter";
	QMutexLocker locker(mpMutex);

	mpDBLogger->logString(astrData);

	qDebug() << "sltLogString --> exit";
}

//void LoggerTask::sltSaveDataItem(const DataEntry &aData)
//{
//	qDebug() << "sltSaveDataItem --> enter";
//	QMutexLocker locker(mpMutex);

////	DEF_LOG(astrData);

//	qDebug() << "sltSaveDataItem --> exit";
//}

//void LoggerTask::sltLogDataList(const QString &astrTableName, const DATA_LIST &aLstData)
//{
//	qDebug() << "sltLogDataList ";
//}
