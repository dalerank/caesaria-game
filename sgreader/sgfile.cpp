#include "sgfile.h"
#include "sgimage.h"

#include <QDataStream>
#include <QFile>
#include <QFileInfo>

enum {
	SG_HEADER_SIZE = 680
};

class SgHeader {
public:
	SgHeader(QDataStream *stream) {
		*stream >> sg_filesize;
		*stream >> version;
		*stream >> unknown1;
		*stream >> max_image_records;
		*stream >> num_image_records;
		*stream >> num_bitmap_records;
		*stream >> num_bitmap_records_without_system;
		*stream >> total_filesize;
		*stream >> filesize_555;
		*stream >> filesize_external;
		stream->device()->seek(SG_HEADER_SIZE);
	}
	
	quint32 sg_filesize;
	quint32 version;
	quint32 unknown1;
	qint32 max_image_records;
	qint32 num_image_records;
	qint32 num_bitmap_records;
	qint32 num_bitmap_records_without_system; /* ? */
	quint32 total_filesize;
	quint32 filesize_555;
	quint32 filesize_external;
};

SgFile::SgFile(const QString &filename)
	: header(NULL)
{
	this->filename = filename;
	QFileInfo fi(filename);
	this->basefilename = fi.baseName();
}

SgFile::~SgFile() {
	for (int i = 0; i < bitmaps.size(); i++) {
		delete bitmaps[i];
		bitmaps[i] = 0;
	}
	for (int i = 0; i < images.size(); i++) {
		delete images[i];
		images[i] = 0;
	}
}

int SgFile::bitmapCount() const {
	return bitmaps.size();
}

int SgFile::imageCount(int bitmapId) const {
	if (bitmapId < 0 || bitmapId >= bitmaps.size()) {
		return -1;
	}
	
	return bitmaps[bitmapId]->imageCount();
}

QString SgFile::basename() const {
	return basefilename;
}

int SgFile::totalImageCount() const {
	return images.size();
}

SgImage *SgFile::image(int imageId) const {
	if (imageId < 0 || imageId >= images.size()) {
		return NULL;
	}
	return images[imageId];
}

SgImage *SgFile::image(int bitmapId, int imageId) const {
	if (bitmapId < 0 || bitmapId >= bitmaps.size() ||
		imageId < 0 || imageId >= bitmaps[bitmapId]->imageCount()) {
		return NULL;
	}
	
	return bitmaps[bitmapId]->image(imageId);
}

QImage SgFile::getImage(int imageId) {
	if (imageId < 0 || imageId >= images.size()) {
		qDebug("Id out of range");
		return QImage();
	}
	return images[imageId]->getImage();
}

QImage SgFile::getImage(int bitmapId, int imageId) {
	if (bitmapId < 0 || bitmapId >= bitmaps.size() ||
		imageId < 0 || imageId >= bitmaps[bitmapId]->imageCount()) {
		qDebug("Id out of range");
		return QImage();
	}
	
	return bitmaps[bitmapId]->getImage(imageId);
}

SgBitmap *SgFile::getBitmap(int bitmapId) const {
	if (bitmapId < 0 || bitmapId >= bitmaps.size()) {
		return NULL;
	}
	
	return bitmaps[bitmapId];
}

QString SgFile::getBitmapDescription(int bitmapId) const {
	if (bitmapId < 0 || bitmapId >= bitmaps.size()) {
		return QString();
	}
	
	return bitmaps[bitmapId]->description();
}

QString SgFile::errorMessage(int bitmapId, int imageId) const {
	if (bitmapId < 0 || bitmapId >= bitmaps.size() ||
		imageId < 0 || imageId >= bitmaps[bitmapId]->imageCount()) {
		return QString();
	}
	
	return bitmaps[bitmapId]->errorMessage(imageId);
}

QString SgFile::errorMessage(int imageId) const {
	if (imageId < 0 || imageId >= images.size()) {
		return QString();
	}
	return images[imageId]->errorMessage();
}

bool SgFile::load() {
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly)) {
		qDebug("unable to open file");
		return false;
	}
	
	QDataStream stream(&file);
	stream.setByteOrder(QDataStream::LittleEndian);
	
	header = new SgHeader(&stream);
	
	if (!checkVersion()) {
		return false;
	}
	
	qDebug("Read header, num bitmaps = %d, num images = %d",
		header->num_bitmap_records, header->num_image_records);
	
	loadBitmaps(&stream);
	
	file.seek(SG_HEADER_SIZE +
		maxBitmapRecords() * SgBitmap::RECORD_SIZE);
	
	loadImages(&stream, header->version >= 0xd6);
	
	if (bitmaps.size() > 1 && images.size() == bitmaps[0]->imageCount()) {
		qDebug("SG file has %d bitmaps but only the first is in use",
			bitmaps.size());
		// Remove the bitmaps other than the first
		for (int i = bitmaps.size() - 1; i > 0; i--) {
			SgBitmap *bmp = bitmaps.takeLast();
			delete bmp;
		}
	}
	
	return true;
}

void SgFile::loadBitmaps(QDataStream *stream) {
	
	for (int i = 0; i < header->num_bitmap_records; i++) {
		SgBitmap *bitmap = new SgBitmap(i, filename, stream);
		bitmaps.append(bitmap);
	}
	
}

void SgFile::loadImages(QDataStream *stream, bool includeAlpha) {
	// The first one is a dummy/null record
	SgImage dummy(0, stream, includeAlpha);
	
	for (int i = 0; i < header->num_image_records; i++) {
		SgImage *image = new SgImage(i + 1, stream, includeAlpha);
		qint32 invertOffset = image->invertOffset();
		if (invertOffset < 0 && (i + invertOffset) >= 0) {
			image->setInvertImage(images[i + invertOffset]);
		}
		int bitmapId = image->bitmapId();
		if (bitmapId >= 0 && bitmapId < bitmaps.size()) {
			bitmaps[bitmapId]->addImage(image);
			image->setParent(bitmaps[bitmapId]);
		} else {
			qDebug("Image %d has no parent: %d", i, bitmapId);
		}
		images.append(image);
	}
}

bool SgFile::checkVersion() {
	if (header->version == 0xd3) {
		// SG2 file: filesize = 74480 or 522680 (depending on whether it's
		// a "normal" sg2 or an enemy sg2
		if (header->sg_filesize == 74480 || header->sg_filesize == 522680) {
			return true;
		}
	} else if (header->version == 0xd5 || header->version == 0xd6) {
		// SG3 file: filesize = the actual size of the sg3 file
		QFileInfo fi(filename);
		if (header->sg_filesize == 74480 || fi.size() == header->sg_filesize) {
			return true;
		}
	}
	
	// All other cases:
	return false;
}

int SgFile::maxBitmapRecords() const {
	if (header->version == 0xd3) {
		return 100; // SG2
	} else {
		return 200; // SG3
	}
}
