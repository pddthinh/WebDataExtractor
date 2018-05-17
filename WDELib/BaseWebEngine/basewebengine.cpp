#include "basewebengine.h"
#include "../WDEDef.h"

#include <QDebug>

#include <QWebView>
#include <QPushButton>

#include <QTimer>
#include <QMessageBox>
#include <QFile>

#include <QDesktopWidget>
#include <QtNetwork/QNetworkAccessManager>

#include <QSettings>
#include <QCoreApplication>
#include <QDir>

#include "Logger/dblogger.h"
#include "Logger/xmllogger.h"

#define WDE_SETTINGS_NAME					QLatin1String("wde.ini")
#define WDE_SETTINGS_GRP_WND_SIZE			QLatin1String("window-size")
#define WDE_SETTINGS_KEY_WND_SIZE			QLatin1String("size")
#define WDE_SETTINGS_KEY_WND_POS			QLatin1String("pos")

using namespace nsBaseWebEngine;
using namespace nsWDEDef;

BaseWebEngine::BaseWebEngine(QWidget *parent)
	: QDialog(parent)
{
	initLocalMembers();
	initBaseControl();
	init();

	loadSettings();
}

void BaseWebEngine::initLocalMembers() {
	mpLog = NULL;
	//	mpLoggerTask = NULL;
	mpStatus = NULL;
	mpStopTimer = NULL;
	mpURL = NULL;
	mpWebView = NULL;

	mstrJQuery = "";
	mblApplyJQuery = false;

	mDbTableName = "";
	mDbLstColName.clear();
	mDbLstKey.clear();
	mpDBLogger = NULL;

	mstrProgressFrmt = QLatin1String("Loading --- %1%");
	mstrRemainItemFrmt = QLatin1String("Remain items: %1");
	mstrSaveDataMsgFrmt = QLatin1String("Save %1 data entry");

	mpRunDelayTimer = NULL;
}

void BaseWebEngine::initBaseControl() {
	QGridLayout		*lpLayout = new QGridLayout(this);
	QPushButton		*lpBtnStart, *lpBtnStop;

	mpWebView = new WebEngine(this);
	lpBtnStart = new QPushButton(QLatin1String("Start"), this);
	lpBtnStop = new QPushButton(QLatin1String("Stop"), this);
	mpURL = new QLineEdit(this);
	mpStatus = new QLineEdit(this);
	mpStatus->setEnabled(false);
	mpWebProgress = new QLineEdit(this);
	mpWebProgress->setEnabled(false);

	// Add to layout
	lpLayout->addWidget(mpURL, 0, 0);
	lpLayout->addWidget(mpWebView, 1, 0);
	lpLayout->addWidget(mpWebProgress, 2, 0);
	lpLayout->addWidget(mpStatus, 3, 0);
	lpLayout->addWidget(lpBtnStart, 0, 1);
	lpLayout->addWidget(lpBtnStop, 1, 1, 1, 1, Qt::AlignTop);

	connect(lpBtnStart, SIGNAL(clicked()), this, SLOT(sltBtnStartClick()));
	connect(lpBtnStop, SIGNAL(clicked()), this, SLOT(sltBtnStopClick()));
	connect(mpURL, SIGNAL(returnPressed()), this, SLOT(sltBtnStartClick()));

	connect(mpWebView, SIGNAL(statusBarMessage(QString)), this, SLOT(sltWebStatusChanged(QString)));
	connect(mpWebView, SIGNAL(loadProgress(int)), this, SLOT(sltWebLoadProgress(int)));
	connect(mpWebView, SIGNAL(loadFinished(bool)), this, SLOT(sltWebLoadFinished(bool)));
	connect(mpWebView, SIGNAL(urlChanged(QUrl)), this, SLOT(sltWebURLChanged(QUrl)));
	connect(mpWebView, SIGNAL(loadStarted()), this, SLOT(sltWebLoadStated()));

	// Page signals
	connect(mpWebView->page(), SIGNAL(linkHovered(QString,QString,QString)), this, SLOT(sltWebLinkHover(QString,QString,QString)));

	this->setLayout(lpLayout);

	connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(sltAboutToQuit()));
}

void BaseWebEngine::initCustomControl() {
	do {
		mpExtFileLoading = NULL;

		initExtWidget();

		if(mpExtFileLoading == NULL) break;

		layout()->addWidget(mpExtFileLoading);
		mlstCtrlWidget << mpExtFileLoading;
	} while(false);
}

