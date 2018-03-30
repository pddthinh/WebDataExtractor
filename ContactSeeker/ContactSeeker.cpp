#include "ContactSeeker.h"

#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <Utilities/utilities.h>
#include <QRegExp>

#define		LOG_NOT_FOUND_CONTACT_PATH	QLatin1String("ContactSeeker-not-found.txt")

TContactSeeker::TContactSeeker(QWidget *apParent)
	: BaseWebEngine(apParent)
{
	initCustomControl();

	setWindowTitle(QLatin1String("ContactSeeker"));

	if(mpDBLogger != NULL)
		mpDBLogger->setLogPath(QLatin1String("ContactSeeker.sql"));

	mpLog->setFilePath(QLatin1String("ContactSeeker.txt"));

	mpContactLog = new Logger(LOG_NOT_FOUND_CONTACT_PATH);
	///////////////////////////////////////////////////////////
	setWebLoadTimeout(20000);
}

void TContactSeeker::initExtWidget()
{
	mpExtFileLoading = new ExtFileLoading(this);
}

enum enuDatabase
{
	eName = 0,
	eEmail, eWebsite,
	eDatabaseMax
};

void TContactSeeker::initDatabase()
{
	mDbLstColName << QLatin1String("Name")
				  << QLatin1String("Email") << QLatin1String("Website");
	mDbLstKey << eName;
	mDbTableName = QLatin1String("yellowpage_au");
}

void TContactSeeker::start()
{
	int liSkip;

	do
	{
		loadInputWebsiteList(mpExtFileLoading->getLoadPath());

		liSkip = mpExtFileLoading->getNumberOfSkipItem();
		while (liSkip > 0)
		{
			mlstDataLink.takeFirst();
			liSkip --;
		}

		if (gotoNextDataLink(E_STATE_SUB1) == false)
		{
			DEF_LOG(QLatin1String("Finished for this URL."));
			emit sigProcessFinished(ENU_PROCESS_UNDEFINED);
			break;
		}

		emit sigStatusMsgChanged(QString("Loading page: [%1]").arg(mpWebView->url().toString()));
	} while(false);
}

void TContactSeeker::setInputWebsiteList(QStringList alstWebsite)
{
	mlstDataLink.clear();
	mlstDataLink << alstWebsite;
	miDataSize = mlstDataLink.size();
}

void TContactSeeker::loadInputWebsiteList(QString astrFilePath)
{
	mlstDataLink = Utilities::loadTextData(astrFilePath);

	miDataSize = mlstDataLink.size();

	qDebug() << QLatin1String("Loaded web links: ") << miDataSize;
}

void TContactSeeker::doStart()
{
	start();
}

