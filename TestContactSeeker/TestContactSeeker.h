#ifndef TEMPLATEPAGE_H
#define TEMPLATEPAGE_H

#include "WDEInclude.h"

#include <QDebug>
#include <QPushButton>
//#include <QTextEdit>
#include <QLineEdit>

using namespace nsBaseWebEngine;

class TestContactSeeker : public QWidget
{
	Q_OBJECT
public:
	TestContactSeeker(QWidget *apParent = 0);

protected:
	void initControl();
	void initSeeker();

protected Q_SLOTS:
	void sltBtnBrowseClick();
	void sltBtnStartClick();
	void sltBtnStopClick();

	void sltSeekerStatusChanged(QString astrStatus);
	void sltSeekerProcessFinished(ContactSeeker::ProcessStatus aStatus);
	void sltSeekerFinishedGet1Contact(ContactSeeker::TAG_CONTACT_INFO atInfo);

private:
	QPushButton			*mpBtnStart;
	QPushButton			*mpBtnStop;
	QPushButton			*mpBtnExit;

	QPushButton			*mpBtnBrowse;
	QLineEdit			*mpTxtInputPath;

	QLineEdit			*mpTxtStatus;
	ContactSeeker		*mpSeeker;
};

#endif // TEMPLATEPAGE_H
