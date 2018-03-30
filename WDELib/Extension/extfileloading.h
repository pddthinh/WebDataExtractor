#ifndef EXTFILELOADING_H
#define EXTFILELOADING_H

#include "WDE.h"
#include "WDEDef.h"
#include <QWidget>

#include "textfilterwidget.h"
#include "Utilities/utilities.h"

class QCheckBox;
class QLineEdit;
class QPushButton;
class QGridLayout;
class QStringList;
class QSettings;

class WDE_LIB_EXPORT ExtFileLoading : public QWidget
{
	Q_OBJECT
public:
	explicit ExtFileLoading(QWidget *parent = 0);
	ExtFileLoading(QWidget *parent, QSettings *apPref);

	bool getExtractLink();
	QString getLoadPath();
	int getNumberOfSkipItem();
	bool isLoadFromFile();
	int getWebTimeout();
	QStringList loadData();
	void saveSettings();

	QString getParamCategory();
	int getParamPageIndex();

	void setApplicationSettings(QSettings *pPref) {
		mpAppPref = pPref;
	}

private slots:
	void sltBtnBrowseClicked();
	void setEnabled(bool ablEnabled);

protected:
	void initControl();
	void initOptExtraction(QGridLayout *apMainLayout);
	void initOptLoad(QGridLayout *apMainLayout);

	void loadSettings();

protected:
	QCheckBox		*mpChkBoxGetLink;

	QLineEdit		*mpTxtLoadPath;
	QPushButton		*mpBtnBrowse;

	QLineEdit		*mpTxtNumberOfSkipItem;

	QLineEdit		*mpTxtWebTimeout;

	QLineEdit		*mpParamCategory;
	QLineEdit		*mpParamPageIdx;

private:
	TextFilterWidget	*mpTextFilter;
	QSettings			*mpAppPref;
};

#endif // EXTFILELOADING_H
