#include "remainwnd.h"
#include <QDebug>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>

#define SEPERATOR	"----------------------------------------------------------------"

QString gSites[] = {
	"http://www.tlcrealestate.com.au/consultants/",
	"http://www.loverealestate.com.au/About/Our-Team",
	"http://www.bigginscott.com.au/consultants/office/21/altona",
	"http://www.hockingstuart.com.au/?About-Us/Find-An-Agent",
	"http://www.garypeer.com.au/about/the-team/",
	"http://www.peterblackshaw.com.au/people",
	"http://www.realmark.com.au/staff.cfm",
	"http://www.wwre.com.au/About/Our-Team",
	"http://www.thepropertyexchange.com.au/team.aspx",
	"http://www.re88.com.au/team.aspx",
	SEPERATOR,
	"http://www.abelmcgrath.com/team.aspx",
	"http://www.jennyandrews.com.au/staff",
	"http://www.noeljones.com.au/our-team/property-consultants/",
	"http://www.barrandstandley.com.au/sales-agents.php",
	"http://doylespillane.com.au/Team-Profiles.php",
	"http://www.conduitagencies.com/main.php?page=ourteam",
	"http://www.sheads.com.au/",
	"http://kevinwrightre.com.au/?go=Contact+Us",
	"http://www.rtforsyth.com.au/Team-Profiles.php",
	"http://www.blackwaterrealestate.com.au/pol/property/staff.asp?f_AgentID=4703&xsl=4703",
	SEPERATOR,
	"http://www.laingsimmons.com.au/staff-profiles.html",
	"http://www.starrpartners.com.au/about/find-an-office?",
	"http://chadwickrealestate.com.au/Our-Team.php?roletype=all",
	"http://www.dignam.com.au/about-us/meet-the-team.html",
	"http://www.magainagain.com.au/index-tess1.php?ItemNo=1057",
	"http://www.robertcrawfordrealestate.com.au/index.cfm?pagecall=staff&MenuItemID=27442&/search/pageid=Our%20Team",
	"http://reiwa.com.au/Agents/Pages/Find-reiwa-agent.aspx",
	"http://www.crawfordrealty.com.au/",
	"",
	"",
	"",
	"",
	"",
};

REMainWnd::REMainWnd(QWidget *parent) :
	QWidget(parent)
{
	initControls();
	initSiteList();

	mpSiteList->setCurrentRow(0);
	mpEngine = NULL;
}

void REMainWnd::initControls()
{
	QGridLayout *lpMainLayout = new QGridLayout(this);

	lpMainLayout->addWidget(new QLabel("Site list:"), 0, 0, 1, 2);
	mpSiteList = new QListWidget(this);
	connect(mpSiteList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(sltDbClickItem(QListWidgetItem*)));
	lpMainLayout->addWidget(mpSiteList, 1, 0, 1, 2);

	QPushButton *lpBtnExit = new QPushButton("Exit", this);
	connect(lpBtnExit, SIGNAL(clicked()), this, SLOT(close()));
	lpMainLayout->addWidget(lpBtnExit, 2, 1);

	QPushButton *lpBtnExe = new QPushButton("Run", this);
	connect(lpBtnExe, SIGNAL(clicked()), this, SLOT(sltRun()));
	lpMainLayout->addWidget(lpBtnExe, 2, 0);

	this->setLayout(lpMainLayout);
}

void REMainWnd::initSiteList()
{
	do
	{
		if (mpSiteList == NULL)
			break;

		QStringList labels;
		int i, j;
		int iType;
		QListWidgetItem *lpItem = NULL;

		i = 0;
		j = 0;
		while (gSites[i] != "")
		{
			iType = -1;
			if (gSites[i] != SEPERATOR)
			{
				iType = j;
				j ++;
			}

			lpItem = new QListWidgetItem(gSites[i], mpSiteList, iType);
			mpSiteList->addItem(lpItem);

			i ++;
		}

		mpSiteList->addItems(labels);
	} while (false);
}

