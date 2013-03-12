#ifndef EXTRACTTHREAD_H
#define EXTRACTTHREAD_H

#include <QThread>
#include <QString>
#include <QStringList>
#include <QDir>

class ExtractThread : public QThread {
	Q_OBJECT
	
	public:
		ExtractThread(const QStringList &files, const QString &outputDir,
			bool extractSystem);
		QStringList errors() const;
		void cancel();
		int errorCount() const;
		int extractCount() const;
	
	signals:
		void progressChanged(int value);
		void fileChanged(const QString &filename, int maxImages);
	
	protected:
		void run();
	
	private:
		void extractFile(const QString &filename);
		
		QStringList errorMessages;
		QStringList files;
		QDir outputDir;
		bool extractSystem;
		bool doCancel;
		int extracted;
		int errorImages;
};

#endif /* EXTRACTTHREAD_H */