void BaseWebEngine::init() {
	// Init url
	if(mstrBaseURL.isEmpty())
		mstrBaseURL = DEF_STR_EMPTY;

	mpURL->setText(mstrBaseURL);

	mpStopTimer = new QTimer(this);
	connect(mpStopTimer, SIGNAL(timeout()), this, SLOT(sltTimerForceStopLoad()));
	setWebLoadTimeout();

	setWindowTitle(DEF_STR_TITLE);

	mblApplyJQuery = false;
	// Init Logger
	mpLog = new Logger(DEF_STR_DEFAULT_LOGGER_NAME);

	mpDBLogger = new DBLogger(mpLog, DEF_STR_DEFAULT_DATA_LOG_NAME);

	mpPauseTimer = new QTimer(this);
	connect(mpPauseTimer, SIGNAL(timeout()), this, SLOT(sltTimerPauseResum()));
	mpPauseTimer->setInterval(1500);
	mpPauseTimer->setSingleShot(true);

	mpRunDelayTimer = new QTimer(this);
	connect(mpRunDelayTimer, SIGNAL(timeout()), this, SLOT(sltRunDelayTimeout()));

	// Initial settings
	mpAppPref = new QSettings(QDir(QCoreApplication::applicationDirPath()).filePath(WDE_SETTINGS_NAME),
							  QSettings::NativeFormat,
							  this);
	resize(792, 792);
	move(QApplication::desktop()->availableGeometry().center() - this->rect().center());
}

void BaseWebEngine::setEnableCtrlWidget(bool ablEnable) {
	foreach (QWidget *lpWidget, mlstCtrlWidget) {
		if(lpWidget == NULL)
			continue;
		lpWidget->setEnabled(ablEnable);
	}
}

void BaseWebEngine::setWebLoadTimeout(int aiTimeout) {
	if(mpStopTimer)
		mpStopTimer->setInterval(aiTimeout);
}

void BaseWebEngine::createDBLog() {
	// init child database
	initDatabase();

	SQLQueryBuilder lbuilder;
	QString lstrCreateTable = lbuilder.getSQLCreateTable(mDbTableName, mDbLstColName, mDbLstKey, true);

	if(mpDBLogger) {
		mpDBLogger->setTableName(mDbTableName);
		mpDBLogger->logString(lstrCreateTable);
	}

	mData.initColName(mDbLstColName);
}

void BaseWebEngine::logDataList() {
	mpDBLogger->logDataList(mlstData);
	DEF_LOG(mstrSaveDataMsgFrmt.arg(mlstData.count()));

	mlstData.clear();
}

void BaseWebEngine::sltTimerForceStopLoad() {
	qDebug() << QLatin1String("sltTimerForceStopLoad --- Timeout");

	// Fix bug emit enter sltWebLoadFinished 2 times
	disconnect(mpWebView, SIGNAL(loadFinished(bool)), this, SLOT(sltWebLoadFinished(bool)));

	mpWebView->stop();
	sltWebLoadFinished(false);

	connect(mpWebView, SIGNAL(loadFinished(bool)), this, SLOT(sltWebLoadFinished(bool)));
}

void BaseWebEngine::sltBtnStartClick() {
	mlstCategoryLink.clear();
	mlstDataLink.clear();

	createDBLog();

	// Load JQuery
	//	loadJQuery();

	setEnableCtrlWidget(false);

	if (mpExtFileLoading != NULL) {
		int webLoadTimeout = mpExtFileLoading->getWebTimeout();
		if (webLoadTimeout > 0)
			setWebLoadTimeout(webLoadTimeout);
	}

	doStart();
}

void BaseWebEngine::sltBtnStopClick() {
	mpStopTimer->stop();
	mpWebView->stop();

	setEnableCtrlWidget();

	doStop();
}

void BaseWebEngine::sltWebLoadStated() {
	//	qDebug() << "web load started signal";
	mpStopTimer->start();

	//	mpWebView->page()->setNetworkAccessManager(mpNetManager);
}

void BaseWebEngine::sltWebLoadFinished(bool ablStatus) {
	do {
		mpStopTimer->stop();

		if(ablStatus == false)
			setStatus(QLatin1String("Page loaded with error(s)"));
		else
			setStatus(QLatin1String("Page loaded successfull"));

		mblLastLoadStatus = ablStatus;

		//		if(mblApplyJQuery == false && ablStatus == true)
		//		{
		//			if(mstrJQuery.isEmpty() == false)
		//			{
		//				mpWebView->executeJS(mstrJQuery);
		//				DEF_LOG(QLatin1String("JQuery load successfull"));
		//			}
		//			else
		//				DEF_LOG(QLatin1String("Can not load JQuery file"));

		//			mblApplyJQuery = true;
		//		}

		if (mblLastLoadStatus) {
			doWebLoadFinished();
			break;
		}

		mpPauseTimer->start();
	} while(false);
}

void BaseWebEngine::sltWebStatusChanged(const QString &astrStatus) {
	mpWebProgress->setText(astrStatus);
}

void BaseWebEngine::sltWebLoadProgress(int aiProgress) {
	mpWebProgress->setText(mstrProgressFrmt.arg(aiProgress));
}

