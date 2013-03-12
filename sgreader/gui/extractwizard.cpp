#include "extractwizard.h"

#include "inputdirpage.h"
#include "filelistpage.h"
#include "outputdirpage.h"
#include "progresspage.h"

#include <QtGui> // TODO: specify

ExtractWizard::ExtractWizard(QWidget *parent)
	: QWizard(parent)
{
	setWindowTitle("Extract files wizard");
	
	addPage(new InputDirPage(this));
	addPage(new FileListPage(this));
	addPage(new OutputDirPage(this));
	addPage(new ProgressPage(this));
	
	setOptions(QWizard::DisabledBackButtonOnLastPage);
	setWizardStyle(QWizard::ClassicStyle);
	
	QList<QWizard::WizardButton> layout;
	layout << QWizard::CancelButton << QWizard::Stretch << QWizard::BackButton
		<< QWizard::NextButton << QWizard::FinishButton;
	setButtonLayout(layout);
}

ExtractWizard::~ExtractWizard() {
	
}

void ExtractWizard::closeEvent(QCloseEvent *event) {
	ProgressPage *page = qobject_cast<ProgressPage *>(currentPage());
	
	qDebug("Close called on dialog");
	if (page && page->isBusy()) {
		event->ignore();
	} else {
		event->accept();
	}
}

