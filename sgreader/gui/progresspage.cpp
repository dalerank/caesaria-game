#include "progresspage.h"

#include "extractthread.h"

#include <QVBoxLayout>
#include <QVariant>
#include <QAbstractButton>
#include <QPushButton>

#include <QDebug>

ProgressPage::ProgressPage(QWidget *parent)
	: QWizardPage(parent), thread(NULL)
{
	setTitle("Extracting images... please wait");
	setButtonText(QWizard::FinishButton, tr("&Close"));
	
	QVBoxLayout *layout = new QVBoxLayout(this);
	
	fileProgress = new QProgressBar(this);
	imageProgress = new QProgressBar(this);
	
	fileLabel = new QLabel(this);
	imageLabel = new QLabel(this);
	
	cancelButton = new QPushButton(tr("Cancel extracting"));
	
	errorTextEdit = new QTextEdit(this);
	errorTextEdit->setVisible(false);
	
	layout->addWidget(fileLabel);
	layout->addWidget(fileProgress);
	layout->addSpacing(20);
	layout->addWidget(imageLabel);
	layout->addWidget(imageProgress);
	layout->addWidget(errorTextEdit);
	layout->addStretch();
	layout->addWidget(cancelButton);
	
	setLayout(layout);
	
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancelThread()));
}

void ProgressPage::initializePage() {
	
	// Disable "cancel" button
	wizard()->button(QWizard::CancelButton)->setVisible(false);
	
	QStringList files = field("fileList").toStringList();
	QString outputDir = field("outputDir").toString();
	bool system = field("extractSystem").toBool();
	
	fileProgress->setMaximum(files.size());
	
	thread = new ExtractThread(files, outputDir, system);
	
	connect(thread, SIGNAL(finished()), this, SLOT(threadFinished()));
	connect(thread, SIGNAL(fileChanged(const QString&, int)),
	        this, SLOT(fileChanged(const QString&, int)));
	connect(thread, SIGNAL(progressChanged(int)),
	        this, SLOT(progressChanged(int)));
	
	thread->start();
}

bool ProgressPage::isComplete() const {
	if (thread && thread->isFinished()) {
		return true;
	}
	return false;
}

bool ProgressPage::isBusy() const {
	return (thread && thread->isRunning());
}

void ProgressPage::threadFinished() {
	qDebug("Thread finished");
	
	setTitle("Images extracted");
	fileLabel->setText(QString("Done. %0 images were extracted, %1 images were skipped (see below)")
		.arg(thread->extractCount())
		.arg(thread->errorCount()));
	//fileProgress->setValue(fileProgress->value() + 1);
	
	fileProgress->setVisible(false);
	imageProgress->setVisible(false);
	cancelButton->setVisible(false);
	
	imageLabel->setText("Error messages:");
	QStringList errors = thread->errors();
	if (errors.size()) {
		errorTextEdit->setPlainText(errors.join("\r\n"));
	} else {
		errorTextEdit->setPlainText("No errors.");
	}
	errorTextEdit->setVisible(true);
	
	emit completeChanged();
}

void ProgressPage::progressChanged(int image) {
	imageLabel->setText(QString("Extracting image %0 of %1")
		.arg(image)
		.arg(imageProgress->maximum()));
	imageProgress->setValue(image);
}

void ProgressPage::fileChanged(const QString &filename, int numFiles) {
	int fileid = fileProgress->value() + 1;
	imageProgress->reset();
	imageProgress->setMaximum(numFiles);
	fileLabel->setText(QString("Processing file %0 of %1: %2")
		.arg(fileid + 1)
		.arg(fileProgress->maximum())
		.arg(filename));
	fileProgress->setValue(fileid);
}

void ProgressPage::cancelThread() {
	if (thread) {
		cancelButton->setEnabled(false);
		cancelButton->setText("Cancelling...");
		thread->cancel();
	}
}
