#include <QApplication>
#include "mainwnd.h"

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);

	MainWnd mainWnd;
	mainWnd.show();

	return app.exec();
}
