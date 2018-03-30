#include <QApplication>
#include "remainwnd.h"
#include <QRect>
#include <QDesktopWidget>

int main(int argc, char **argv)
{
	QApplication app(argc, argv);

	REMainWnd mainWnd;
	const QRect screen = QApplication::desktop()->screenGeometry();
	mainWnd.resize(QSize(500, 400));
	mainWnd.move(screen.center() - mainWnd.rect().center());
	mainWnd.showNormal();

	return app.exec();
}
