#include "sgfile.h"
#include "mainwindow.h"
#include <QApplication>

int main(int argc, char **argv) {
	QApplication app(argc, argv);
	MainWindow window;
	QMessageBox::warning(NULL, "Pre-release software", QString("The program "
		"you are running is PRE-RELEASE software. Be aware that there might "
		"be bugs in this program which may cause your computer to crash. "
		"You are running the program at your own risk."));
	window.show();
	return app.exec();
}
