#ifndef CONTACTSEEKER_H
#define CONTACTSEEKER_H

#include "WDEInclude.h"

#include <QDebug>
#include <QWebElement>
#include <QWebElementCollection>

using namespace nsBaseWebEngine;

class WDE_LIB_EXPORT ContactSeeker : public BaseWebEngine
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
		QString mstrEmail;

		void clear()
		{
			mstrWebSite = "";
			mstrEmail = "";
		}

		QString toString()
		{
			return QString("Website: [%1],\tEmail: [%2]")
					.arg(mstrWebSite)
					.arg(mstrEmail);
		}
	} TAG_CONTACT_INFO;

public:
	ContactSeeker(QWidget *apParent = 0);
	void start();
	void stop();
	void setInputWebsiteList(QStringList alstWebsite);
	void loadInputWebsiteList(QString astrFilePath);

Q_SIGNALS:
	void sigStatusMsgChanged(QString astrMsg);
	void sigProcessFinished(ContactSeeker::ProcessStatus aeStatus);
	void sigFinishGetOneContact(ContactSeeker::TAG_CONTACT_INFO atInfo);

protected:
	void doStart();
	void doStop(){}
	void doWebLoadFinished();
	void initDatabase();
	void initExtWidget(){}

private:
	void searchContactInfoLink();
	void searchContactInfo();
	bool gotoNextContactLink();

	// Email address mining
	bool searchEmailAddress();
	bool searchEmailByText();
	bool searchEmailByLink();

private:
	QStringList mlstContactLink;
	TAG_CONTACT_INFO mtContactInfo;
};

#endif // CONTACTSEEKER_H
