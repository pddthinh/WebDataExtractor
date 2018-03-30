#include "homecheck_co_uk.h"
#include "cookie.h"

#define BASEURL "http://www.homecheck.co.uk/Welcome.do"
//#define BASEURL		"file:///home/pddthinh/working/paul/homecheck.co.uk/HomecheckWelcome.html"
//#define BASEURL		"file:///home/pddthinh/working/paul/homecheck.co.uk/HomecheckSummary.html"

#define PCLIST_SEP		QLatin1String("::")

homecheck_co_uk::homecheck_co_uk(QWidget *apParent)
	: BaseWebEngine(apParent) {
	// for using custom control, call this function then implement initExtWidget()
	initCustomControl();

	setWindowTitle("HomeCheck");
	setBaseURL(BASEURL);

	if(mpDBLogger != NULL)
		mpDBLogger->setLogPath("homecheck_co_uk.sql");

	mpLog->setFilePath("homecheck_co_uk.txt");
	///////////////////////////////////////////////////////////

	// init the cookie
	mpWebView->page()->networkAccessManager()->setCookieJar(new Cookie());
}

enum enuDatabase {
	ePCReg = 0, ePCFull,
	ePro1, eP1Rate, eP1Full,
	ePro2, eP2Rate, eP2Full,
	ePro3, eP3Rate, eP3Full,
	ePro4, eP4Rate, eP4Full,
	ePro5, eP5Rate, eP5Full,
	ePro6, eP6Rate, eP6Full,
	ePro7, eP7Rate, eP7Full,

//	eNameF, eFRate, eFFull,
//	eNameS, eSRate, eSFull,
//	eNameR, eRRate, eRFull,
//	eNameCM, eCMRate, eCMFull,
//	eNameLF, eLFRate, eLFFull,
//	eNameHLU, eHLURate, eHLUFull,
//	eNameP, ePRate, ePFull,

	eDatabaseMax
};

void homecheck_co_uk::initDatabase() {
	mDbLstColName << "PCRegion" << "Postcode"
				  << "Pro1" << "Pro1Rate" << "Pro1Detail"
				  << "Pro2" << "Pro2Rate" << "Pro2Detail"
				  << "Pro3" << "Pro3Rate" << "Pro3Detail"
				  << "Pro4" << "Pro4Rate" << "Pro4Detail"
				  << "Pro5" << "Pro5Rate" << "Pro5Detail"
				  << "Pro6" << "Pro6Rate" << "Pro6Detail"
				  << "Pro7" << "Pro7Rate" << "Pro7Detail"

//				  << "Flood" << "FloodRate" << "FloodDetail"
//				  << "Subsidence" << "SubsidenceRate" << "SubsidenceDetail"
//				  << "Radon" << "RadonRate" << "RadonDetail"
//				  << "CoalMining" << "CoalMiningRate" << "CoalMiningDetail"
//				  << "Landfill" << "LandfillRate" << "LandfillDetail"
//				  << "HistoricalLandUse" << "HistoricalLandUseRate" << "HistoricalLandUseDetail"
//				  << "Pollution" << "PollutionRate" << "PollutionDetail"
					 ;
	mDbLstKey << ePCReg << ePCFull;
	mDbTableName = "homecheck_co_uk";
}

void homecheck_co_uk::doStart() {
	do {
		if (loadRegionCodeData() == false) {
			showFinishMsg(QLatin1String("Failed to load region code data!"));
			break;
		}

		mRCode.clear();
		mPCodeList.clear();

		meState = E_STATE_INIT;
		loadPage(BASEURL);
	} while (0);
}

void homecheck_co_uk::doWebLoadFinished() {
	switch(meState) {
	case E_STATE_INIT:
		doSearchPostcode();
		break;

	case E_STATE_GET_MAIN_CATEGORY:
		getSearchPCResult();
		break;

	default:
		qDebug() << "default";
		break;
	}
}

/**
 * @brief homecheck_co_uk::loadRegionCodeData
 *		Load region code data list from csv file (created by UKPostcodeHelper)
 * @return
 */
