#include <QApplication>
#include "restaurantcom.h"

int main(int argc, char **argv)
{
	QApplication lapp(argc, argv);

	RestaurantCom lres;
	lres.show();

	return lapp.exec();
}
