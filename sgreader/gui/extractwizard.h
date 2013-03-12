#ifndef EXTRACTWIZARD_H
#define EXTRACTWIZARD_H

#include <QWizard>
#include <QString>
#include <QStringList>

class QStackedWidget;
class QPushButton;

class InputDirPage;
class OutputDirPage;
class FileListPage;
class ProgressPage;

class ExtractWizard : public QWizard {
	Q_OBJECT
	
	public:
		ExtractWizard(QWidget *parent = 0);
		~ExtractWizard();
	
	protected:
		void closeEvent(QCloseEvent *event);
	
	private:
		InputDirPage *inputPage;
		FileListPage *filePage;
		OutputDirPage *outputPage;
		ProgressPage *progressPage;
		
		QStackedWidget *stackWidget;
		QPushButton *nextButton;
		QPushButton *prevButton;
		QPushButton *cancelButton;
		
		QString inputFolder;
		QString outputFolder;
		QStringList files;
};

#endif /* EXTRACTWIZARD_H */
