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

#include "oc3_picture_bank.hpp"

#include <cstdlib>
#include <string>
#include <memory>
#include <archive.h>
#include <archive_entry.h>
#include <sys/stat.h>
#include <map>
#include <SDL.h>
#include <SDL_image.h>

#include "oc3_positioni.hpp"
#include "oc3_exception.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_animation.hpp"
#include "oc3_app_config.hpp"
#include "oc3_stringhelper.hpp"
#include "oc3_picture_info_bank.hpp"

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
  if (it != _d->resources.end())
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
  Impl::ItPicture it = _d->resources.find( StringHelper::hash( name ) );
  if (it == _d->resources.end()) 
  {
    StringHelper::debug( 0xff, "Unknown resource %s", name.c_str() );
  }

  return it->second;
}

Picture& PictureBank::getPicture(const std::string &prefix, const int idx)
{
   std::string resource_name = StringHelper::format( 0xff, "%s_%05d.png", prefix.c_str(),idx );

   return getPicture(resource_name);
}

PicturesArray PictureBank::getPictures()
{
   PicturesArray pictures;
   for( Impl::ItPicture it = _d->resources.begin(); it != _d->resources.end(); ++it)
   {
      // for every resource
      pictures.push_back(&(*it).second);
   }
   return pictures;
}

void PictureBank::loadWaitPics()
{
  std::string aPath = AppConfig::get( AppConfig::resourcePath ).toString() + "/pics/";
  loadArchive(aPath+"pics_wait.zip");

  StringHelper::debug( 0xff, "number of images loaded: %d", _d->resources.size() );
}

void PictureBank::loadAllPics()
{
  std::string aPath = AppConfig::get( AppConfig::resourcePath ).toString() + "/pics/";
  loadArchive(aPath + "pics.zip");
  loadArchive(aPath + "pics_oc3.zip");	
  StringHelper::debug( 0xff, "number of images loaded: %d", _d->resources.size() );
}

