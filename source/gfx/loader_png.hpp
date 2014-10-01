// this file was created by rt (www.tomkorp.com), based on ttk's png-reader
// i wanted to be able to read in PNG images with CaesarIA :)

#ifndef __CAESARIA_PICTURELOADER_PNG_H_INCLUDED__
#define __CAESARIA_PICTURELOADER_PNG_H_INCLUDED__

#include "loader.hpp"

//!  Surface Loader for PNG files
class PictureLoaderPng : public AbstractPictureLoader
{
public:

   //! returns true if the file maybe is able to be loaded by this class
   //! based on the file extension (e.g. ".png")
   virtual bool isALoadableFileExtension(const vfs::Path& filename) const;

   //! returns true if the file maybe is able to be loaded by this class
   virtual bool isALoadableFileFormat( vfs::NFile file) const;

   //! creates a surface from the file
   virtual gfx::Picture load( vfs::NFile file ) const;
};

#endif //__OC3_PICTURELOADER_PNG_H_INCLUDED__

