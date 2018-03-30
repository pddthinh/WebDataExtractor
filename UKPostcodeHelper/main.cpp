#include <QApplication>
#include <QDebug>
#include "postcodedb.h"

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	PostcodeDB pcDB(QLatin1String("/home/pddthinh/working/paul/UK-postcode.db"));

	qDebug() << QLatin1String("Running result: ")
			 << pcDB.generateCSVCode2(QLatin1String("pc"), 5);

	return 0;
}
