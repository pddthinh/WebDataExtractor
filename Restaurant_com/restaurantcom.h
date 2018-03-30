#ifndef RESTAURANT_COM_H
#define RESTAURANT_COM_H

#include "../WDELib/WDEInclude.h"

using namespace nsBaseWebEngine;
using namespace nsDatabase;

class RestaurantCom : public BaseWebEngine
{
public:
	RestaurantCom(QWidget *apParent = 0);

protected:
	void doStart();
	void doStop();
	void doWebLoadFinished();
	void initDatabase();

protected:
	void finish();
	void getRestaurantList();

private:
	int		miTotalPage;
	int		miCurPage;
};

#endif // RESTAURANT_COM_H
