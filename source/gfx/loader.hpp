#ifndef _CAESARIA_PICTURE_LOADER_H_INLCUDE_
#define _CAESARIA_PICTURE_LOADER_H_INLCUDE_

#include "picture.hpp"
#include "core/scopedptr.hpp"
#include "vfs/file.hpp"

//! Class which is able to create a picture(sdl surface) from a file.
class AbstractPictureLoader : public ReferenceCounted
{
public:
    //! returns true if the file maybe is able to be loaded by this class
    //! based on the file extension (e.g. ".tga")
    virtual bool isALoadableFileExtension( const vfs::Path& filename ) const = 0;

    //! returns true if the file maybe is able to be loaded by this class
    virtual bool isALoadableFileFormat( vfs::NFile file ) const = 0;

    //! creates a surface from the file
    virtual Picture load( vfs::NFile file ) const = 0;
};

class PictureLoader
{
public:
    static PictureLoader& instance();

    Picture load( vfs::NFile file );

    ~PictureLoader(void);
private:

    PictureLoader(void);

    class Impl;
    ScopedPtr< Impl > _d;
};

#endif //_CAESARIA_PICTURE_LOADER_H_INLCUDE_