void TContactSeeker::doWebLoadFinished()
{
	switch(meState)
	{
	case E_STATE_SUB1:
		mtContactInfo.mstrWebSite = mpWebView->url().toString();
		DEF_LOG(QString("------------------------------------------\n"
						"Searching contact for: [%1]")
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

void TContactSeeker::searchContactInfoLink()
{
	QWebElementCollection	leCol;
	QWebElement				lele;
	QString					lstrLink;

	do
	{
		mlstContactLink.clear();

		leCol = mpWebView->findAll(QLatin1String("a"));
		foreach(lele, leCol)
		{
			if (isContactInfoLink(lele) == false)
				continue;

			lstrLink = mpWebView->getHrefURL(&lele);
			if (lstrLink.indexOf('#') < 0)
				mlstContactLink << lstrLink;
		}

		mlstContactLink.removeDuplicates();
		DEF_LOG(QString("Found contact link: [%1]")
				.arg(mlstContactLink.size()));
		DEF_LOG_LIST(mlstContactLink, "");

		// mining the home page
		searchEmailAddress();

		// Loop all found links
		if (gotoNextContactLink() == false)
		{
			if (gotoNextDataLink(E_STATE_SUB1) == false)
			{
				DEF_LOG(QString("Finished site: %1")
						.arg(mpWebView->url().toString()));

				// save / notify result
				emit sigFinishGetOneContact(mtContactInfo);

				if (mtContactInfo.isEmpty() == false)
				{
					DEF_LOG(QString("Found email: %1").arg(mtContactInfo.getEmails()));
					mpDBLogger->logString(getSQLUpdateEmail());
					mtContactInfo.clear();
				}

				showFinishMsg(QLatin1String("Extract finished ..."));
				break;
			}
		}
	} while(false);
}

/**
 * Heuristic function to determine a link is contact/about link
 *
 * @param link
 * @return
 */
bool TContactSeeker::isContactInfoLink(const QWebElement &link)
{
	bool lblRet = false;
	QRegExp regEx("contact|about", Qt::CaseInsensitive);
	QWebElementCollection wCol;
	QWebElement element;

	do
	{
		if (link.isNull() == true)
			break;

		lblRet = true;
		if (regEx.indexIn(link.toPlainText()) >= 0)
			break;

		if (regEx.indexIn(link.attribute(QLatin1String("href"))) >= 0)
			break;

		wCol = link.findAll(QLatin1String("*"));
		foreach (element, wCol)
		{
			if (regEx.indexIn(element.attribute(QLatin1String("alt"))) >= 0)
				goto _end;
		}

		lblRet = false;
	} while (false);

_end:
	return lblRet;
}

bool TContactSeeker::gotoNextContactLink()
{
	bool lblRet = false;

	do
	{
		if (mlstContactLink.isEmpty() == true)
			break;

		emit sigStatusMsgChanged(QString("Remain %1 Contact link")
								 .arg(mlstContactLink.size()));

		meState = E_STATE_SUB2;
		loadPage(mlstContactLink.takeFirst());
		lblRet = true;
	} while(false);

	return lblRet;
}

void TContactSeeker::searchContactInfo()
{
	searchEmailAddress();

	if (gotoNextContactLink() == false)
	{
		DEF_LOG(QString("Item: %1").arg(miDataSize - mlstDataLink.size()));
		qDebug() << QLatin1String("Item: ") << miDataSize - mlstDataLink.size();

		// colected all contact info
		// emit to outside
		if (mtContactInfo.isEmpty() == true)
		{
			DEF_LOG(QLatin1String("\t\tNot found any contact"));
			mpContactLog->logString(mtContactInfo.mstrWebSite);
		}
		else
		{
			// save / notify result
			emit sigFinishGetOneContact(mtContactInfo);

			DEF_LOG(QString("Found email: %1").arg(mtContactInfo.getEmails()));
			mpDBLogger->logString(getSQLUpdateEmail());
			mtContactInfo.clear();
		}

		if (gotoNextDataLink(E_STATE_SUB1) == false)
		{
			DEF_LOG(QLatin1String("Finished sites"));
			emit sigProcessFinished(ENU_PROCESS_UNDEFINED);

			//			showFinishMsg(QLatin1String("Finish extract process ..."));
		}
	}
}

bool TContactSeeker::searchEmailAddress()
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

bool TContactSeeker::searchEmailByText()
{
	bool lblRet = false;
	QString lstrTmp;
	QStringList llstEmail;

	do
	{
		lstrTmp = mpWebView->document().toPlainText() + QLatin1String("\n");

		llstEmail.append(Utilities::extractEmailAddress(lstrTmp));
		llstEmail.removeDuplicates();
		if (llstEmail.isEmpty() == true)
			break;

		mtContactInfo.mEmails << llstEmail;

		lblRet = true;
	} while(false);

	return lblRet;
}

bool TContactSeeker::searchEmailByLink()
{
	bool lblRet = false;
	QWebElementCollection llstEle;
	QString lstrTmp;
	QStringList llstEmail;

	do
	{
		llstEle = mpWebView->findAll(QLatin1String("a"));
		foreach (QWebElement lele, llstEle)
		{
			lstrTmp = lele.attribute(QLatin1String("href"));
			if (lstrTmp.indexOf(QLatin1String("mailto:")) != -1)
				llstEmail << Utilities::extractEmailAddress(lstrTmp);
		}

		llstEmail.removeDuplicates();
		if (llstEmail.isEmpty() == true)
			break;

		mtContactInfo.mEmails << llstEmail;

		lblRet = true;
	} while(false);

	return lblRet;
}

QString TContactSeeker::getSQLUpdateEmail()
{
	QString lstrRet = "";
	QString lstrSQL = QLatin1String("UPDATE IGNORE `%1` SET %2 WHERE %3;");
	QString lstrValue = QLatin1String("`%1`='%2'");
	QString lstrWhere = QLatin1String("`%1` LIKE '%%2%'");
	QString lstrTmp;

	do
	{
		if (mtContactInfo.mEmails.isEmpty())
			break;

		lstrTmp = mtContactInfo.getEmails();
		if (lstrTmp.isEmpty())
			break;

		lstrValue = lstrValue
				.arg(mDbLstColName[eEmail])
				.arg(Utilities::correctData(lstrTmp));

		lstrTmp = mtContactInfo.mstrWebSite.replace(QLatin1String("http://"), "");
		if (lstrTmp.endsWith('/'))
			lstrTmp.chop(1);
		lstrWhere = lstrWhere
				.arg(mDbLstColName[eWebsite])
				.arg(Utilities::correctData(lstrTmp));

		lstrRet = lstrSQL
				.arg(mDbTableName)
				.arg(lstrValue)
				.arg(lstrWhere);
	} while (false);

	return lstrRet;
}

WebEngine_Main(TContactSeeker)