void BaseWebEngine::sltWebLinkHover(const QString &astrLink, const QString &astrTitle, const QString &astrTextContent) {
	Q_UNUSED(astrTextContent)
	Q_UNUSED(astrTitle)

	mpWebProgress->setText(astrLink);
}

void BaseWebEngine::sltWebURLChanged(const QUrl &aURL) {
	mpURL->setText(aURL.toString());
}

QString BaseWebEngine::updateURL(QString astrURL) const {
	do {
		if (astrURL.indexOf(QLatin1String("://")) > 0) break;

		QString lstrTmp = astrURL.toLower();
		if(!lstrTmp.startsWith(QLatin1String("http")))
			astrURL = QLatin1String("http://") + astrURL;
	} while (0);

	return astrURL;
}

void BaseWebEngine::loadPage(QString astrURL) {
	do {
		if(mpWebView == NULL) {
			qWarning("BaseWebEngine::loadPage --> WebView is NULL");
			break;
		}

		mpWebView->stop();

		if(astrURL == "")
			astrURL = getURL();
		else
			setURL(astrURL);

		QString strURL = updateURL(astrURL);
		mpWebView->setUrl(QUrl(strURL));
	} while(false);
}

bool BaseWebEngine::gotoNextDataLink(ENU_STATE aeNextState) {
	bool	lblRet = false;

	do {
		setStatus(mstrRemainItemFrmt.arg(mlstDataLink.count()));

		if(mlstDataLink.isEmpty()) {
			DEF_LOG(QLatin1String("gotoNextDataLink ---> Finished extract data"));
			break;
		}

		lblRet = true;
		meState = aeNextState;
		loadPage(mlstDataLink.takeFirst());
	} while(false);

	return lblRet;
}

bool BaseWebEngine::gotoNextCategoryLink(ENU_STATE aeNextState) {
	bool	lblRet = false;

	do {
		setStatus(mstrRemainItemFrmt.arg(mlstCategoryLink.count()));
		if(mlstCategoryLink.isEmpty()) {
			DEF_LOG(QLatin1String("gotoNextCategoryLink ---> Finished extract data"));
			break;
		}

		lblRet = true;
		meState = aeNextState;
		loadPage(mlstCategoryLink.takeFirst());
	} while(false);

	return lblRet;
}

bool BaseWebEngine::loadJQuery(QString astrPath) {
	bool    lblRet = false;
	QFile   lFile;

	do {
		if(astrPath.isEmpty())
			astrPath = QLatin1String(":/jquery.js");

		lFile.setFileName(astrPath);

		DEF_LOG(astrPath);

		if(!lFile.open(QIODevice::ReadOnly | QIODevice::Text))
			break;

		mstrJQuery = lFile.readAll();

		lFile.close();
		lblRet = true;
	} while(false);

	return lblRet;
}

//void BaseWebEngine::setLoggerTask(LoggerTask *apTask)
//{
//	mpLoggerTask = apTask;

//	if(mpLoggerTask != NULL)
//	{
//		if(mpLoggerTask->getLogger() == NULL)
//			mpLoggerTask->setLogger(mpLog);
//	}
//}

void BaseWebEngine::showFinishMsg(QString astrMsg) {
	meState = E_STATE_MAX;

	if(astrMsg.isEmpty())
		astrMsg = QLatin1String("Finished all data");

	DEF_LOG(astrMsg);

	QMessageBox lmsg;
	lmsg.setText(astrMsg);

	const QRect screen = QApplication::desktop()->screenGeometry();
	lmsg.adjustSize();
	lmsg.move(screen.center() - lmsg.rect().center());
	lmsg.exec();
}

bool BaseWebEngine::updateBaseRootURL(const QString &astrURL) {
	bool	lblRet = false;
	QStringList	llstTmp;

	do {
		llstTmp = astrURL.split(QLatin1String("/"));
		if(llstTmp.isEmpty()) break;

		if(llstTmp.count() >= 2)
			llstTmp.removeLast();
		mstrBaseRootURL.clear();

		foreach(QString lstrTmp, llstTmp)
			mstrBaseRootURL += lstrTmp + QLatin1String("/");

		lblRet = true;
	} while(false);

	return lblRet;
}

/**
 * Update data entry with the input element
 *
 * @param apData
 * @param aiColIdx
 * @param apElement
 * @param property
 * @return
 */
