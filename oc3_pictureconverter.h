#ifndef _OPENCAESAR3_PICTURECONVERTER_H_INCLUDE_
#define _OPENCAESAR3_PICTURECONVERTER_H_INCLUDE_

#include <memory>

class PictureConverter;
typedef std::auto_ptr< PictureConverter > PictureConverterPtr;

class Picture;

class PictureConverter
{
    friend PictureConverterPtr;
public:
    static PictureConverterPtr create();

    void convToGrayscale( Picture& dst, const Picture& src );
    void rgbBalance( Picture& dst, const Picture& src, int lROffset, int lGOffset, int lBOffset );

private:
    PictureConverter();
    ~PictureConverter();
};

#endif