bool homecheck_co_uk::loadRegionCodeData() {
	bool blRet = false;

	do {
		mRCMap.clear();

		QString path = mpExtFileLoading->getLoadPath();
		if (path.length() == 0) {
			blRet = true;
			break;
		}

		QFile csv(path);
		if (csv.open(QFile::Text | QFile::ReadOnly) == false) {
			qDebug() << QLatin1String("Failed to open CSV file!");
			break;
		}

		QTextStream stream(&csv);
		int count = 0;

		int ignore = mpExtFileLoading->getNumberOfSkipItem();
		if (ignore > 0) {
			qDebug() << QLatin1String("Skipped item: ") << ignore;
			// ignore the title
			ignore++;
		}
		else
			ignore = 1;

		while (count < ignore && stream.atEnd() == false) {
			stream.readLine();
			count++;
		}

		QString line;
		QString rcode;
		QStringList pcList;
		QStringList segment;
		while (stream.atEnd() == false) {
			line = stream.readLine();
			if (line.length() == 0)
				continue;

			// parse the line
			segment = line.split(QLatin1String(","));
			if (segment.length() != 2) {
				qDebug() << QLatin1String("Invalid line: ") << line;
				continue;
			}

			rcode = segment.at(0);
			pcList = segment.at(1).trimmed().split(PCLIST_SEP);

			if (rcode.length() == 0 || pcList.length() == 0)
				continue;

			mRCMap[rcode] = pcList;
		}

		qDebug() << QLatin1String("Loaded item: ") << mRCMap.count();

		csv.close();

		blRet = true;
	} while (0);

	return blRet;
}

QString homecheck_co_uk::gotoNextPCode(bool blNextRC) {
	QString nextPC;

	do {
		// first time or force next region code?
		if ((mRCode.length() == 0 && mPCodeList.length() == 0) ||
				blNextRC == true) {
			// clear all previous data
			mRCode.clear();
			mPCodeList.clear();

			if (mRCMap.isEmpty() == true)
				break;

			mRCode = mRCMap.firstKey();
			mPCodeList = mRCMap.take(mRCode);
		}

		if (mPCodeList.isEmpty() == true) {
			// Enter here mean can not get any data of the current region/postcodes
			DEF_LOG(QString("FAILED getting data: %1").arg(mRCode));

			nextPC = gotoNextPCode(true);
			break;
		}

		nextPC = mPCodeList.takeFirst();
	} while (0);

	return nextPC;
}

void homecheck_co_uk::doSearchPostcode() {
	QWebElement elem;
	QWebElement eForm;

	do {
		mPCode = gotoNextPCode();
		setStatus(QString("Remain region: %1").arg(mRCMap.count()));

		if (mPCode.isEmpty() == true) {
			qDebug() << QLatin1String("Can not go to next postcode!");
			showFinishMsg(QLatin1String("Finished all postcodes!"));
			break;
		}
		DEF_LOG(QString("Processing rcode [%1] - [%2]").arg(mRCode).arg(mPCode));

		// postcode Form
		eForm = mpWebView->findFirstElement(QLatin1String("form"),
											QLatin1String("name"),
											QLatin1String("postcodeForm"));
		DEF_CHECK_BREAK(eForm, QLatin1String("Can not find <postcodeForm> element!"));

		// postcode
		elem = mpWebView->findFirstElement(QLatin1String("input"),
										   QLatin1String("name"),
										   QLatin1String("postcode"), &eForm);
		DEF_CHECK_BREAK(elem, QLatin1String("Can not find the <postcode> element!"));
		elem.setAttribute(QLatin1String("value"), mPCode);

		// search
		elem = mpWebView->findFirstElement(QLatin1String("input"),
										   QLatin1String("type"),
										   QLatin1String("submit"), &eForm);
		DEF_CHECK_BREAK(elem, QLatin1String("Can not find the <submit> (Search button) element!"));

		meState = E_STATE_GET_MAIN_CATEGORY;
		mpWebView->doClick(&elem);
	} while(0);
}

