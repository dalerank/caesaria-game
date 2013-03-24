#include "sgfile.h"
#include "mainwindow.h"

#include <iostream>

#include <QApplication>
#include <QCoreApplication>

#include <QxtCommandOptions>

class ConsoleWorker : public QObject 
{
    Q_OBJECT
public:
    explicit ConsoleWorker(QObject *parent = 0) : QObject(parent) {}
public slots:
    void run() {std::cout << "Hello" << std::endl; emit finished();}
signals:
    void finished();
};

#include "main.moc"

int main(int argc, char **argv) {
  
	QxtCommandOptions options;
	
	options.add("no-gui", "runs program in console mode");
	options.add("input", "sets directory where input files are located", QxtCommandOptions::ValueRequired);
	options.alias("input", "i");
	options.add("output", "sets directory for output files", QxtCommandOptions::ValueRequired);
	options.alias("output","o");
	options.add("help", "show this help text");
	options.alias("help","h");
	options.parse(argc, argv);

	if (options.count("help") || options.showUnrecognizedWarning())
	{
	    options.showUsage();
	    return -1;
	}
	
	bool isinput = options.count("input");
	bool isoutput = options.count("output");  

	if (!isinput && !isoutput)
	{	
	    QApplication app(argc, argv);
	    MainWindow window;
	    QMessageBox::warning(NULL, "Pre-release software", QString("The program "
		"you are running is PRE-RELEASE software. Be aware that there might "
		"be bugs in this program which may cause your computer to crash. "
		"You are running the program at your own risk."));
	    window.show();
	    return app.exec();
	}
	else
	{
	    QCoreApplication app(argc, argv);
	    ConsoleWorker *cw = new ConsoleWorker(&app);
	    
	    QObject::connect(cw, SIGNAL(finished()), &app, SLOT(quit()));
	    QTimer::singleShot(0, cw, SLOT(run()));
	    
	    return app.exec();	
	}
	return 0;
}
