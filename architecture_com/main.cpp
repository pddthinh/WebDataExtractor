#include <QApplication>
#include <QDesktopWidget>

#include "architecture_gui.h"

int main(int argc, char **argv)
{
	QApplication app(argc, argv);

	architecture_gui gui;
	const QRect screen = QApplication::desktop()->screenGeometry();
	gui.resize(QSize(500, 400));
	gui.move(screen.center() - gui.rect().center());
	gui.showNormal();

	return app.exec();
}