bool homecheck_co_uk::checkConfirm() {
	bool blRet = false;

	do {
		QWebElement eMain;
		QWebElement element;

		eMain = mpWebView->getElementById(QLatin1String("Main"));
		DEF_CHECK_BREAK(eMain, QLatin1String("Can not find Main element!"));

		element = mpWebView->findFirstElement(QLatin1String("form"),
											  QLatin1String("name"),
											  QLatin1String("postcodeForm"), &eMain);
		if (element.isNull() == true)
			break;

		// click on button [I Agree]
		QWebElement eBtn = mpWebView->findFirstElement(QLatin1String("input"),
													   QLatin1String("type"),
													   QLatin1String("submit"), &element);
		QString btnText = eBtn.attribute(QLatin1String("value"));
		if (btnText == QLatin1String("I Agree")) {
			mpWebView->doClick(&eBtn);
			blRet = true;
		}
	} while(0);

	return blRet;
}

void homecheck_co_uk::getSearchPCResult() {
	QWebElement eForm;

	do {
		// check confirm
		if (checkConfirm())
			break;

		// check for the postcode error
		eForm = mpWebView->findFirstElement(QLatin1String("form"),
											QLatin1String("name"),
											QLatin1String("postcodeForm"));
		if (eForm.isNull() == true) {
			// no error, continue parsing result
			parseSearchResult();

			//NOTE: this force going to next region code
			mRCode.clear();
			mPCodeList.clear();

			// continue fall down for searching next postcode
			break;
		}

		doSearchPostcode();
		qDebug() << QLatin1String("Searching next postcode");
	} while (0);
}

void homecheck_co_uk::parseSearchResult() {
	QWebElement elem;
	QWebElement eTable, eTD;
	QWebElementCollection eTables, eTDs, eCol;
	DataEntry data(mDbLstColName);
	QString strTmp;
	int proIdx;
	bool blErr = true;

	// clear all un-used text
	eCol = mpWebView->findAll(QLatin1String("*.copyright, *.notcommercial"));
	foreach (elem, eCol)
		elem.setPlainText(QLatin1String(""));

	// postcode, rcode
	data.updateProperty(mDbLstColName[ePCFull], mPCode);
	data.updateProperty(mDbLstColName[ePCReg], mRCode);

	// select all properties container
	eTables = mpWebView->findAll(QLatin1String("td[class=\"LeftDotted\"] > table"));

	// loop for all properties
	proIdx = ePro1;
	foreach (eTable, eTables) {
		if (proIdx >= eDatabaseMax)
			break;

		elem = mpWebView->findFirstElement(QLatin1String("td"),
										   QLatin1String("class"),
										   QLatin1String("coloured"), &eTable);
		if (elem.isNull() == true) {
			// skip all of this property
			proIdx += 3;
			continue;
		}

		// property name
		updateData(&data, proIdx, &elem);
		proIdx++;

		// property rating
		elem = mpWebView->findFirstElement(QLatin1String("td"),
										   QLatin1String("class"),
										   QLatin1String("hazard"), &eTable);
		eTDs = elem.findAll(QLatin1String("td"));
		foreach (eTD, eTDs) {
			if (eTD.attribute(QLatin1String("class")) == QLatin1String("hazardno"))
				continue;

			strTmp = eTD.toPlainText();
			break;
		}
		if (strTmp.isEmpty() == false)
			data.updateProperty(mDbLstColName[proIdx], strTmp);
		proIdx++;

		// property detail
		elem = elem.nextSibling();
		strTmp = elem.toPlainText().replace(QLatin1String("\n\n"), QLatin1String("\n"))
				.replace(QLatin1String(". "), QLatin1String("."))
				.replace(QRegExp("If you would like to purchase(.+)$"), QLatin1String(""))
				.replace(QRegExp("The level of concern has been(.+)\\n"), QLatin1String(""));
		if (strTmp.startsWith("\n") == true)
			strTmp = strTmp.remove(0, 1);
		if (strTmp.endsWith("\n") == true)
			strTmp.chop(1);
		strTmp.trimmed();
		//		qDebug() << strTmp;
		data.updateProperty(mDbLstColName[proIdx], strTmp, false);

		proIdx ++;
		blErr = false;
	} // properties

	if (blErr == false) {
		mlstData << data;
	}

	if (mlstData.count() >= 1)
		logDataList();

	doWait(1, 1);
}

void homecheck_co_uk::doWaitFinished() {
	qDebug() << QLatin1String("Run delay timeout ...");

	//TODO: goto next postcode
	meState = E_STATE_INIT;
	loadPage(BASEURL);
}

WebEngine_Main(homecheck_co_uk)