real_estate *REMainWnd::getEngine(ENU_SITE aeSite, QString url)
{
	real_estate *lpEngine = NULL;

	do
	{
		if (aeSite == E_SITE_MAX)
			break;

		switch (aeSite)
		{
		case E_SITE_tlcrealestate:
			lpEngine = new tlcrealestate(url);
			break;

		case E_SITE_loverealestate:
			lpEngine = new loverealestate(url);
			break;

		case E_SITE_bigginscott:
			lpEngine = new bigginscott(url);
			break;

		case E_SITE_hockingstuart:
			lpEngine = new hockingstuart(url);
			break;

		case E_SITE_garypeer:
			lpEngine = new garypeer(url);
			break;

			// TODO: this site has no email address
		case E_SITE_peterblackshaw:
			lpEngine = new peterblackshaw(url);
			break;

		case E_SITE_realmark:
			lpEngine = new realmark(url);
			break;

		case E_SITE_wwre:
			lpEngine = new wwre(url);
			break;

		case E_SITE_thepropertyexchange:
			lpEngine = new thepropertyexchange(url);
			break;

		case E_SITE_re88:
			lpEngine = new re88(url);
			break;

		case E_SITE_abelmcgrath:
			lpEngine = new abelmcgrath(url);
			break;

		case E_SITE_jennyandrews:
			lpEngine = new jennyandrews(url);
			break;

		case E_SITE_noeljones:
			lpEngine = new noeljones(url);
			break;

		case E_SITE_barrandstandley:
			lpEngine = new barrandstandley(url);
			break;

		case E_SITE_doylespillane:
			lpEngine = new doylespillane(url);
			break;

		case E_SITE_conduitagencies:
			lpEngine = new conduitagencies(url);
			break;

		case E_SITE_sheads:
			lpEngine = new sheads(url);
			break;

		case E_SITE_kevinwrightre:
			lpEngine = new kevinwrightre(url);
			break;

		case E_SITE_rtforsyth:
			lpEngine = new rtforsyth(url);
			break;

		case E_SITE_blackwaterrealestate:
			lpEngine = new blackwaterrealestate(url);
			break;

		case E_SITE_laingsimmons:
			lpEngine = new laingsimmons(url);
			break;
		case E_SITE_starrpartners:
			lpEngine = new starrpartners(url);
			break;
		case E_SITE_chadwickrealestate:
			lpEngine = new chadwickrealestate(url);
			break;
		case E_SITE_dignam:
			lpEngine = new dignam(url);
			break;
		case E_SITE_magainagain:
			lpEngine = new magainagain(url);
			break;
		case E_SITE_robertcrawfordrealestate:
			lpEngine = new robertcrawfordrealestate(url);
			break;
		case E_SITE_reiwa:
			lpEngine = new reiwa(url);
			break;
		case E_SITE_crawfordrealty:
			lpEngine = new crawfordrealty(url);
			break;
			//		case E_SITE_:
			//			lpEngine = new (url);
			//			break;
			//		case E_SITE_:
			//			lpEngine = new (url);
			//			break;
			//		case E_SITE_:
			//			lpEngine = new (url);
			//			break;
			//		case E_SITE_:
			//			lpEngine = new (url);
			//			break;
			//		case E_SITE_:
			//			lpEngine = new (url);
			//			break;

		default:
			lpEngine = new real_estate(this);
			break;
		}
	} while (false);

	return lpEngine;
}

void REMainWnd::sltRun()
{
	sltDbClickItem(mpSiteList->currentItem());
}

void REMainWnd::sltDbClickItem(QListWidgetItem *apItem)
{
	do
	{
		if (apItem == NULL)
			break;

		qDebug() << "DbClick on item type: " << apItem->type();
		real_estate *lpEngine = NULL;
		int iType = apItem->type();
		if (iType == -1)
			break;

		QString url = apItem->text();
		lpEngine = getEngine((ENU_SITE)iType, url);
		connect(lpEngine, SIGNAL(finished(int)), this, SLOT(show()));

		this->hide();
		lpEngine->showNormal();
	} while (false);
}
