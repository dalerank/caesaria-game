#include "filelistpage.h"

#include <QtGui> // TODO: optimize

FileListPage::FileListPage(QWidget *parent)
	: QWizardPage(parent)
{
	setTitle("Choose files to extract");
	
	// Invisible QTextEdit because registerField() requires one
	QTextEdit *dummyField = new QTextEdit();
	dummyField->setVisible(false);
	
	QVBoxLayout *layout = new QVBoxLayout(this);
	
	QLabel *fileLabel = new QLabel(
		tr("Please select the files you'd like to extract. Click an item to"
		" select it, click it again to deselect it."));
	fileLabel->setWordWrap(true);
	
	fileList = new QListWidget(this);
	fileList->setSelectionMode(QAbstractItemView::MultiSelection);
	
	QHBoxLayout *buttons = new QHBoxLayout(this);
	
	QPushButton *allButton = new QPushButton(tr("Select all"));
	QPushButton *noneButton = new QPushButton(tr("Deselect all"));
	
	buttons->addWidget(allButton);
	buttons->addWidget(noneButton);
	buttons->addStretch();
	
	layout->addWidget(fileLabel);
	layout->addWidget(fileList);
	layout->addLayout(buttons);
	setLayout(layout);
	
	connect(allButton, SIGNAL(clicked()), fileList, SLOT(selectAll()));
	connect(noneButton, SIGNAL(clicked()), fileList, SLOT(clearSelection()));
	
	registerField("fileList", dummyField);
}

void FileListPage::initializePage() {
	QString inputDir = field("inputDir").toString();
	
	QDir dir(inputDir);
	setField("inputDir", dir.path()); // Clean up possible end "/"
	
	QStringList filters;
	filters << "*.sg2" << "*.sg3";
	
	QFileInfoList files = dir.entryInfoList(filters, QDir::Files,
		QDir::Name | QDir::IgnoreCase);
	
	fileList->clear();
	for (int i = 0; i < files.size(); i++) {
		fileList->addItem(files.at(i).fileName());
	}
}

bool FileListPage::validatePage() {
	QStringList files;
	QDir inputDir(field("inputDir").toString());
	
	QList<QListWidgetItem *> selection = fileList->selectedItems();
	
	for (int i = 0; i < selection.size(); i++) {
		files << inputDir.filePath(selection[i]->text());
	}
	
	setField("fileList", files);
	
	return true;
}
