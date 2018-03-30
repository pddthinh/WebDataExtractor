#include "ContactSeeker.h"

#define BASEURL ""
#define DEF_STR_PATH "website.txt"

#include <Utilities/utilities.h>

ContactSeeker::ContactSeeker(QWidget *apParent)
	: BaseWebEngine(apParent)
{
	setWindowTitle(QLatin1String("ContactSeeker"));
	setBaseURL(BASEURL);

	if(mpDBLogger != NULL)
		mpDBLogger->setLogPath(QLatin1String("ContactSeeker.sql"));

	mpLog->setFilePath(QLatin1String("ContactSeeker.txt"));
	///////////////////////////////////////////////////////////
//	loadInputWebsiteList(DEF_STR_PATH);
}

enum enuDatabase
{
	eName = 0,
	eDatabaseMax
};

void ContactSeeker::initDatabase()
{
	mDbLstColName << QLatin1String("");
	mDbLstKey << eName;
	mDbTableName = QLatin1String("ContactSeeker");
}

void ContactSeeker::start()
{
	do
	{
		if (gotoNextDataLink(E_STATE_SUB1) == false)
		{
			DEF_LOG(QLatin1String("Finished for this URL."));
			emit sigProcessFinished(ENU_PROCESS_UNDEFINED);
			break;
		}

		emit sigStatusMsgChanged(QString("Loading page: [%1]")
								 .arg(mpWebView->url().toString()));
	} while(false);
}

void ContactSeeker::stop()
{
	mpWebView->stop();
}

void ContactSeeker::setInputWebsiteList(QStringList alstWebsite)
{
	mlstDataLink.clear();
	mlstDataLink << alstWebsite;
}

void ContactSeeker::loadInputWebsiteList(QString astrFilePath)
{
	mlstDataLink = Utilities::loadTextData(astrFilePath);

	DEF_LOG(QString("Loaded entries: %1")
			.arg(mlstDataLink.size()));
}

void ContactSeeker::doStart()
{
	start();
}

void ContactSeeker::doWebLoadFinished()
{
	switch(meState)
	{
	case E_STATE_SUB1:
		mtContactInfo.clear();
		mtContactInfo.mstrWebSite = mpWebView->url().toString();
		DEF_LOG(QString("------------------------------------------\n"
						"Start searching contact for: [%1]")
				.arg(mtContactInfo.mstrWebSite));

		emit sigStatusMsgChanged(QLatin1String("Page loaded, start searching contact info link ..."));
		searchContactInfoLink();
		break;

	case E_STATE_SUB2:
		emit sigStatusMsgChanged(QLatin1String("Searching contact info ..."));
		searchContactInfo();
		break;

	default:
		qDebug() << QLatin1String("default");
		break;
	}
}

void ContactSeeker::searchContactInfoLink()
{
	QWebElementCollection	leCol;
	QWebElement				lele;
	QString					lstrTmp;

	do
	{
		mlstContactLink.clear();

		leCol = mpWebView->findAll(QLatin1String("a"));
		foreach(lele, leCol)
		{
			lstrTmp = lele.toPlainText();

			if (lstrTmp.contains(QLatin1String("contact"), Qt::CaseInsensitive)
					|| lstrTmp.contains(QLatin1String("about us"), Qt::CaseInsensitive))
				mlstContactLink << mpWebView->getHrefURL(&lele);
		}

		DEF_LOG(QString("Found contact link: [%1]")
				.arg(mlstContactLink.size()));

		// Loop all found links
		if (gotoNextContactLink() == false)
		{
			if (gotoNextDataLink(E_STATE_SUB1) == false)
			{
				DEF_LOG(QLatin1String("Finished for this URL."));
				emit sigProcessFinished(ENU_PROCESS_UNDEFINED);
				break;
			}
		}
	} while(false);
}

bool ContactSeeker::gotoNextContactLink()
{
	bool lblRet = false;

	do
	{
		if (mlstContactLink.isEmpty() == true)
			break;

		meState = E_STATE_SUB2;
		loadPage(mlstContactLink.takeFirst());
		emit sigStatusMsgChanged(QString("Remain Contact link: %1")
								 .arg(mlstContactLink.size()));
		lblRet = true;
	} while(false);

	return lblRet;
}

void ContactSeeker::searchContactInfo()
{
	bool lblResult = searchEmailAddress();

	if (lblResult == true)
		emit sigFinishGetOneContact(mtContactInfo);

	if (gotoNextContactLink() == false)
	{
		if (gotoNextDataLink(E_STATE_SUB1) == false)
		{
			DEF_LOG("Finished for this URL.");
			emit sigProcessFinished(ENU_PROCESS_UNDEFINED);
			//			showFinishMsg();
		}
	}
}

bool ContactSeeker::searchEmailAddress()
{
	bool lblRet = false;

	do
	{
		lblRet = searchEmailByLink();
		if (lblRet == true)
			break;

		lblRet = searchEmailByText();
	} while(false);

	return lblRet;
}

bool ContactSeeker::searchEmailByText()
{
	bool lblRet = false;
	QString lstrTmp;
	QStringList llstEmail;

	do
	{
		lstrTmp = mpWebView->document().toPlainText() + QLatin1String("\n");
		llstEmail.append(Utilities::extractEmailAddress(lstrTmp));
		llstEmail.removeDuplicates();

		if (llstEmail.size() == 0)
			break;

		foreach (QString lstr, llstEmail)
			mtContactInfo.mstrEmail += lstr + QLatin1String("; ");

		mtContactInfo.clear();
		mtContactInfo.mstrWebSite = mpWebView->url().toString();
		mtContactInfo.mstrEmail.chop(2);

		lblRet = true;
	} while(false);

	return lblRet;
}

bool ContactSeeker::searchEmailByLink()
{
	bool lblRet = false;
	QWebElementCollection llstEle;
	QString lstrTmp = "";
	QStringList llstEmail;

	do
	{
		llstEle = mpWebView->findAll(QLatin1String("a"));
		foreach (QWebElement lele, llstEle)
		{
			lstrTmp = lele.attribute(QLatin1String("href"));
			if (lstrTmp.indexOf(QLatin1String("mailto:")) != -1)
				llstEmail << lstrTmp.replace(QLatin1String("mailto:"), "");
		}

		llstEmail.removeDuplicates();

		if (llstEmail.size() == 0)
			break;

		foreach (QString lstr, llstEmail)
			mtContactInfo.mstrEmail += lstr + QLatin1String("; ");

		mtContactInfo.clear();
		mtContactInfo.mstrWebSite = mpWebView->url().toString();
		mtContactInfo.mstrEmail.chop(2);

		lblRet = true;
	} while(false);

	return lblRet;
}

//WebEngine_Main(ContactSeeker)
