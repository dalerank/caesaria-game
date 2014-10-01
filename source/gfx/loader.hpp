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
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

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
    //! based on the file extension (e.g. ".png")
    virtual bool isALoadableFileExtension( const vfs::Path& filename ) const = 0;

    //! returns true if the file maybe is able to be loaded by this class
    virtual bool isALoadableFileFormat( vfs::NFile file ) const = 0;

    //! creates a surface from the file
    virtual gfx::Picture load( vfs::NFile file ) const = 0;
};

class PictureLoader
{
public:
    static PictureLoader& instance();

    gfx::Picture load( vfs::NFile file );

    ~PictureLoader(void);
private:

    PictureLoader(void);

    class Impl;
    ScopedPtr< Impl > _d;
};

#endif //_CAESARIA_PICTURE_LOADER_H_INLCUDE_
