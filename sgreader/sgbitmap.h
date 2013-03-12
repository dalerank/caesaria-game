#ifndef SGBITMAP_H
#define SGBITMAP_H

#include <QImage>
#include <QList>
#include <QString>
#include <QDir>

class QDataStream;
class QFile;
class SgBitmapRecord;
class SgImage;

class SgBitmap {
	public:
		SgBitmap(int id, const QString &sgFilename, QDataStream *stream);
		~SgBitmap();
		int imageCount() const;
		QString description() const;
		QString bitmapName() const;
		QImage getImage(int id);
		SgImage *image(int id);
		void addImage(SgImage *child);
		QFile *openFile(char isExtern);
		QString errorMessage(int id) const;
		
		enum {
			RECORD_SIZE = 200
		};
	private:
		QString find555File();
		QString findFilenameCaseInsensitive(QDir directory, QString filename);
		
		QList<SgImage*> images;
		SgBitmapRecord *record;
		QFile *file;
		QString sgFilename;
		int bitmapId;
		char isExtern;
};

#endif /* SGBITMAP_H */