void PictureBank::loadArchive(const std::string &filename)
{
  std::cout << "reading image archive: " << filename << std::endl;
  struct archive *a;
  struct archive_entry *entry;
  int rc;

  a = archive_read_new();
  archive_read_support_compression_all(a);
  archive_read_support_format_all(a);
  rc = archive_read_open_filename(a, filename.c_str(), 16384); // block size
  
  if (rc != ARCHIVE_OK) 
  {
    THROW("Cannot open archive " << filename);
  }

  SDL_Surface *surface;
  SDL_RWops *rw;

  const Uint32 bufferSize = 10000000;  // allocated buffer
  std::auto_ptr< Uint8 > buffer( new Uint8[ bufferSize ] );
   
  if( buffer.get() == 0 ) 
    THROW("Memory error, cannot allocate buffer size " << bufferSize);

  std::string entryname;
  while( archive_read_next_header(a, &entry) == ARCHIVE_OK )
  {
    // for all entries in archive
    entryname = archive_entry_pathname(entry);
    if ((archive_entry_stat(entry)->st_mode & S_IFREG) == 0)
    {
      // not a regular file (maybe a directory). skip it.
      continue;
    }

    if (archive_entry_stat(entry)->st_size >= bufferSize) 
    {
      THROW("Cannot load archive: file is too big " << entryname << " in archive " << filename);
    }
      
    int size = archive_read_data(a, buffer.get(), bufferSize);  // read data into buffer
    rw = SDL_RWFromMem(buffer.get(), size);
    surface = IMG_Load_RW(rw, 0);
    SDL_SetAlpha( surface, 0, 0 );

    setPicture(entryname, *surface);
    SDL_FreeRW(rw);
  }

  rc = archive_read_finish(a);
  if (rc != ARCHIVE_OK)
  {
    THROW("Error while reading archive " << filename);
  }
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

class WalkerLoader::Impl
{
public:
  typedef std::vector< WalkerLoader::WalkerAnimationMap > Animations;
  Animations animations; // anim[WalkerGraphic][WalkerAction]
};

WalkerLoader& WalkerLoader::instance()
{
   static WalkerLoader inst;
   return inst;
}

WalkerLoader::WalkerLoader() : _d( new Impl )
{ 
}

void WalkerLoader::loadAll()
{
   _d->animations.resize(30);  // number of walker types

   WalkerAnimationMap waMap;

   waMap.clear();
   fillWalk(waMap, "citizen01", 1, 12);
   _d->animations[WG_POOR] = waMap;

   waMap.clear();
   fillWalk(waMap, "citizen01", 105, 12);
   _d->animations[WG_BATH] = waMap;

   waMap.clear();
   fillWalk(waMap, "citizen01", 209, 12);
   _d->animations[WG_PRIEST] = waMap;

   waMap.clear();
   fillWalk(waMap, "citizen01", 313, 12);
   _d->animations[WG_ACTOR] = waMap;

   waMap.clear();
   fillWalk(waMap, "citizen01", 417, 12);
   _d->animations[WG_TAMER] = waMap;

   waMap.clear();
   fillWalk(waMap, "citizen01", 617, 12);
   _d->animations[WG_TAX] = waMap;

   waMap.clear();
   fillWalk(waMap, "citizen01", 721, 12);
   _d->animations[WG_CHILD] = waMap;

   waMap.clear();
   fillWalk(waMap, "citizen01", 825, 12);
   _d->animations[WG_MARKETLADY] = waMap;

   waMap.clear();
   fillWalk(waMap, "citizen01", 929, 12);
   _d->animations[WG_PUSHER] = waMap;

   waMap.clear();
   fillWalk(waMap, "citizen01", 1033, 12);
   _d->animations[WG_PUSHER2] = waMap;

   waMap.clear();
   fillWalk(waMap, "citizen01", 1137, 12);
   _d->animations[WG_ENGINEER] = waMap;

   waMap.clear();
   fillWalk(waMap, "citizen02", 1, 12);
   _d->animations[WG_GLADIATOR] = waMap;

   waMap.clear();
   fillWalk(waMap, "citizen02", 199, 12);
   _d->animations[WG_GLADIATOR2] = waMap;

   waMap.clear();
   fillWalk(waMap, "citizen02", 351, 12);
   _d->animations[WG_RIOTER] = waMap;

   waMap.clear();
   fillWalk(waMap, "citizen02", 463, 12);
   _d->animations[WG_BARBER] = waMap;

   waMap.clear();
   fillWalk(waMap, "citizen02", 615, 12);
   _d->animations[WG_PREFECT] = waMap;

   waMap.clear();
   fillWalk(waMap, "citizen02", 767, 12);
   _d->animations[WG_PREFECT_DRAG_WATER] = waMap;

   waMap.clear();
   fillWalk(waMap, "citizen02", 863, 6);
   _d->animations[WG_PREFECT_FIGHTS_FIRE] = waMap;

   waMap.clear();
   fillWalk(waMap, "citizen02", 911, 12);
   _d->animations[WG_HOMELESS] = waMap;

   waMap.clear();
   fillWalk(waMap, "citizen03", 713, 12);
   _d->animations[WG_RICH] = waMap;

   waMap.clear();
   fillWalk(waMap, "citizen03", 817, 12);
   _d->animations[WG_DOCTOR] = waMap;

   waMap.clear();
   fillWalk(waMap, "citizen03", 921, 12);
   _d->animations[WG_RICH2] = waMap;

   waMap.clear();
   fillWalk(waMap, "citizen03", 1025, 12);
   _d->animations[WG_LIBRARIAN] = waMap;

   waMap.clear();
   fillWalk(waMap, "citizen03", 553, 12);
   _d->animations[WG_SOLDIER] = waMap;

   waMap.clear();
   fillWalk(waMap, "citizen03", 241, 12);
   _d->animations[WG_JAVELINEER] = waMap;

   waMap.clear();
   fillWalk(waMap, "citizen04", 1, 12);
   _d->animations[WG_HORSEMAN] = waMap;

   waMap.clear();
   fillWalk(waMap, ResourceGroup::carts, 145, 12);
   _d->animations[WG_HORSE_CARAVAN] = waMap;

   waMap.clear();
   fillWalk(waMap, ResourceGroup::carts, 273, 12);
   _d->animations[WG_CAMEL_CARAVAN] = waMap;

   waMap.clear();
   fillWalk(waMap, ResourceGroup::carts, 369, 12);
   _d->animations[WG_MARKETLADY_HELPER] = waMap;
}

void WalkerLoader::fillWalk(std::map<WalkerAction, Animation> &ioMap, const std::string &prefix, const int start, const int size)
{
   WalkerAction action;
   action._action = WA_MOVE;

   action._direction = D_NORTH;
   ioMap[action].load( prefix, start, size, Animation::straight, 8);
   action._direction = D_NORTH_EAST;
   ioMap[action].load( prefix, start+1, size, Animation::straight, 8);
   action._direction = D_EAST;
   ioMap[action].load( prefix, start+2, size, Animation::straight, 8);
   action._direction = D_SOUTH_EAST;
   ioMap[action].load( prefix, start+3, size, Animation::straight, 8);
   action._direction = D_SOUTH;
   ioMap[action].load( prefix, start+4, size, Animation::straight, 8);
   action._direction = D_SOUTH_WEST;
   ioMap[action].load( prefix, start+5, size, Animation::straight, 8);
   action._direction = D_WEST;
   ioMap[action].load( prefix, start+6, size, Animation::straight, 8);
   action._direction = D_NORTH_WEST;
   ioMap[action].load( prefix, start+7, size, Animation::straight, 8);
}

const std::map<WalkerAction, Animation>& WalkerLoader::getAnimationMap(const WalkerGraphicType walkerGraphic)
{
   return _d->animations[walkerGraphic];
}




