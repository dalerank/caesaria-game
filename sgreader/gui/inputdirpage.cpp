#include "inputdirpage.h"

#include <QtGui> // TODO: specify

InputDirPage::InputDirPage(QWidget *parent)
	: QWizardPage(parent)
{
	setTitle("Select the input folder");
	
	QLabel *inputDescription = new QLabel(
		tr("Choose the folder where the SG2/SG3 files are located that you wish"
		" to extract.\n\nFor Caesar 3, this is the installation folder, for"
		" the other Citybuilding games it's install_dir\\Data."));
	inputDescription->setWordWrap(true);
	
	QVBoxLayout *boxlayout = new QVBoxLayout();
	QHBoxLayout *dirlayout = new QHBoxLayout();
	
	QLabel *inputLabel = new QLabel(tr("Input folder:"));
	inputLineEdit = new QLineEdit();
	QPushButton *browseButton = new QPushButton(tr("Browse..."));
	
	connect(browseButton, SIGNAL(clicked()), this, SLOT(browseClicked()));
	
	dirlayout->addWidget(inputLabel);
	dirlayout->addWidget(inputLineEdit);
	dirlayout->addWidget(browseButton);
	
	boxlayout->addWidget(inputDescription);
	boxlayout->addLayout(dirlayout);
	boxlayout->addStretch();
	
	setLayout(boxlayout);
	
	registerField("inputDir*", inputLineEdit);
}

void InputDirPage::browseClicked() {
	QString dirname = QFileDialog::getExistingDirectory(this,
		tr("Choose the folder where the .sg2/.sg3 files are located"), inputLineEdit->text());
	
	if (!dirname.isEmpty()) {
		inputLineEdit->setText(dirname);
	}
}
