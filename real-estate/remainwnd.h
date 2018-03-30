#ifndef REMAINWND_H
#define REMAINWND_H

#include <QWidget>
#include <QListWidget>
#include <QListWidgetItem>

#include "real_estate.h"

// sub class
#include "tlcrealestate.h"
#include "loverealestate.h"
#include "bigginscott.h"
#include "hockingstuart.h"
#include "garypeer.h"
#include "peterblackshaw.h"
#include "realmark.h"
#include "wwre.h"
#include "thepropertyexchange.h"
#include "re88.h"

#include "abelmcgrath.h"
#include "jennyandrews.h"
#include "noeljones.h"
#include "barrandstandley.h"
#include "doylespillane.h"
#include "conduitagencies.h"
#include "sheads.h"
#include "kevinwrightre.h"
#include "rtforsyth.h"
#include "blackwaterrealestate.h"

#include "laingsimmons.h"
#include "starrpartners.h"
#include "chadwickrealestate.h"
#include "dignam.h"
#include "magainagain.h"
#include "robertcrawfordrealestate.h"
#include "reiwa.h"
#include "crawfordrealty.h"
//#include ".h"
//#include ".h"

class REMainWnd : public QWidget
{
	Q_OBJECT
public:
	explicit REMainWnd(QWidget *parent = 0);

	typedef enum {
		E_SITE_tlcrealestate = 0,
		E_SITE_loverealestate,
		E_SITE_bigginscott,
		E_SITE_hockingstuart,
		E_SITE_garypeer,
		E_SITE_peterblackshaw,
		E_SITE_realmark,
		E_SITE_wwre,
		E_SITE_thepropertyexchange,
		E_SITE_re88,
		E_SITE_abelmcgrath,
		E_SITE_jennyandrews,
		E_SITE_noeljones,
		E_SITE_barrandstandley,
		E_SITE_doylespillane,
		E_SITE_conduitagencies,
		E_SITE_sheads,
		E_SITE_kevinwrightre,
		E_SITE_rtforsyth,
		E_SITE_blackwaterrealestate,
		E_SITE_laingsimmons,
		E_SITE_starrpartners,
		E_SITE_chadwickrealestate,
		E_SITE_dignam,
		E_SITE_magainagain,
		E_SITE_robertcrawfordrealestate,
		E_SITE_reiwa,
		E_SITE_crawfordrealty,
//		E_SITE_,
//		E_SITE_,
//		E_SITE_,
//		E_SITE_,
//		E_SITE_,
//		E_SITE_,
//		E_SITE_,
//		E_SITE_,
		E_SITE_MAX
	} ENU_SITE;

signals:
	
public slots:
	void sltRun();
	void sltDbClickItem(QListWidgetItem *item);

private:
	void initControls();
	void initSiteList();
	real_estate *getEngine(ENU_SITE aeSite, QString url);

private:
	QListWidget		*mpSiteList;
	real_estate		*mpEngine;
};

#endif // REMAINWND_H
