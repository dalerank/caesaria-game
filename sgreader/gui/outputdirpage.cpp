#include "outputdirpage.h"

#include <QtGui> // todo: optimize

OutputDirPage::OutputDirPage(QWidget *parent)
	: QWizardPage(parent)
{
	setTitle("Set the output folder");
	setButtonText(QWizard::NextButton, tr("&Extract"));
	
	QLabel *outputDescription = new QLabel(
		tr("Choose the folder where the extracted images will be stored. "
		"For each SG2/SG3 file, a new folder will be created inside the folder"
		" you choose here."));
	outputDescription->setWordWrap(true);
	
	QVBoxLayout *layout = new QVBoxLayout();
	QHBoxLayout *dirlayout = new QHBoxLayout();
	
	QLabel *outputLabel = new QLabel(tr("Output folder:"));
	outputLineEdit = new QLineEdit();
	QPushButton *browseButton = new QPushButton(tr("Browse..."));
	
	connect(browseButton, SIGNAL(clicked()), this, SLOT(browseClicked()));
	
	QLabel *systemDescription = new QLabel(
		tr("System files are 200 images present in most of the SG2/SG3 files."
		" Usually they contain no images of interest, and since they're present"
		" (and the same) in most SG2/SG3 files, you get duplicate images."));
	systemDescription->setWordWrap(true);
	systemCheckbox = new QCheckBox(tr("Extract system files"));
	
	QLabel *extractLabel = new QLabel(
		tr("Press 'Extract' to start extracting the images."));
	
	dirlayout->addWidget(outputLabel);
	dirlayout->addWidget(outputLineEdit);
	dirlayout->addWidget(browseButton);
	
	layout->addWidget(outputDescription);
	layout->addLayout(dirlayout);
	layout->addSpacing(20);
	layout->addWidget(systemDescription);
	layout->addWidget(systemCheckbox);
	layout->addStretch();
	layout->addWidget(extractLabel);
	
	setLayout(layout);
	
	registerField("outputDir*", outputLineEdit);
	registerField("extractSystem", systemCheckbox);
}

void OutputDirPage::browseClicked() {
	QString dirname = QFileDialog::getExistingDirectory(this,
		tr("Choose the folder where you'd like to store the images"), outputLineEdit->text());
	
	if (!dirname.isEmpty()) {
		outputLineEdit->setText(dirname);
	}
}

void OutputDirPage::initializePage() {
	outputLineEdit->setText(field("inputDir").toString());
}
