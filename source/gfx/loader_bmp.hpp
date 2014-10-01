// This file is part of CaesarIA.
//
// CaesarIA is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CaesarIA is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CaesarIA.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_PICTURELOADER_BMP_H_INCLUDED__
#define __CAESARIA_PICTURELOADER_BMP_H_INCLUDED__

#include "loader.hpp"

//!  Surface Loader for PNG files
class PictureLoaderBmp : public AbstractPictureLoader
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

#endif //__CAESARIA_PICTURELOADER_BMP_H_INCLUDED__

