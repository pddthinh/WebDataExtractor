#include "restaurantcom.h"

#include <QWebElement>
#include <QWebElementCollection>
#include <QMessageBox>
#include <QDebug>


#define BASEURL		"http://www.restaurant.com/search-map.asp?StateID=66&nv=1&rn=AL"
#define BASEROOT	"http://www.restaurant.com/"

RestaurantCom::RestaurantCom(QWidget *apParent)
	:BaseWebEngine(apParent)
{
	setBaseURL(BASEURL);
	setWebLoadTimeout(10000);

	miTotalPage = 0;
	miCurPage = 0;
}

void RestaurantCom::initDatabase()
{

}

void RestaurantCom::doStart()
{
	meState = E_STATE_SUB1;
	loadPage();
}

void RestaurantCom::doStop()
{

}

void RestaurantCom::doWebLoadFinished()
{
	switch(meState)
	{
	case E_STATE_SUB1:
		meState = E_STATE_SUB2;
		mpWebView->doClick("input[alt=\"Find Restaurants\"]");
		break;

	case E_STATE_SUB2:
		meState = E_STATE_GET_MAIN_CATEGORY;
		mpWebView->doClick("#ctl00_ContentPlaceHolder1_ListView1_lstPager1_LinkButton2");
		break;

	case E_STATE_GET_MAIN_CATEGORY:
		getRestaurantList();
		break;

	default:
		break;
	}
}

void RestaurantCom::finish()
{
	QMessageBox lmsg;

	lmsg.setText("Finished");
	DEF_LOG("Finished");
	meState = E_STATE_MAX;

	lmsg.exec();
}

void RestaurantCom::getRestaurantList()
{
	QWebElementCollection	leLst;
	QWebElement				lelement, leA;

	do
	{
		leLst = mpWebView->document().findAll("div[class=\"restaurant-address\"]");
		foreach(lelement, leLst)
		{
			leA = lelement.findFirst("a");
			DEF_CHECK_BREAK(leA, "Can not find title element");

			mlstDataLink << BASEROOT + leA.attribute("href");
		}

		qDebug() << "Got " << mlstDataLink.count();

		if(miTotalPage == 0)
		{
			lelement = mpWebView->getElementById("ctl00_ContentPlaceHolder1_ListView1_lstPager1_DataPager2_ctl00_TotalRowsLabel");
			miTotalPage = lelement.toPlainText().toInt();
			miCurPage = 1;
		}

		if(miCurPage > miTotalPage)
		{
			finish();
			break;
		}

		lelement = mpWebView->getElementById("ctl00_ContentPlaceHolder1_ListView1_lstPager1_DataPager2");
		DEF_CHECK_BREAK(lelement, "Can not get pagination element");

		leLst = lelement.findAll("a");
		foreach(lelement, leLst)
		{
			if(lelement.toPlainText() == QString("%1").arg(miCurPage))
			{
				miCurPage++;
				qDebug() << "Goto page " << miCurPage;
				mpWebView->doClick(&lelement);

				return;
			}
		}

		DEF_LOG_LIST(mlstDataLink, "");
		qDebug() << "END";
	}while(false);
}
