#include "extfileloading.h"
#include <QDebug>

#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QFileDialog>
#include <QIntValidator>
#include <QHBoxLayout>
#include <QSettings>

#define EXT_GRP_NAME			QLatin1String("ext-data")
#define EXT_KEY_EXTRACT_ONLY		QLatin1String("only-extract")
#define EXT_KEY_WEB_TIMEOUT		QLatin1String("web-timeout")
#define EXT_KEY_PARAM_CATEGORY		QLatin1String("param-category")
#define EXT_KEY_PARAM_PAGE_IDX		QLatin1String("param-page-idx")
#define EXT_KEY_FILE_PATH		QLatin1String("load-path")
#define EXT_KEY_SKIP_ITEM		QLatin1String("skip-item")


ExtFileLoading::ExtFileLoading(QWidget *parent)
	: QWidget(parent) {
	initControl();
	loadSettings();
}

ExtFileLoading::ExtFileLoading(QWidget *parent, QSettings *apPref)
	: QWidget(parent) {
	mpAppPref = apPref;

	initControl();
	loadSettings();
}

void ExtFileLoading::initControl() {
	QGridLayout		*lpMainLayout = new QGridLayout();

	initOptExtraction(lpMainLayout);
	initOptLoad(lpMainLayout);

	this->setLayout(lpMainLayout);
}

void ExtFileLoading::initOptExtraction(QGridLayout *apMainLayout) {
	QGroupBox	*lpGrpBox;
	QGridLayout	*lpGrpLayout;
	QPushButton *lpBtnTextFilter;

	do {
		if(apMainLayout == NULL)
			break;

		lpGrpBox = new QGroupBox(QLatin1String("Extraction Option"));
		lpGrpLayout = new QGridLayout();

		mpChkBoxGetLink = new QCheckBox(QLatin1String("Extract link only"), this);
		lpBtnTextFilter = new QPushButton(QLatin1String("Text filter"), this);

		mpTxtWebTimeout = new QLineEdit(this);
		QHBoxLayout *phLayout = new QHBoxLayout();
		phLayout->addWidget(new QLabel(QLatin1String("Web timeout (ms)")));
		phLayout->insertSpacing(1, 10);
		phLayout->addWidget(mpTxtWebTimeout);

		mpParamCategory = new QLineEdit(this);
		QHBoxLayout *phParam1 = new QHBoxLayout();
		phParam1->addWidget(new QLabel(QLatin1String("Param category")));
		phParam1->insertSpacing(1, 25);
		phParam1->addWidget(mpParamCategory);

		mpParamPageIdx = new QLineEdit(this);
		QHBoxLayout *phParam2 = new QHBoxLayout();
		phParam2->addWidget(new QLabel(QLatin1String("Param page index")));
		phParam2->insertSpacing(1, 10);
		phParam2->addWidget(mpParamPageIdx);

		lpGrpLayout->addWidget(mpChkBoxGetLink, 0, 0, 1, 1);
		lpGrpLayout->addWidget(lpBtnTextFilter, 0, 1, 1, 1, Qt::AlignLeft);
		lpGrpLayout->addItem(phLayout, 1, 0, 1, 2);
		lpGrpLayout->addItem(phParam1, 2, 0, 1, 2);
		lpGrpLayout->addItem(phParam2, 3, 0, 1, 2);
		lpGrpBox->setLayout(lpGrpLayout);

		apMainLayout->addWidget(lpGrpBox);
	} while(false);
}

void ExtFileLoading::initOptLoad(QGridLayout *apMainLayout) {
	QGroupBox	*lpGrpBox;
	QGridLayout	*lpGrpLayout;

	do {
		if(apMainLayout == NULL)
			break;

		lpGrpBox = new QGroupBox(QLatin1String("Load Option"), this);
		lpGrpLayout = new QGridLayout();

		mpTxtLoadPath = new QLineEdit(this);
		mpBtnBrowse = new QPushButton(QLatin1String("..."), this);
		mpTxtNumberOfSkipItem = new QLineEdit(this);

		lpGrpLayout->addWidget(new QLabel(QLatin1String("Load Path")), 0, 0);
		lpGrpLayout->addWidget(mpTxtLoadPath, 0, 1);
		lpGrpLayout->addWidget(mpBtnBrowse, 0, 2);

		lpGrpLayout->addWidget(new QLabel(QLatin1String("Skip items")), 1, 0);
		lpGrpLayout->addWidget(mpTxtNumberOfSkipItem, 1, 1, 1, 2);

		lpGrpBox->setLayout(lpGrpLayout);

		connect(mpBtnBrowse, SIGNAL(clicked()), this,
				SLOT(sltBtnBrowseClicked()));
		mpTxtNumberOfSkipItem->setValidator(new QIntValidator(this));

		apMainLayout->addWidget(lpGrpBox);
	} while(false);
}

