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
// You should have received a createCopy of the GNU General Public License
// along with CaesarIA.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com

#include "picture_bank.hpp"

#include <cstdlib>
#include <string>
#include <memory>
#include <sys/stat.h>
#include <set>
#include <SDL.h>

#include "core/variant_map.hpp"
#include "core/position.hpp"
#include "core/exception.hpp"
#include "game/resourcegroup.hpp"
#include "gfx/animation.hpp"
#include "game/settings.hpp"
#include "core/utils.hpp"
#include "core/logger.hpp"
#include "picture_info_bank.hpp"
#include "core/foreach.hpp"
#include "loader.hpp"
#include "core/saveadapter.hpp"
#include "vfs/file.hpp"
#include "gfx/helper.hpp"
#include "core/color.hpp"

using namespace gfx;

namespace {
const char* framesSection = "frames";
}

struct AtlasPreview
{
  std::string filename;
  std::set<unsigned int> images;

  inline bool find( unsigned int hash ) { return images.count( hash ) > 0; }
};

class PictureBank::Impl
{
public:
  typedef std::map<unsigned int, Picture> CachedPictures;
  typedef std::vector< AtlasPreview > AtlasPreviews;
  typedef std::map<SDL_Texture*, int> TextureCounter;
  typedef CachedPictures::iterator ItPicture;

  AtlasPreviews atlases;
  StringArray picExentions;
  TextureCounter txCounters;
  CachedPictures resources;  // key=image name, value=picture

public:
  Picture tryLoadPicture( const std::string& name );
  void loadAtlas(const vfs::Path& filename );
  void setPicture( const std::string &name, const Picture& pic );
  void destroyUnusableTextures();
};

void PictureBank::Impl::setPicture( const std::string &name, const Picture& pic )
{
  int dot_pos = name.find('.');
  std::string rcname = name.substr(0, dot_pos);

  // first: we deallocate the current picture, if any
  unsigned int picId = utils::hash( rcname );
  Picture* ptrPic = 0;
  Impl::ItPicture it = resources.find( picId );
  if( it != resources.end() )
  {
    //SDL_DestroyTexture( it->second.texture() );
    if( it->second.texture() > 0 )
      txCounters[ it->second.texture() ]--;

    ptrPic = &it->second;
  }
  else
  {
    resources[ picId ] = Picture();
    ptrPic = &resources[ picId ];
  }

  *ptrPic = pic;
  if( pic.texture() > 0 )
    txCounters[ pic.texture() ]++;

  Point offset( 0, 0 );

  // decode the picture name => to set the offset manually
  Point pic_info = PictureInfoBank::instance().getOffset( rcname );

  if( pic_info == Point( -1, -1 ) )
  {
    // this is a tiled picture=> automatic offset correction
    int cw = gfx::tilemap::cellSize().width() * 2;
    int ch = gfx::tilemap::cellSize().width() / 2;
    offset.setY( pic.height()-ch*( (pic.width()+2)/cw ) );   // (w+2)/60 is the size of the tile: (1x1, 2x2, 3x3, ...)
  }
  else if( pic_info == Point( -2, -2 ) )
  {
     // this is a walker picture=> automatic offset correction
     offset = Point( -pic.width()/2, int(pic.height()*3./4.) );
  }
  else
  {
     offset = pic_info;
  }

  ptrPic->setOffset( offset );
  ptrPic->setName( rcname );
}

void PictureBank::Impl::destroyUnusableTextures()
{
  for( TextureCounter::iterator it=txCounters.begin(); it != txCounters.end(); )
  {
    if( it->second <= 0 )
    {
      SDL_DestroyTexture( it->first );
      txCounters.erase( it++ );
    }
    else
    {
      ++it;
    }
  }
}

PictureBank& PictureBank::instance()
{
  static PictureBank inst; 
  return inst;
}

void PictureBank::reset()
{

}

void PictureBank::setPicture( const std::string &name, const Picture& pic )
{
  _d->setPicture( name, pic );
}

