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

#include "loader.hpp"
#include "loader_png.hpp"
#include "loader_bmp.hpp"
#include "core/foreach.hpp"
#include "core/logger.hpp"
#include "vfs/path.hpp"
#include "core/time.hpp"

using namespace gfx;

class PictureLoader::Impl
{
public:
  void initLoaders();

  typedef std::vector< AbstractPictureLoader* > Loaders;
  typedef Loaders::iterator LoaderIterator;
  Loaders loaders;
};

PictureLoader::PictureLoader(void) : _d( new Impl )
{
  _d->initLoaders();
}

void PictureLoader::Impl::initLoaders()
{
  loaders.push_back( new PictureLoaderPng() );
  loaders.push_back( new PictureLoaderBmp() );
  //_d->loaders.push_back( new PixmapLoaderPsd() );
  //_d->loaders.push_back( new PixmapLoaderJpeg() );
}

PictureLoader::~PictureLoader(void){}

PictureLoader& PictureLoader::instance()
{
  static PictureLoader instanceLoader;
  return instanceLoader;
}

Picture PictureLoader::load( vfs::NFile file )
{
  if( !file.isOpen() )
     return Picture::getInvalid();

  // try to load file based on file extension
  foreach( loader, _d->loaders )
  {
    if( (*loader)->isALoadableFileExtension(file.path()) )
    {
      // reset file position which might have changed due to previous loadImage calls
      file.seek(0);
      Picture ret = (*loader)->load( file );
      return ret;
    }
    else
    {
      Logger::warning( "WARNING !!!: PictureLoader have unknown extension with " + file.path().absolutePath().toString() );
      bool isMyFormat = (*loader)->isALoadableFileFormat(file);
      if( isMyFormat )
      {
         file.seek(0);
        return (*loader)->load( file );
      }
    }
  }

  return Picture::getInvalid(); // failed to load
}
