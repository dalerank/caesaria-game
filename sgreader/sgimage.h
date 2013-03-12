#ifndef SGIMAGE_H
#define SGIMAGE_H

#include "sgbitmap.h"
#include <QImage>

class SgImageRecord;

class SgImage {
	public:
		SgImage(int id, QDataStream *stream, bool includeAlpha);
		~SgImage();
		qint32 invertOffset() const;
		int bitmapId() const;
		QString description() const;
		QString fullDescription() const;
		void setInvertImage(SgImage *invert);
		void setParent(SgBitmap *parent);
		QImage getImage();
		QString errorMessage() const;
		
	private:
		quint8 *fillBuffer();
		/* Image loaders */
		void loadPlainImage(QImage *img, quint8 *buffer);
		void loadIsometricImage(QImage *img, quint8 *buffer);
		void loadSpriteImage(QImage *img, quint8 *buffer);
		void loadAlphaMask(QImage *img, const quint8 *buffer);
		
		/* Image decoding methods */
		void writeIsometricBase(QImage *img, const quint8 *buffer);
		void writeIsometricTile(QImage *img, const quint8 *buffer,
			int offset_x, int offset_y, int tile_width, int tile_height);
		void writeTransparentImage(QImage *img, const quint8 *buffer, int length);
		
		/* Pixel setting */
		void set555Pixel(QImage *img, int x, int y, quint16 color);
		void setAlphaPixel(QImage *img, int x, int y, quint8 color);
		
		/* Error handling */
		void setError(const QString &message);
		
		SgImageRecord *record;
		SgImageRecord *workRecord;
		SgBitmap *parent;
		QString error;
		bool invert;
		int imageId;
};

#endif /* SGIMAGE_H */
