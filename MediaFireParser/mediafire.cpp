#include "mediafire.h"

#include <QWebElement>
#include <QWebElementCollection>
#include <QFile>

//#define		BASEURL		"http://www.mediafire.com"
#define		BASEURL		"http://www.mediafire.com/?sharekey=71799e804f221f22d5a101cf914073b4e08419366049f13f"
#define		BASEROOT	"http://www.mediafire.com"

MediaFire::MediaFire(QWidget *parent)
	: BaseWebEngine(parent)
{
//	if(getBaseURL().isEmpty())
//		setBaseURL(BASEURL);

	setWindowTitle("www.mediafire.com");
	setWebLoadTimeout(20000);

	meDownMode = E_MODE_MAX;
}

void MediaFire::setSavedLinkPath(QString astrPath)
{
	mstrSavedLinkPath = astrPath;
}

bool MediaFire::isCategory(QString astrURL)
{
	bool lblRet = false;

	lblRet = astrURL.contains("sharekey");

	return lblRet;
}

void MediaFire::doStart()
{
	meState = E_STATE_INIT;
	loadPage();
}

void MediaFire::doWebLoadFinished()
{
	switch(meState)
	{
	case E_STATE_INIT:
		getSubPage();
		break;

	case E_STATE_GET_DETAIL_INFO:
		getDownloadLink();
		break;

	default:
		break;
	}
}

bool MediaFire::updateItemPerPage()
{
	bool		lblRet = false;
	QWebElement	leElement;
	QWebElementCollection	leLst;

	do
	{
		qDebug() << "ABC";
		leElement = mpWebView->getElementById("count_current");
		DEF_CHECK_BREAK(leElement, "Can not get count_current element");

	// Click on this item
		mpWebView->doClick(&leElement);

	// Get the next element
		leElement = leElement.nextSibling();
		DEF_CHECK_BREAK(leElement, "Can not get number item list");

		leLst = leElement.findAll("li");
		if(leLst.count() <= 0)
		{
			DEF_LOG("Get invalid number of item element");
			break;
		}

		leElement = leLst.last();
		mpWebView->doClick(&leElement);

		lblRet = true;
	}while(false);

	return lblRet;
}

void MediaFire::getSubPage()
{
	QWebElement		leElement;
	QString			lstrTmp;
	QWebElementCollection	leLst;

	do
	{
		if(updateItemPerPage() == false)
		{
			DEF_LOG("Can not set number of item/page");
			break;
		}

		leElement = mpWebView->getElementById("filelistingtab1");
		DEF_CHECK_BREAK(leElement, "Can not get the file list table");

		leLst = leElement.findAll("a");

		foreach(leElement, leLst)
		{
			lstrTmp = leElement.attribute("href");

			if(lstrTmp.startsWith("/download.php?"))
				mlstDataLink << BASEROOT + lstrTmp;
		}

		qDebug() << mlstDataLink;

		showFinishMsg("Finished getting link");
		meState = E_STATE_MAX;
	}while(false);
}

bool MediaFire::loadLinkFromFile()
{
	bool	lblRet = false;
	QFile	file;

	do
	{
		file.setFileName(mstrSavedLinkPath);
		if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
			break;

		mlstDataLink.clear();
		while(!file.atEnd())
			mlstDataLink << file.readLine();

		file.close();
		lblRet = true;
	}while(false);

	return lblRet;
}

void MediaFire::startDownloadFromFile()
{
	do
	{
		mlstDownloadLink.clear();

		if(loadLinkFromFile() == false)
		{
			qDebug() << "Can not load links from file";
			DEF_LOG("Can not load links from file");
			showFinishMsg("Can not load links from file");
			break;
		}

		if(gotoNextDataLink(E_STATE_GET_DETAIL_INFO) == false)
		{
			qDebug() << "Finished";
			DEF_LOG("Finished");
			showFinishMsg();
			break;
		}
	}while(false);
}

void MediaFire::getDownloadLink()
{
	QWebElement				lelement;
	QWebElementCollection	leCol;

	do
	{
		lelement = mpWebView->document().findFirst("div[style=\"display: block; \"]");
		DEF_CHECK_BREAK(lelement, "Can not get element dl_startlink");
		leCol = lelement.findAll("a");

		foreach(lelement, leCol)
		{
			if(lelement.toPlainText() == DEF_STR_DOWNLOAD_LINK_TEXT)
				mlstDownloadLink << lelement.attribute("href");
		}
	}while(false);

	if(gotoNextDataLink(E_STATE_GET_DETAIL_INFO) == false)
	{
		qDebug() << "Finished";
		DEF_LOG("Finished");

		DEF_LOG_LIST(mlstDownloadLink, "Download link: ");

		showFinishMsg();
	}
}

void MediaFire::startDownload(DOWN_MODE aeMode)
{
	switch(aeMode)
	{
	case E_MODE_FROM_MEDIA_FOLDER:
		doStart();
		break;

	case E_MODE_FROM_SAVED_FILE:
		startDownloadFromFile();
		break;

	default:
		qDebug() << "Un-supported mode";
		break;
	}
}