void PictureBank::addAtlas( const std::string& filename )
{
  VariantMap options = config::load( filename );
  if( !options.empty() )
  {
    Logger::warning( "PictureBank: load atlas " + filename );

    AtlasPreview atlas;
    atlas.filename = filename;

    VariantMap items = options.get( framesSection ).toMap();
    foreach( i, items )
    {
      unsigned int hash = utils::hash( i->first );
      atlas.images.insert( hash );
    }

    _d->atlases.push_back( atlas );
  }
}

void PictureBank::loadAtlas(const std::string& filename)
{
  _d->loadAtlas( filename );
}

Picture& PictureBank::getPicture(const std::string &name)
{
  const unsigned int hash = utils::hash( name );
  //Logger::warning( "PictureBank getpic " + name );

  Impl::ItPicture it = _d->resources.find( hash );
  if( it == _d->resources.end() )
  {
    //can't find image in valid resources, try load from hdd
    const Picture& pic = _d->tryLoadPicture( name );

    if( pic.isValid() ) { setPicture( name, pic );  }
    else{ _d->resources[ hash ] = pic; }

    return _d->resources[ hash ];
  }
  return it->second;
}

Picture& PictureBank::getPicture(const std::string& prefix, const int idx)
{
  std::string resource_name = utils::format( 0xff, "%s_%05d", prefix.c_str(), idx );

  return getPicture(resource_name);
}

bool PictureBank::present(const std::string& prefix, const int idx) const
{
  return false;
}

PictureBank::PictureBank() : _d( new Impl )
{
  _d->picExentions << ".png";
  _d->picExentions << ".bmp";
}

PictureBank::~PictureBank(){}


Picture PictureBank::Impl::tryLoadPicture(const std::string& name)
{
  vfs::Path realPath( name );  

  bool fileExist = false;
  if( realPath.extension().empty() )
  {
    foreach( itExt, picExentions )
    {
      realPath = name + *itExt;

      if( realPath.exist() )
      {
        fileExist = true;
        break;
      }
    }
  }

  if( fileExist )
  {    
    vfs::NFile file = vfs::NFile::open( realPath );
    if( file.isOpen() )
    {
      Picture ret = PictureLoader::instance().load( file );
      return ret;
    }
  }

  unsigned int hash = utils::hash( name );
  foreach( i, atlases )
  {
    bool found = i->find( hash );
    if( found )
    {
      loadAtlas( (*i).filename );
      //unloadAtlas.erase( i );
      break;
    }
  }

  CachedPictures::iterator it = resources.find( hash );
  if( it != resources.end() )
  {
    return it->second;
  }

  Logger::warning( "PictureBank: Unknown resource %s", name.c_str() );
  return Picture::getInvalid();
}

void PictureBank::Impl::loadAtlas(const vfs::Path& filePath)
{
  if( !filePath.exist() )
  {
    Logger::warning( "PictureBank: cant find atlas " + filePath.toString() );
    return;
  }

  VariantMap info = config::load( filePath );

  vfs::Path texturePath = info.get( "texture" ).toString();

  vfs::NFile file = vfs::NFile::open( texturePath );

  Picture mainTexture;
  if( file.isOpen() )
  {
    mainTexture = PictureLoader::instance().load( file );
  }
  else
  {
    Logger::warning( "PictureBank: load atlas failed for texture" + texturePath.toString() );
    mainTexture = Picture::getInvalid();
  }

  //SizeF mainRectSize = mainTexture.size().toSizeF();
  if( !info.empty() )
  {
    VariantMap items = info.get( framesSection ).toMap();
    foreach( i, items )
    {
      VariantList rInfo = i->second.toList();
      Picture pic = mainTexture;
      Point start( rInfo.get( 0 ).toInt(), rInfo.get( 1 ).toInt() );
      Size size( rInfo.get( 2 ).toInt(), rInfo.get( 3 ).toInt() );

      Rect orect( start, size );
      pic.setOriginRect( orect );
      //pic.setOriginRectf( );
      setPicture( i->first, pic );
    }
  }
}
