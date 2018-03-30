#include "find_a_vet.h"

/*
 * Search result can be executed with this URL
 *	http://findavet.rcvs.org.uk/find-a-vet/?filter-choice=location&search=true#show-results&filter-keyword=Cookstown
 * (search keywork = Cookstown)
 *
 * Anyway, using this URL seems be able to get all result
 *	http://findavet.rcvs.org.uk/find-a-vet/?filter-choice=location&search=true#show-results&filter-keyword=
 *(request Location search without keyword)
 *
 * 2017-04-13
 *	URL search by county
 *		http://www.rcvs.org.uk/find-a-vet/search-by/county/
 */

//#define BASEURL "http://findavet.rcvs.org.uk/find-a-vet/?filter-choice=location&search=true#show-results&filter-keyword="
//#define BASEURL		"http://findavet.rcvs.org.uk/find-a-vet/?search=true&filter-keyword=&filter-choice=location&filter-comp_med=&filter-food_hyg=&filter-qualifications=&filter-specialism=&filter-ems=&filter-training=&filter-pss=&filter-postcode=&filter-radius=&filter-advanced_practices=&p=287"
#define BASEURL	"http://www.rcvs.org.uk/find-a-vet/search-by/county/"

find_a_vet::find_a_vet(QWidget *apParent)
	: BaseWebEngine(apParent) {
	// for using custom control, call this function then implement initExtWidget()
	initCustomControl();

	setWindowTitle("find_a_vet");
	setBaseURL(BASEURL);

	if(mpDBLogger != NULL)
		mpDBLogger->setLogPath("find_a_vet.sql");

	mpLog->setFilePath("find_a_vet.txt");
	///////////////////////////////////////////////////////////
}

enum enuDatabase {
	eName = 0,
	eWebsite, eEmail, eTelephone, eFax,
	eAdd, eTown, eCounty, ePostcode, eFullAdd,
	eDatabaseMax
};

void find_a_vet::initDatabase() {
	mDbLstColName << QLatin1String("Name");
	mDbLstColName << QLatin1String("Website")
				  << QLatin1String("Email")
				  << QLatin1String("Telephone")
				  << QLatin1String("Fax")
				  << "Address" << "Town" << "County" << "Postcode" << "Full Address";
	mDbLstKey << eName << ePostcode;
	mDbTableName = "find_a_vet";
}

void find_a_vet::doStart() {
	do {
		QString path = mpExtFileLoading->getLoadPath();
		if (path.isEmpty()) {
			meState = E_STATE_INIT;
			loadPage(BASEURL);
			break;
		}

		// load data from previous setting
		mlstDataLink = Utilities::loadTextData(path);
		if (mlstDataLink.isEmpty()) {
			showFinishMsg(QLatin1String("Data link is empty!"));
			break;
		}

		if (gotoNextDataLink() == false) {
			showFinishMsg(QLatin1String("Cannot load link from data list!"));
			break;
		}
	} while (false);
}

void find_a_vet::doWebLoadFinished() {
	doWait(1, 1);
}

void find_a_vet::wrapperPageLoadFinished() {
	switch(meState)
	{
	case E_STATE_INIT:
		getCountyList();
		break;

	case E_STATE_SUB1:
		getLocationSearchResult();
		break;

	case E_STATE_GET_DETAIL_INFO:
		getVetDetail();
		break;

	default:
		qDebug() << "default";
		break;
	}
}

void find_a_vet::doWaitFinished() {
	wrapperPageLoadFinished();
}

void find_a_vet::getCountyList() {
	do {
		mlstCategoryLink.clear();

		QWebElement element;
		QWebElementCollection aTags;

		aTags = mpWebView->findAll(QLatin1String("#searchbylistpage li > a"));
		foreach(element, aTags)
			mlstCategoryLink << mpWebView->getHrefURL(&element);

		if (gotoNextCategoryLink(E_STATE_SUB1) == false) {
			showFinishMsg(QLatin1String("Cannot load County links from data list!"));
			break;
		}
	} while (false);
}

void find_a_vet::getLocationSearchResult() {
	QWebElement element, eTmp;
	QWebElementCollection aTags;

	do {
		// colect all search result link
		// select a Tag - child of (td Tag having class=practice - inside of tag ID results)
		aTags = mpWebView->findAll(QLatin1String("#results div[class=\"practice-padding\"] > a"));
		foreach (element, aTags)
			mlstDataLink << mpWebView->getHrefURL(&element);

		// save links
		DEF_LOG_LIST(mlstDataLink, QLatin1String(""));
		qDebug() << QLatin1String("Save result: ") << mlstDataLink.count();
		mlstDataLink.clear();

		// goto next result
		// select a Tag - child of (li Tag having class=next - child of ol Tag having class=paging)
		aTags = mpWebView->findAll(QLatin1String("#paging ol[class=\"paging\"] > li"));
		foreach (element, aTags) {
			if (element.toPlainText() != QLatin1String(">"))
				continue;

			eTmp = element.findFirst(QLatin1String("a"));
		}

		if (eTmp.isNull()) {
			// goto next county
			if (gotoNextCategoryLink(E_STATE_SUB1) == false)
				showFinishMsg(QLatin1String("Finished extract all Location search result ..."));

			break;
		}

		mpWebView->doClick(&eTmp);
	} while(false);
}

void find_a_vet::getVetDetail() {
	DataEntry data(mDbLstColName);
	QWebElement element;

	// Name
	updateData(&data, eName, QLatin1String("#primary div[class=\"practice-title-back\"] > h1"));

	QWebElement contact = mpWebView->findFirst(QLatin1String("#practicecontactdetails"));

	// Address
	{
		element = contact.findFirst(QLatin1String("p"));
		QString address = element.toPlainText();
		if (address.endsWith("\n"))
			address.chop(1);
		bool containCountry = address.endsWith(QLatin1String("United Kingdom"), Qt::CaseInsensitive);
		TAG_ADDRESS tAdd = Utilities::parseUK_Add(address, containCountry);

		data.updatePropertyString(mDbLstColName[eAdd], tAdd.strAddress);
		data.updatePropertyString(mDbLstColName[eTown], tAdd.strTown);
		data.updatePropertyString(mDbLstColName[eCounty], tAdd.strCounty);
		data.updatePropertyString(mDbLstColName[ePostcode], tAdd.strPostcode);
		data.updatePropertyString(mDbLstColName[eFullAdd], tAdd.strFullAddress);
	}

	// website, email, telephone, fax
	{
		contact = mpWebView->findFirst(QLatin1String("#practicenumbers"));

		updateData(&data, eTelephone, QLatin1String("i[class=\"fa fa-phone\"] + span"), &contact);
		updateData(&data, eFax, QLatin1String("i[class=\"fa fa-print\"] + span"), &contact);
		updateData(&data, eEmail, QLatin1String("i[class=\"fa fa-envelope\"] + span > a"), &contact);
		updateData(&data, eWebsite, QLatin1String("i[class=\"fa fa-globe\"] + span > a"), &contact,
				   QLatin1String("href"));
	}

	mlstData << data;
	if (mlstData.count() >= 10) {
		saveData(&mlstData);
		mlstData.clear();
	}

	if (gotoNextDataLink() == false) {
		if (mlstData.isEmpty() == false)
			saveData(&mlstData);

		showFinishMsg(QLatin1String("Finished extract all Vet info ..."));
	}
}

WebEngine_Main(find_a_vet)
