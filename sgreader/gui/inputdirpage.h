#ifndef INPUTDIRPAGE_H
#define INPUTDIRPAGE_H

#include <QWizardPage>

class QLineEdit;

class InputDirPage : public QWizardPage {
	Q_OBJECT
	
	public:
		InputDirPage(QWidget *parent);
		QString inputDir() const;
	
	private slots:
		void browseClicked();
	
	private:
		QLineEdit *inputLineEdit;
};

#endif /* INPUTDIRPAGE_H */
