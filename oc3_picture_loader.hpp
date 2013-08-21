#ifndef _OC3_PICTURE_LOADER_H_INLCUDE_
#define _OC3_PICTURE_LOADER_H_INLCUDE_

#include "oc3_picture.hpp"
#include "oc3_scopedptr.hpp"
#include "oc3_filesystem_file.hpp"

//! Class which is able to create a picture(sdl surface) from a file.
class AbstractPictureLoader : public ReferenceCounted
{
public:
    //! returns true if the file maybe is able to be loaded by this class
    //! based on the file extension (e.g. ".tga")
    virtual bool isALoadableFileExtension( const io::FilePath& filename ) const = 0;

    //! returns true if the file maybe is able to be loaded by this class
    virtual bool isALoadableFileFormat( io::NFile file ) const = 0;

    //! creates a surface from the file
    virtual Picture load( io::NFile file ) const = 0;
};

class PictureLoader
{
public:
    static PictureLoader& instance();

    Picture load( io::NFile file );

    ~PictureLoader(void);
private:

    PictureLoader(void);

    class Impl;
    ScopedPtr< Impl > _d;
};

#endif //_OC3_PICTURE_LOADER_H_INLCUDE_
