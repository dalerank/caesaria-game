#include "sgfile.h"
#include "mainwindow.h"

#include <iostream>

#include <QApplication>
#include <QCoreApplication>

#include <QxtCommandOptions>

#include "gui/extractthread.h"

class ConsoleWorker : public QObject 
{
    Q_OBJECT
public:
    explicit ConsoleWorker(QObject *parent = 0) : QObject(parent) {}
public slots:
    void run();
    void threadFinished();
    void fileChanged(const QString&, int);
    void progressChanged(int);
signals:
    void finished();
public:
    void SetInputPath(QString ip)  {InputPath = ip;}
    void SetOutputPath(QString op) {OutputPath = op;}
    void SetSystem(bool sys) {system = sys;}
private:
    QString InputPath;
    QString OutputPath;
    bool system;
};

void ConsoleWorker::run()
{
    // here goes the work
    
    QStringList files;
    
    files << InputPath;
    
    ExtractThread *thread = new ExtractThread(files, OutputPath, system);

    connect(thread, SIGNAL(finished()), this, SLOT(threadFinished()));
    connect(thread, SIGNAL(fileChanged(const QString&, int)), this, SLOT(fileChanged(const QString&, int)));
    connect(thread, SIGNAL(progressChanged(int)), this, SLOT(progressChanged(int)));
    
    thread->start();
}

void ConsoleWorker::threadFinished()
{
    emit finished();
}

void ConsoleWorker::fileChanged(const QString &filename, int numFiles)
{
    std::cout << "Processing " << filename.toStdString() << " from " << numFiles << std::endl;
}

void ConsoleWorker::progressChanged(int image)
{
    std::cout << "Processing " << image << std::endl;
}

#include "main.moc"

int startGUIApp(int argc, char **argv)
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

int main(int argc, char **argv) {
	QxtCommandOptions options;
	
//	options.add("no-gui", "Runs program in console mode");
	options.add("input", "Sets input filename", QxtCommandOptions::ValueRequired);
	options.alias("input", "i");
	options.add("output", "Sets directory for output files. Default directory is current", QxtCommandOptions::ValueRequired);
	options.alias("output","o");
	options.add("system", "Extracts system images from SG2/SG3", QxtCommandOptions::NoValue);
	options.alias("system","s");
	options.add("help", "Show this help text");
	options.alias("help","h");
	options.parse(argc, argv);

	if (options.count("help") || options.showUnrecognizedWarning())
	{
	    std::cout << "SGReader v.1.1 (2013-03-25)" << std:: endl <<
 	      "Read graphics files (*.sg2 and *.sg3) from Impressions citybuilding games" << std::endl <<
 	      "Usage: "<< argv[0] << " KEYS" << std::endl << std::endl <<
 	      "Available keys:" << std::endl;
	    options.showUsage();
	    return -1;
	}
	
	bool isinput  = options.count("input");
	bool isoutput = options.count("output");  
	bool issystem = options.count("system");  

	if (!isinput && !isoutput)
	{	
	    return startGUIApp(argc, argv);
	}
	else
	{
	    QCoreApplication app(argc, argv);
	    ConsoleWorker *cw = new ConsoleWorker(&app);
	    
	    QString InputPath = options.value("input").toString();
	    QString OutputPath = ".";
	    
	    if (isoutput) {OutputPath = options.value("output").toString();}
	    
	    cw->SetInputPath(InputPath);
	    cw->SetOutputPath(OutputPath);
	    cw->SetSystem(issystem);
	    
	    QObject::connect(cw, SIGNAL(finished()), &app, SLOT(quit()));
	    QTimer::singleShot(0, cw, SLOT(run()));
	    
	    return app.exec();	
	}
	return 0;
}
