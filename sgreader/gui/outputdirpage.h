#ifndef OUTPUTDIRPAGE_H
#define OUTPUTDIRPAGE_H

#include <QWizardPage>

class QLineEdit;
class QCheckBox;

class OutputDirPage : public QWizardPage {
	Q_OBJECT
	
	public:
		OutputDirPage(QWidget *parent);
		void initializePage();
	
	private slots:
		void browseClicked();
	
	private:
		QLineEdit *outputLineEdit;
		QCheckBox *systemCheckbox;
};

#endif /* OUTPUTDIRPAGE_H */
