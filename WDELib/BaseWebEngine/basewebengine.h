#ifndef BASEWEBENGINE_H
#define BASEWEBENGINE_H

#include <QApplication>
#include <QDialog>
#include <QMessageBox>
#include <QGridLayout>
// For SSL
#include <QList>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QSslConfiguration>
#include <QtNetwork/QSslCertificate>

#include "WDEDef.h"
#include "webengine.h"
#include "Logger/logger.h"
//#include "Logger/loggertask.h"
#include "Database/sqlquerybuilder.h"
#include "Extension/extfileloading.h"

#include "Database/dbmanager.h"

#ifdef USE_DATABASE
#include "Database/mysqlhandler.h"
#endif

#include <QLineEdit>

class QPushButton;
class QUrl;
class QTimer;
class QStringList;
class QCloseEvent;
class QSettings;

using namespace nsLogger;
using namespace nsWDEDef;
using namespace nsDatabase;

class DBLogger;
class XMLLogger;

namespace nsBaseWebEngine
{

typedef enum E_STATE
{
	E_STATE_NONE = 0,
	E_STATE_INIT,
	E_STATE_GET_MAIN_CATEGORY,
	E_STATE_GET_SUB_CATEGORY,
	E_STATE_GET_DETAIL_INFO,
	E_STATE_SUB1,
	E_STATE_SUB2,
	E_STATE_SUB3,
	E_STATE_MAX
} ENU_STATE;

class WDE_LIB_EXPORT BaseWebEngine : public QDialog
{
	Q_OBJECT

public:
	explicit BaseWebEngine(QWidget *parent = 0);
	virtual ~BaseWebEngine() {
		if (mpDBManager != NULL) mpDBManager->close();
	}
	void loadPage(QString astrURL = "");
//	void setLoggerTask(LoggerTask *apTask);
	void setSSLCertificate(QString cerPath, QString cerPassword);

public:
	// Pure virtual functions
	virtual void doStart() = 0;
	virtual void doStop() = 0;
	virtual void doWebLoadFinished() = 0;
	virtual void initDatabase() = 0;
	virtual void initExtWidget() = 0;

signals:
	void sigDBSaveData(const QString &astrData);
	void sigDBLogString(const QString &astrData);

private Q_SLOTS:
	void sltAboutToQuit();

// Buttons slots
	void sltBtnStartClick();
	void sltBtnStopClick();

// Web slots
	void sltWebStatusChanged(const QString &astrStatus);
	void sltWebURLChanged(const QUrl &aURL);
	void sltWebLoadProgress(int aiProgress);
	void sltWebLoadFinished(bool ablStatus);
	void sltWebLoadStated();

	void sltWebLinkHover(const QString &astrLink, const QString &astrTitle, const QString &astrTextContent);

// Timer slots
	void sltTimerForceStopLoad();
	void sltTimerPauseResum();
	void sltRunDelayTimeout();

// Properties
public:
	void setWebLoadTimeout(int aiTimeout = DEF_INT_TIMER_STOP_LOAD);
	inline QString getBaseURL() const { return mstrBaseURL; }
	inline void setBaseURL(const QString &astrURL) {
		mstrBaseURL = astrURL;
		updateBaseRootURL(mstrBaseURL);

		setURL(mstrBaseURL);
	}

	inline QString getURL() const {
		if(mpURL)
			return updateURL(mpURL->text());
		return "";
	}

	inline void setURL(const QString &astrURL) const {
		if(mpURL)
			mpURL->setText(astrURL);
	}

	inline QString getStatus() const {
		if(mpStatus)
			return mpStatus->text();
		return "";
	}

