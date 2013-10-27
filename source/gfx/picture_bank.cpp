// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a createCopy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com

#include "picture_bank.hpp"

#include <cstdlib>
#include <string>
#include <memory>
#include <sys/stat.h>
#include <map>
#include <SDL.h>

#include "core/position.hpp"
#include "core/exception.hpp"
#include "game/resourcegroup.hpp"
#include "gfx/animation.hpp"
#include "game/settings.hpp"
#include "core/stringhelper.hpp"
#include "picture_info_bank.hpp"
#include "engine.hpp"
#include "loader.hpp"
#include "vfs/file.hpp"

class PictureBank::Impl
{
public:
  typedef std::map< unsigned int, Picture> Pictures;
  typedef Pictures::iterator ItPicture;

  Pictures resources;  // key=image name, value=picture
};

PictureBank& PictureBank::instance()
{
  static PictureBank inst; 
  return inst;
}

void PictureBank::setPicture(const std::string &name, SDL_Surface &surface)
{
  // first: we deallocate the current picture, if any
  unsigned int picId = StringHelper::hash( name );
  Impl::ItPicture it = _d->resources.find( picId );
  if( it != _d->resources.end() )
  {
     SDL_FreeSurface( it->second.getSurface());
  }

  _d->resources[ picId ] = makePicture(&surface, name);
}

void PictureBank::setPicture( const std::string &name, const Picture& pic )
{
  setPicture( name, *pic.getSurface() );
}

Picture& PictureBank::getPicture(const std::string &name)
{
  const unsigned int hash = StringHelper::hash( name );
  Impl::ItPicture it = _d->resources.find( hash );
  if( it == _d->resources.end() )
  {
    //can't find image in valid resources, try load from hdd
    io::NFile file = io::NFile::open( name );

    if( file.isOpen() )
    {
      Picture tmpPicture = PictureLoader::instance().load( file );
      setPicture( name, tmpPicture );

      return _d->resources[ hash ];
    }
    else
    {
      StringHelper::debug( 0xff, "Unknown resource %s", name.c_str() );
      _d->resources[ hash ] = Picture::getInvalid();
      return _d->resources[ hash ];
    }
  }

  return it->second;
}

Picture& PictureBank::getPicture(const std::string &prefix, const int idx)
{
   std::string resource_name = StringHelper::format( 0xff, "%s_%05d.png", prefix.c_str(),idx );

   return getPicture(resource_name);
}

Picture PictureBank::makePicture(SDL_Surface *surface, const std::string& resource_name) const
{
   Point offset( 0, 0 );
   // decode the picture name => to set the offset manually
   // resource_name = "buildings/Govt_00005.png"
   int dot_pos = resource_name.find('.');
   std::string filename = resource_name.substr(0, dot_pos);

   Point pic_info = PictureInfoBank::instance().getOffset(filename);

   if (pic_info.getX() == -1 && pic_info.getY() == -1)
   {
      // this is a tiled picture=> automatic offset correction
      offset.setY( surface->h-15*((surface->w+2)/60) );   // (w+2)/60 is the size of the tile: (1x1, 2x2, 3x3, ...)
   }
   else if (pic_info.getX() == -2 && pic_info.getY() == -2)
   {
      // this is a walker picture=> automatic offset correction
      offset = Point( -surface->w/2, int(surface->h*3./4.) );
   }
   else
   {
      offset = pic_info;
   }

   Picture pic;
   pic.init( surface, offset );
   pic.setName(filename);

   return pic;
}

void PictureBank::createResources()
{
  Picture& originalPic = getPicture( ResourceGroup::utilitya, 34 );
  setPicture( std::string( ResourceGroup::waterbuildings ) + "_00001.png", *originalPic.getSurface() );

  Picture* fullReservoir = originalPic.createCopy(); //mem leak on destroy picloader
  fullReservoir->draw( getPicture( ResourceGroup::utilitya, 35 ), 47, 37 );
  setPicture( std::string( ResourceGroup::waterbuildings ) + "_00002.png", *fullReservoir->getSurface() );

  Picture& emptyFontainOrig = getPicture( ResourceGroup::utilitya, 10 );
  setPicture( std::string( ResourceGroup::waterbuildings ) + "_00003.png", *emptyFontainOrig.getSurface() );

  Picture* fullFontain = emptyFontainOrig.createCopy();  //mem leak on destroy picloader
  fullFontain->draw( getPicture( ResourceGroup::utilitya, 11 ), 12, 25 );
  setPicture( std::string( ResourceGroup::waterbuildings) + "_00004.png", *fullFontain->getSurface() );
}

PictureBank::PictureBank() : _d( new Impl )
{

}

PictureBank::~PictureBank()
{

}
