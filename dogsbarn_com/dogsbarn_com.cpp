#include "dogsbarn_com.h"

/*
 * CSS reference
 * 		http://www.w3schools.com/cssref/css_selectors.asp
 */

#define BASEURL "http://dogsbarn.com/dog-rescue-centres/"

dogsbarn_com::dogsbarn_com(QWidget *apParent)
	: BaseWebEngine(apParent) {
// for using custom control, call this function then implement initExtWidget()
	initCustomControl();

	setWindowTitle(QLatin1String("dogsbarn_com"));
	setBaseURL(BASEURL);

	if(mpDBLogger != NULL)
		mpDBLogger->setLogPath(QLatin1String("dogsbarn_com.sql"));

	mpXMLLogger = new XMLLogger(mpLog, QLatin1String("dogsbarn_com.xml"));

	mpLog->setFilePath(QLatin1String("dogsbarn_com.txt"));
///////////////////////////////////////////////////////////
	QString homeDir(::getenv("HOME"));
	mpDBManager = new DBManager(homeDir + "/tmp/dogsbarn_com.db");
	qDebug() << QLatin1String("Open local DB:") << mpDBManager->open()
			 << QLatin1String("--> path: ") << mpDBManager->getDBPath();
	qDebug() << QLatin1String("Create temp table:") << mpDBManager->createTmpTable();
}

enum enuDatabase {
	eName = 0,
	eAdd, eTel, eEmail,
	eTwitter, eFacebook,
	eDescription,
	eDatabaseMax
};

void dogsbarn_com::initDatabase() {
	mDbLstColName << QLatin1String("Name")
				  << QLatin1String("Address") << QLatin1String("Tel") << QLatin1String("Email")
				  << QLatin1String("Twitter") << QLatin1String("Facebook")
				  << QLatin1String("Description")
					 ;
	mDbLstKey << eName << eAdd << eTel << eEmail;
	mDbTableName = QLatin1String("dogsbarn_com");

	mpDBManager->createDataTable(mDbTableName,
								 mDbLstColName,
								 mDbLstKey);
}

void dogsbarn_com::doStart() {
	meState = E_STATE_INIT;
	loadPage(BASEURL);
}

void dogsbarn_com::doWebLoadFinished() {
	switch(meState)
	{
	case E_STATE_INIT:
		getAllRescueCenter();
		break;

//	case E_STATE_GET_MAIN_CATEGORY:
//		qDebug() << "E_STATE_GET_MAIN_CATEGORY";
//		break;

//	case E_STATE_GET_SUB_CATEGORY:
//		qDebug() << "E_STATE_GET_SUB_CATEGORY";
//		break;

//	case E_STATE_GET_DETAIL_INFO:
//		qDebug() << "E_STATE_GET_DETAIL_INFO";
//		break;

	default:
		qDebug() << "default";
		break;
	}
}

////////////////////////////////////////////////////////////////////////

void dogsbarn_com::getAllRescueCenter() {
	QWebElementCollection eCol = mpWebView->findAll(QLatin1String("div.tve_cb_cnt.tve_empty_dropzone.tve_clearfix"));
	QWebElement element;
	DataEntry de(mDbLstColName);

	QWebElementCollection eDetails;
	QWebElement dElement;
	QStringList details;
	QString tmp;
	QString value;

	foreach(element, eCol) {
		de.clearData();

		dElement = element.findFirst(QLatin1String("div.thrv_wrapper.thrv-columns"));
		updateData(&de, eName, QLatin1String("div.thrv_wrapper.thrv_text_element.tve_empty_dropzone"), &dElement);

		details.clear();
		eDetails = dElement.nextSibling()
				   .nextSibling()
				   .findAll(QLatin1String("p"));
		foreach(dElement, eDetails)
			details << dElement.toPlainText();

		if (details.count() > 1) {
			tmp = details.takeLast();
			de.updatePropertyString(eDescription, tmp);
		}

		foreach (tmp, details) {
			if (tmp.startsWith(QLatin1String("Address:"))) {
				tmp = tmp.replace(QLatin1String("Address:"), QLatin1String("")).trimmed();
				de.updatePropertyString(eAdd, tmp);
				continue;
			}

			if (tmp.startsWith(QLatin1String("Tel:"))) {
				tmp = tmp.replace(QLatin1String("Tel:"), QLatin1String("")).trimmed();
				de.updatePropertyString(eTel, tmp);
				continue;
			}

			if (tmp.startsWith(QLatin1String("Email:"))) {
				tmp = tmp.replace(QLatin1String("Email:"), QLatin1String("")).trimmed();
				de.updatePropertyString(eEmail, tmp);
				continue;
			}

			if (tmp.startsWith(QLatin1String("Twitter:"))) {
				tmp = tmp.replace(QLatin1String("Twitter:"), QLatin1String("")).trimmed();
				de.updatePropertyString(eTwitter, tmp);
				continue;
			}

			if (tmp.startsWith(QLatin1String("Facebook:"))) {
				tmp = tmp.replace(QLatin1String("Facebook:"), QLatin1String("")).trimmed();
				de.updatePropertyString(eFacebook, tmp);
				continue;
			}
		}

		mpDBManager->insertData(&de, mDbTableName);
	}

	bool found = false;
	eCol = mpWebView->findAll(QLatin1String("div.thrv_wrapper.tve_image_caption.alignright span.tve_image_frame a"));
	foreach (element, eCol) {
		tmp = mpWebView->getHrefURL(&element);
		if (tmp.indexOf(QLatin1String("?page_id=2970")) > 0 ||
				tmp.indexOf(QLatin1String("?page_id=2984") > 0)) {
			mpWebView->doClick(&element);
			found = true;
			break;
		}
	}

	if (!found)
		showFinishMsg(QLatin1String("Finish extracting data"));
}

////////////////////////////////////////////////////////////////////////

WebEngine_Main(dogsbarn_com)