	/**
	 * Set processing status message
	 *
	 * @param astrStatus Status message
	 */
	inline void setStatus(const QString &astrStatus) const {
		if(mpStatus)
			mpStatus->setText(astrStatus);
	}

//	/**
//	 * Make a shot delay
//	 *
//	 * @param ms
//	 */
//	inline void delay(quint64 ms) const {
//		quint64 timeQuit = QDateTime::currentMSecsSinceEpoch() + ms;
//		while (timeQuit > QDateTime::currentMSecsSinceEpoch())
//			QApplication::processEvents(QEventLoop::AllEvents, 100);
//	}

protected:
	/**
	 * @brief doWait
	 *		Start waiting timer
	 * @param rtLow
	 * @param rtHight
	 */
	void doWait(int rtLow = 5, int rtHight = 10);
	virtual void doWaitFinished() {}

	void initCustomControl();
	QString updateURL(QString astrURL) const;
	bool gotoNextDataLink(ENU_STATE aeNextState = E_STATE_GET_DETAIL_INFO);
	bool gotoNextCategoryLink(ENU_STATE aeNextState = E_STATE_GET_SUB_CATEGORY);
	bool updateBaseRootURL(const QString & astrURL);
	void logDataList();
	bool updateData(DataEntry *apData, int aiColIdx, QWebElement *apElement, QString property = "");
	bool updateData(DataEntry *apData, int aiColIdx, const QString& selector, QWebElement *parent = 0,
					QString property = "");
	//	bool updateData(DataEntry *apData, int aiColIdx, QWebElement *apElement);
	//	bool updateData(DataEntry *apData, int aiColIdx, const QString& selector);
	void saveData(DATA_LIST *apData = NULL, bool ablCheckEmpty = false);
	void saveXMLData(DATA_LIST *pData);

	void loadSettings();
	void saveSettings();

public:
	bool loadJQuery(QString astrPath = "");
	void showFinishMsg(QString astrMsg = "");

private:
	void initLocalMembers();
	void initBaseControl();
	void init();
	void createDBLog();
	void setEnableCtrlWidget(bool ablEnable = true);

protected:
	bool			mblLastLoadStatus;
	QTimer			*mpStopTimer;
	WebEngine		*mpWebView;
	ENU_STATE		meState;

	QLineEdit		*mpURL;
	QLineEdit		*mpStatus;
	QLineEdit       *mpWebProgress;
	QList<QWidget*>	mlstCtrlWidget;
// Support JQuery
	bool            mblApplyJQuery;
	QString         mstrJQuery;

// Save URL link list
	QStringList		mlstCategoryLink;
	QStringList		mlstDataLink;

// Logger
	Logger			*mpLog;
//	LoggerTask		*mpLoggerTask;

// For database log
	QString			mDbTableName;
	QStringList		mDbLstColName;
	INT_LIST		mDbLstKey;
	DATA_LIST       mlstData;
	DBLogger		*mpDBLogger;
	DataEntry		mData;

// For XML log
	XMLLogger		*mpXMLLogger;

// For Extension Control
	ExtFileLoading	*mpExtFileLoading;

	QString			mstrBaseRootURL;

// Timer
	QTimer			*mpPauseTimer;
	QTimer			*mpRunDelayTimer; // timer to slow down the running speed
	int				miRunDelayLow, miRunDelayHight;

// App settings
	QSettings		*mpAppPref;
	DBManager		*mpDBManager;

private:
	QString			mstrProgressFrmt;
	QString			mstrRemainItemFrmt;
	QString			mstrSaveDataMsgFrmt;
	QString			mstrBaseURL;
	int				miWebLoadTimeOut;

	// For SSL
	QString			mstrSSLCertificatePath;
	QString			mstrSSLCertificatePassword;
	QSslConfiguration		*mpSslConfig;

#ifdef USE_DATABASE
/*
 * Support MySQL
 */
protected:
	MySQLHandler	*mpMySQLHandler;
	QString			mDBName;
	QString			mDBHost;
	QString			mDBUsername;
	QString			mDBPassword;

//public:
//	virtual void initMySQL(){}
#endif // USE_DATABASE
};

#define WebEngine_Main(WebObject) \
int main(int argc, char *argv[]) \
{ \
	QApplication app(argc, argv); \
	WebObject web; \
	web.showNormal(); \
	return app.exec(); \
}

}// namespace nsBaseWebEngine

#endif // BASEWEBENGINE_H