void ExtFileLoading::sltBtnBrowseClicked() {
	QString lstrPath = QFileDialog::getOpenFileName(this,
													QLatin1String("Select file for load"));

	mpTxtLoadPath->clear();

	if(lstrPath != NULL)
		mpTxtLoadPath->setText(lstrPath);
}

void ExtFileLoading::setEnabled(bool ablEnabled) {
	QWidget::setEnabled(ablEnabled);

	if(mpChkBoxGetLink != NULL)
		mpChkBoxGetLink->setEnabled(ablEnabled);
	if(mpTxtLoadPath != NULL)
		mpTxtLoadPath->setEnabled(ablEnabled);
	if(mpBtnBrowse != NULL)
		mpBtnBrowse->setEnabled(ablEnabled);
	if(mpTxtNumberOfSkipItem != NULL)
		mpTxtNumberOfSkipItem->setEnabled(ablEnabled);
}

bool ExtFileLoading::getExtractLink() {
	bool lblRet = false;

	do {
		if(mpChkBoxGetLink == NULL)
			break;

		lblRet = mpChkBoxGetLink->isChecked();
	} while(false);

	return lblRet;
}

QString ExtFileLoading::getLoadPath() {
	QString lstrRet = "";

	do
	{
		if(mpTxtLoadPath == NULL)
			break;

		lstrRet = mpTxtLoadPath->text();
	} while(false);

	return lstrRet;
}

int ExtFileLoading::getNumberOfSkipItem() {
	int liRet = 0;

	do {
		if(mpTxtNumberOfSkipItem == NULL)
			break;

		liRet = mpTxtNumberOfSkipItem->text().toInt();

		if(liRet < 0)
			liRet = 0;
	} while(false);

	return liRet;
}

bool ExtFileLoading::isLoadFromFile() {
	return getLoadPath().length() > 0;
}

/**
 * Get the web timeout setting
 *
 * @return
 */
int ExtFileLoading::getWebTimeout() {
	int timeout = -1;

	do {
		if (mpTxtWebTimeout == NULL)
			break;

		timeout = mpTxtWebTimeout->text().toInt();
	} while (false);

	return timeout;
}

/**
 * Load the data from input path;
 * also removed the skipped items
 *
 * @return
 */
QStringList ExtFileLoading::loadData() {
	QStringList data;

	do {
		data = Utilities::loadTextData(getLoadPath());
		if (data.isEmpty() == true)
			break;

		// do the filter
		int numSkipped = getNumberOfSkipItem();
		while (numSkipped > 0) {
			numSkipped --;
			data.takeFirst();
		}
	} while(0);

	return data;
}

void ExtFileLoading::loadSettings() {
	do {
		if (mpAppPref == NULL)
			break;

		mpAppPref->beginGroup(EXT_GRP_NAME);

		mpChkBoxGetLink->setChecked(mpAppPref->value(EXT_KEY_EXTRACT_ONLY, false).toBool());
		mpTxtWebTimeout->setText(mpAppPref->value(EXT_KEY_WEB_TIMEOUT).toString());

		mpParamCategory->setText(mpAppPref->value(EXT_KEY_PARAM_CATEGORY).toString());
		mpParamPageIdx->setText(mpAppPref->value(EXT_KEY_PARAM_PAGE_IDX).toString());

		mpTxtLoadPath->setText(mpAppPref->value(EXT_KEY_FILE_PATH, QLatin1String("")).toString());
		mpTxtNumberOfSkipItem->setText(mpAppPref->value(EXT_KEY_SKIP_ITEM).toString());
		mpAppPref->endGroup();
	} while (false);
}

void ExtFileLoading::saveSettings() {
	do {
		if (mpAppPref == NULL)
			break;

		mpAppPref->beginGroup(EXT_GRP_NAME);

		mpAppPref->setValue(EXT_KEY_EXTRACT_ONLY, mpChkBoxGetLink->isChecked());
		mpAppPref->setValue(EXT_KEY_WEB_TIMEOUT, mpTxtWebTimeout->text());
		mpAppPref->setValue(EXT_KEY_PARAM_CATEGORY, mpParamCategory->text());
		mpAppPref->setValue(EXT_KEY_PARAM_PAGE_IDX, mpParamPageIdx->text());

		mpAppPref->setValue(EXT_KEY_FILE_PATH, mpTxtLoadPath->text());
		mpAppPref->setValue(EXT_KEY_SKIP_ITEM, mpTxtNumberOfSkipItem->text());

		mpAppPref->endGroup();
	} while (false);
}

QString ExtFileLoading::getParamCategory() {
	QString result;

	if (mpParamCategory != NULL)
		result = mpParamCategory->text();

	return result;
}

int ExtFileLoading::getParamPageIndex() {
	int index = -1;

	if (mpParamPageIdx != NULL)
		index = mpParamPageIdx->text().toInt();

	return index;
}
