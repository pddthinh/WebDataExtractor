#include <QApplication>
#include "TestContactSeeker.h"

int main(int argc, char **argv)
{
	QApplication lapp(argc, argv);

	TestContactSeeker seeker;
	seeker.show();

	return lapp.exec();
}