bool BaseWebEngine::updateData(DataEntry *apData, int aiColIdx, QWebElement *apElement, QString property) {
	bool lblRet = false;
	QString lstrTmp("");

	do {
		if (apData == NULL || aiColIdx < 0 || apElement == NULL) break;

		// get element value
		do {
			if (apElement->isNull()) break;

			if (property.length() == 0) {
				lstrTmp = apElement->toPlainText();
				break;
			}

			// get its property
			if (property.compare(QLatin1String("href"), Qt::CaseInsensitive) == 0
					&& apElement->tagName().compare(QLatin1String("a"), Qt::CaseInsensitive) == 0) {
				lstrTmp = mpWebView->getHrefURL(apElement);
				break;
			}

			lstrTmp = apElement->attribute(property);
		} while(false);

		apData->updatePropertyString(mDbLstColName[aiColIdx], lstrTmp);

		lblRet = true;
	} while (false);

	return lblRet;
}

/**
 * Update data entry with the input element
 *
 * @param apData
 * @param aiColIdx
 * @param selector
 * @param parent
 * @return
 */
bool BaseWebEngine::updateData(DataEntry *apData, int aiColIdx, const QString &selector,
							   QWebElement *parent, QString property) {
	QWebElement element;

	if (parent == NULL)
		element = mpWebView->findFirst(selector);
	else
		element = parent->findFirst(selector);

	return updateData(apData, aiColIdx, &element, property);
}

/**
 * @brief BaseWebEngine::saveData
 *		Save data list as SQL script format
 *
 * @param apData
 * @param ablCheckEmpty
 */
void BaseWebEngine::saveData(DATA_LIST *apData, bool ablCheckEmpty) {
	do {
		if (apData == NULL)
			apData = &mlstData;

		if (mpDBLogger == NULL) break;

		if (ablCheckEmpty == true &&
				apData->isEmpty() == true)
			break;

		mpDBLogger->logDataList(*apData);
		DEF_LOG(QString(QLatin1String("Saved: %1"))
				.arg(apData->count()));
		apData->clear();
	} while (false);
}

/**
 * @brief BaseWebEngine::saveXMLData
 *		Save data list into XML format
 *
 * @param pData
 */
void BaseWebEngine::saveXMLData(DATA_LIST *pData) {
	do {
		if (!pData) break;

		if (!mpXMLLogger) break;

		int iSaved = mpXMLLogger->logDataList(pData);
		DEF_LOG(QString(QLatin1String("Saved: %1")).arg(iSaved));
		pData->clear();
	} while (false);
}

void BaseWebEngine::setSSLCertificate(QString cerPath, QString cerPassword) {
	do {
		if (cerPath.isEmpty()) break;

		mpSslConfig = new QSslConfiguration();
		QList<QSslCertificate> certificates = QSslCertificate::fromPath(cerPath);
		foreach (QSslCertificate cer, certificates) {
			qDebug() << "cer, issue info: " << cer.issuerInfo(QSslCertificate::Organization);
		}

		mpSslConfig->setCaCertificates(certificates);

		mstrSSLCertificatePassword = cerPassword;
		mstrSSLCertificatePath = cerPath;
	} while (false);
}

void BaseWebEngine::sltTimerPauseResum() {
	mpPauseTimer->stop();

	doWebLoadFinished();
}

/**
 * @brief BaseWebEngine::doWait
 *		Make a little delay to slow down the running speed
 * @param rtLow		Min wait (s)
 * @param rtHight	Max wait (s)
 */
void BaseWebEngine::doWait(int rtLow, int rtHight) {
	do {
		if (mpRunDelayTimer == NULL) {
			qWarning() << QLatin1String("Running timer is not initialized!");
			break;
		}

		int time = Utilities::randInt(rtLow, rtHight) * 1000;

		qDebug() << QLatin1String("Wait time: ") << time;

		mpRunDelayTimer->setInterval(time);
		mpRunDelayTimer->start();
	} while (false);
}

void BaseWebEngine::sltRunDelayTimeout() {
	mpRunDelayTimer->stop();

	doWaitFinished();
}

void BaseWebEngine::loadSettings() {
	do {
		if (mpAppPref == NULL) break;

		{
			mpAppPref->beginGroup(WDE_SETTINGS_GRP_WND_SIZE);

			this->resize(mpAppPref->value(WDE_SETTINGS_KEY_WND_SIZE, this->size()).toSize());
			this->move(mpAppPref->value(WDE_SETTINGS_KEY_WND_POS, this->pos()).toPoint());

			mpAppPref->endGroup();
		}
	} while (false);
}

void BaseWebEngine::saveSettings() {
	do {
		if (mpAppPref == NULL) break;

		{
			mpAppPref->beginGroup(WDE_SETTINGS_GRP_WND_SIZE);

			mpAppPref->setValue(WDE_SETTINGS_KEY_WND_SIZE, size());
			mpAppPref->setValue(WDE_SETTINGS_KEY_WND_POS, pos());

			mpAppPref->endGroup();
		}

		// save child settings
		if (mpExtFileLoading != NULL)
			mpExtFileLoading->saveSettings();
	} while (false);
}

void BaseWebEngine::sltAboutToQuit() {
	saveSettings();
}
