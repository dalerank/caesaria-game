#ifndef FILELISTPAGE_H
#define FILELISTPAGE_H

#include <QWizardPage>

class QListWidget;

class FileListPage : public QWizardPage {
	public:
		FileListPage(QWidget *parent);
		void initializePage();
		bool validatePage();
	
	private:
		QListWidget *fileList;
};

#endif /* FILELISTPAGE_H */
