#include <QApplication>

#include "mcsproduct.h"
#include "mcsmainwnd.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	MCSMainWnd mainWnd;
	mainWnd.show();

	return a.exec();
}

