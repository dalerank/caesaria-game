#ifndef __OPENCAESAR3_PICTURECONVERTER_H_INCLUDE_
#define __OPENCAESAR3_PICTURECONVERTER_H_INCLUDE_

#include <memory>

// class PictureConverter;
// typedef std::auto_ptr< PictureConverter > PictureConverterPtr;

class Picture;

class PictureConverter
{
   // friend PictureConverterPtr;
public:
   // static PictureConverterPtr create();
   static PictureConverter& instance();

   void convToGrayscale( Picture& dst, const Picture& src );
   void rgbBalance( Picture& dst, const Picture& src, int lROffset, int lGOffset, int lBOffset );

private:
   static PictureConverter *_instance;
   PictureConverter();
   ~PictureConverter();
};

#endif
