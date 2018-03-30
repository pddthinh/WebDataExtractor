#ifndef TCONTACTSEEKER_H
#define TCONTACTSEEKER_H

#include "WDEInclude.h"

#include <QDebug>
#include <QWebElement>
#include <QWebElementCollection>

using namespace nsBaseWebEngine;

class TContactSeeker : public BaseWebEngine
{
	Q_OBJECT

public:
	enum ProcessStatus
	{
		ENU_PROCESS_UNDEFINED,
		ENU_PROCESS_FOUND_INFO,
		ENU_PROCESS_NOT_FOUND,
		ENU_PROCESS_MAX
	};

	typedef struct tagContactInfo
	{
		QString mstrWebSite;
		QStringList mEmails;

		void clear()
		{
			mstrWebSite = "";
			mEmails.clear();
		}

		bool isEmpty()
		{
			mEmails.removeDuplicates();

			return mEmails.isEmpty();
		}

		QString getEmails()
		{
			QString lstrEmails;

			foreach (QString email, mEmails)
				lstrEmails += email + "; ";
			if (lstrEmails.endsWith("; "))
				lstrEmails.chop(2);

			return lstrEmails;
		}
	} TAG_CONTACT_INFO;

public:
	TContactSeeker(QWidget *apParent = 0);
	void start();
	void setInputWebsiteList(QStringList alstWebsite);
	void loadInputWebsiteList(QString astrFilePath);
	QString getSQLUpdateEmail();

signals:
	void sigStatusMsgChanged(QString astrMsg);
	void sigProcessFinished(ProcessStatus aeStatus);
	void sigFinishGetOneContact(TAG_CONTACT_INFO atInfo);

protected:
	void doStart();
	void doStop(){}
	void doWebLoadFinished();
	void initDatabase();
	void initExtWidget();

private:
	void searchContactInfoLink();
	void searchContactInfo();
	bool gotoNextContactLink();

	// Email address mining
	bool isContactInfoLink(const QWebElement &link);
	bool searchEmailAddress();
	bool searchEmailByText();
	bool searchEmailByLink();

private:
	QStringList mlstContactLink;
	TAG_CONTACT_INFO mtContactInfo;

	Logger		*mpContactLog;
	int			miDataSize;
};

#endif // TCONTACTSEEKER_H
