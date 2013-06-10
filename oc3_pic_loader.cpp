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
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com

#include "oc3_pic_loader.hpp"

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
#include "oc3_stringhelper.hpp"

class PicMetaData::Impl
{
public:
  void setRange(const std::string &preffix, const int first, const int last, const Point &data);
  void setOne(const std::string &preffix, const int index, const Point& data);
  void setOne(const std::string &preffix, const int index, const int xoffset, const int yoffset);

  std::map<std::string, Point> data;   // key=image name (Govt_00005)
};

PicMetaData& PicMetaData::instance()
{
   static PicMetaData inst;
   return inst;
}

PicMetaData::PicMetaData() : _d( new Impl )
{
   // tiles
  Point offset( -1, -1 );
  _d->setRange("land1a", 1, 303, offset);
  _d->setRange("oc3_land", 1, 2, offset);
  _d->setRange( ResourceGroup::land2a, 1, 151, offset);
  _d->setRange( ResourceGroup::land2a, 187, 195, offset); //burning ruins start animation
  _d->setRange( ResourceGroup::land2a, 214, 231, offset); //burning ruins middle animation
  _d->setRange( "land3a", 47, 92, offset);
  _d->setRange( "plateau", 1, 44, offset);
  _d->setRange( ResourceGroup::commerce, 1, 167, offset);
  _d->setRange( ResourceGroup::transport, 1, 93, offset);
  _d->setRange( ResourceGroup::security, 1, 61, offset);
  _d->setRange( ResourceGroup::entertaiment, 1, 116, offset);
  _d->setRange( ResourceGroup::housing, 1, 51, offset);
  _d->setRange( ResourceGroup::warehouse, 19, 83, offset);
  _d->setRange( ResourceGroup::utilitya, 1, 42, offset);
  _d->setRange( ResourceGroup::govt, 1, 10, offset);
  _d->setRange( ResourceGroup::sprites, 1, 8, offset ); //collapse fog
  _d->setRange( ResourceGroup::sprites, 9, 20, offset ); //overlay columns

  _d->setRange( ResourceGroup::waterOverlay, 1, 2, offset ); //wateroverlay building 1x1
  _d->setRange( ResourceGroup::waterOverlay, 11, 12, offset ); //wateroverlay houses 1x1
  _d->setRange( ResourceGroup::waterOverlay, 21, 22, offset ); //wateroverlay reservoir area 1x1
  _d->setRange( ResourceGroup::waterbuildings, 1, 4, offset ); //waterbuidlings (reservoir,fontain) empty/full

  offset = Point( 0, 30 );
  _d->setRange( ResourceGroup::waterOverlay, 3, 4, offset ); //wateroverlay building 2x2
  _d->setRange( ResourceGroup::waterOverlay, 13, 14, offset ); //wateroverlay houses 2x2 

  offset = Point( 0, 60 );

  _d->setRange( ResourceGroup::waterOverlay, 5, 6, offset ); //wateroverlay building 3x3
  _d->setRange( ResourceGroup::waterOverlay, 15, 16, offset ); //wateroverlay houses 3x3 

  offset = Point( 0, 90 );
  _d->setRange( ResourceGroup::waterOverlay, 7, 8, offset ); //wateroverlay building 4x4
  _d->setRange( ResourceGroup::waterOverlay, 17, 18, offset ); //wateroverlay houses 4x4 

  offset = Point( 0, 120 );
  _d->setRange( ResourceGroup::waterOverlay, 9, 10, offset ); //wateroverlay building 5x5
  
  _d->setOne( ResourceGroup::entertaiment, 12, 37, 62); // amphitheater
  _d->setOne( ResourceGroup::entertaiment, 35, 34, 37); // theater
  _d->setOne( ResourceGroup::entertaiment, 50, 70, 105);  // collosseum

  // animations
  _d->setRange(ResourceGroup::commerce, 2, 11, Point( 42, 34 ));  // market poor
  _d->setRange(ResourceGroup::commerce, 44, 53, Point( 66, 44 ));  // marble
  _d->setRange(ResourceGroup::commerce, 55, 60, Point( 45, 18 ));  // iron
  _d->setRange(ResourceGroup::commerce, 62, 71, Point( 15, 32 ));  // clay
  _d->setRange(ResourceGroup::commerce, 73, 82, Point( 35, 6 ) );  // timber
  _d->setRange(ResourceGroup::commerce, 87, 98, Point( 14, 36 ) );  // wine
  _d->setRange(ResourceGroup::commerce, 100, 107, Point( 0, 45 ) );  // oil
  _d->setRange(ResourceGroup::commerce, 109, 116, Point( 42, 36 ) );  // weapons
  _d->setRange(ResourceGroup::commerce, 118, 131, Point( 38, 39) );  // furniture
  _d->setRange(ResourceGroup::commerce, 133, 139, Point( 65, 42 ) );  // pottery
  _d->setRange(ResourceGroup::commerce, 159, 167, Point( 62, 42 ) );  // market rich

  // stock of input good
  _d->setOne(ResourceGroup::commerce, 153, 45, -8);  // grapes
  _d->setOne(ResourceGroup::commerce, 154, 37, -2);  // olive
  _d->setOne(ResourceGroup::commerce, 155, 48, -4);  // timber
  _d->setOne(ResourceGroup::commerce, 156, 47, -11);  // iron
  _d->setOne(ResourceGroup::commerce, 157, 47, -9);  // clay

  // warehouse
  _d->setOne(ResourceGroup::warehouse, 1, 60, 56);
  _d->setOne(ResourceGroup::warehouse, 18, 56, 93);
  _d->setRange(ResourceGroup::warehouse, 2, 17, Point( 55, 75 ));
  _d->setRange(ResourceGroup::warehouse, 84, 91, Point( 79, 108 ) );

  // granary
  _d->setOne(ResourceGroup::commerce, 141, 28, 109);
  _d->setOne(ResourceGroup::commerce, 142, 33, 75);
  _d->setOne(ResourceGroup::commerce, 143, 56, 65);
  _d->setOne(ResourceGroup::commerce, 144, 92, 65);
  _d->setOne(ResourceGroup::commerce, 145, 118, 76);
  _d->setOne(ResourceGroup::commerce, 146, 78, 69);
  _d->setOne(ResourceGroup::commerce, 147, 78, 69);
  _d->setOne(ResourceGroup::commerce, 148, 78, 69);
  _d->setOne(ResourceGroup::commerce, 149, 78, 69);
  _d->setOne(ResourceGroup::commerce, 150, 78, 69);
  _d->setOne(ResourceGroup::commerce, 151, 78, 69);
  _d->setOne(ResourceGroup::commerce, 152, 78, 69);

   // walkers
  offset = Point( -2, -2 );
  _d->setRange("citizen01", 1, 1240, offset);
  _d->setRange("citizen02", 1, 1030, offset);
  _d->setRange("citizen03", 1, 1128, offset);
  _d->setRange("citizen04", 1, 577, offset);
  _d->setRange("citizen05", 1, 184, offset);
}

void PicMetaData::Impl::setRange(const std::string &preffix, const int first, const int last, const Point& data)
{
   for (int i = first; i<=last; ++i)
   {
      setOne(preffix, i, data);
   }
}

void PicMetaData::Impl::setOne(const std::string &preffix, const int index, const Point& offset)
{
   std::string resource_name = StringHelper::format( 0xff, "%s_%05d", preffix.c_str(), index );
   data[resource_name] = offset;
}

void PicMetaData::Impl::setOne(const std::string &preffix, const int index, const int xoffset, const int yoffset)
{
   std::string resource_name = StringHelper::format( 0xff, "%s_%05d", preffix.c_str(), index );
   data[resource_name] = Point( xoffset, yoffset );
}

Point PicMetaData::get(const std::string &resource_name)
{
   std::map<std::string, Point>::iterator it = _d->data.find(resource_name);
   if (it == _d->data.end())
   {
      return Point();
      // THROW("Invalid resource name: " << resource_name);
   }

   return (*it).second;
}

PicMetaData::~PicMetaData()
{

}
PicLoader& PicLoader::instance()
{
  static PicLoader inst; 
  return inst;
}

void PicLoader::setPicture(const std::string &name, SDL_Surface &surface)
{
  // first: we deallocate the current picture, if any
  std::map<std::string, Picture>::iterator it = _resources.find(name);
  if (it != _resources.end())
  {
     Picture &pic = it->second;
     SDL_FreeSurface(pic.getSurface());
  }

  _resources[name] = makePicture(&surface, name);
}

void PicLoader::setPicture( const std::string &name, const Picture& pic )
{
  setPicture( name, *pic.getSurface() );
}

Picture& PicLoader::getPicture(const std::string &name)
{
   std::map<std::string, Picture>::iterator it = _resources.find(name);
   if (it == _resources.end()) 
   {
     THROW("Unknown resource " << name);
   }
   return it->second;
}

Picture& PicLoader::getPicture(const std::string &prefix, const int idx)
{
   std::string resource_name = StringHelper::format( 0xff, "%s_%05d.png", prefix.c_str(),idx );

   return getPicture(resource_name);
}


Picture& PicLoader::getPicture(const GoodType goodType)
{
   int pic_index;
   switch (goodType)
   {
   case G_WHEAT:
      pic_index = 317;
      break;
   case G_VEGETABLE:
      pic_index = 318;
      break;
   case G_FRUIT:
      pic_index = 319;
      break;
   case G_OLIVE:
      pic_index = 320;
      break;
   case G_GRAPE:
      pic_index = 321;
      break;
   case G_MEAT:
      pic_index = 322;
      break;
   case G_WINE:
      pic_index = 323;
      break;
   case G_OIL:
      pic_index = 324;
      break;
   case G_IRON:
      pic_index = 325;
      break;
   case G_TIMBER:
      pic_index = 326;
      break;
   case G_CLAY:
      pic_index = 327;
      break;
   case G_MARBLE:
      pic_index = 328;
      break;
   case G_WEAPON:
      pic_index = 329;
      break;
   case G_FURNITURE:
      pic_index = 330;
      break;
   case G_POTTERY:
      pic_index = 331;
      break;
   case G_FISH:
      pic_index = 333;
      break;
   default:
      THROW("This good type has no picture:" << goodType);
   }

   return getPicture( ResourceGroup::panelBackground, pic_index);
}

PicturesArray PicLoader::getPictures()
{
   PicturesArray pictures;
   for (std::map<std::string, Picture>::iterator it = _resources.begin(); it != _resources.end(); ++it)
   {
      // for every resource
      pictures.push_back(&(*it).second);
   }
   return pictures;
}


void PicLoader::loadWaitPics()
{
  std::string aPath = AppConfig::get( AppConfig::resourcePath ).toString() + "/pics/";
  loadArchive(aPath+"pics_wait.zip");

  StringHelper::debug( 0xff, "number of images loaded: %d", _resources.size() );
}

void PicLoader::loadAllPics()
{
  std::string aPath = AppConfig::get( AppConfig::resourcePath ).toString() + "/pics/";
  loadArchive(aPath + "pics.zip");
  loadArchive(aPath + "pics_oc3.zip");	
  StringHelper::debug( 0xff, "number of images loaded: %d", _resources.size() );
}

void PicLoader::loadArchive(const std::string &filename)
{
  std::cout << "reading image archive: " << filename << std::endl;
  struct archive *a;
  struct archive_entry *entry;
  int rc;

  a = archive_read_new();
  archive_read_support_compression_all(a);
  archive_read_support_format_all(a);
  rc = archive_read_open_filename(a, filename.c_str(), 16384); // block size
  if (rc != ARCHIVE_OK) THROW("Cannot open archive " << filename);

  SDL_Surface *surface;
  SDL_RWops *rw;

  const Uint32 bufferSize = 10000000;  // allocated buffer
  std::auto_ptr< Uint8 > buffer( new Uint8[ bufferSize ] );
   
  if( buffer.get() == 0 ) 
    THROW("Memory error, cannot allocate buffer size " << bufferSize);

  std::string entryname;
  while (archive_read_next_header(a, &entry) == ARCHIVE_OK)
  {
    // for all entries in archive
    entryname = archive_entry_pathname(entry);
    if ((archive_entry_stat(entry)->st_mode & S_IFREG) == 0)
    {
      // not a regular file (maybe a directory). skip it.
      continue;
    }
    if (archive_entry_stat(entry)->st_size >= bufferSize) 
      THROW("Cannot load archive: file is too big " << entryname << " in archive " << filename);
      
    int size = archive_read_data(a, buffer.get(), bufferSize);  // read data into buffer
    rw = SDL_RWFromMem(buffer.get(), size);
    surface = IMG_Load_RW(rw, 0);
    setPicture(entryname, *surface);
    SDL_FreeRW(rw);
  }

  rc = archive_read_finish(a);
  if (rc != ARCHIVE_OK)
  {
    THROW("Error while reading archive " << filename);
  }
}


Picture PicLoader::makePicture(SDL_Surface *surface, const std::string& resource_name) const
{
   Point offset( 0, 0 );
   // decode the picture name => to set the offset manually
   // resource_name = "buildings/Govt_00005.png"
   int dot_pos = resource_name.find('.');
   std::string filename = resource_name.substr(0, dot_pos);

   Point pic_info = PicMetaData::instance().get(filename);

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

void PicLoader::createResources()
{
  Picture& originalPic = getPicture( ResourceGroup::utilitya, 34 );
  setPicture( std::string( ResourceGroup::waterbuildings ) + "_00001.png", *originalPic.getSurface() );

  Picture* fullReservoir = originalPic.copy(); //mem leak on destroy picloader
  fullReservoir->draw( getPicture( ResourceGroup::utilitya, 35 ), 47, 37 );
  setPicture( std::string( ResourceGroup::waterbuildings ) + "_00002.png", *fullReservoir->getSurface() );

  Picture& emptyFontainOrig = getPicture( ResourceGroup::utilitya, 10 );
  setPicture( std::string( ResourceGroup::waterbuildings ) + "_00003.png", *emptyFontainOrig.getSurface() );

  Picture* fullFontain = emptyFontainOrig.copy();  //mem leak on destroy picloader
  fullFontain->draw( getPicture( ResourceGroup::utilitya, 11 ), 12, 25 );
  setPicture( std::string( ResourceGroup::waterbuildings) + "_00004.png", *fullFontain->getSurface() );
}

PicLoader::PicLoader()
{

}
bool operator<(const WalkerAction &v1, const WalkerAction &v2)
{
   if (v1._action!=v2._action)
   {
      return v1._action < v2._action;
   }
   else
   {
      return v1._direction < v2._direction;
   }
}


WalkerLoader* WalkerLoader::_instance = NULL;
WalkerLoader& WalkerLoader::instance()
{
   if (_instance == NULL)
   {
      _instance = new WalkerLoader();
      if (_instance == NULL) THROW("Memory error, cannot instantiate object");
   }
   return *_instance;
}

WalkerLoader::WalkerLoader() { }

void WalkerLoader::loadAll()
{
   _animations.resize(30);  // number of walker types

   std::map<WalkerAction, Animation> map;

   map.clear();
   fillWalk(map, "citizen01", 1, 12);
   _animations[WG_POOR] = map;

   map.clear();
   fillWalk(map, "citizen01", 105, 12);
   _animations[WG_BATH] = map;

   map.clear();
   fillWalk(map, "citizen01", 209, 12);
   _animations[WG_PRIEST] = map;

   map.clear();
   fillWalk(map, "citizen01", 313, 12);
   _animations[WG_ACTOR] = map;

   map.clear();
   fillWalk(map, "citizen01", 417, 12);
   _animations[WG_TAMER] = map;

   map.clear();
   fillWalk(map, "citizen01", 617, 12);
   _animations[WG_TAX] = map;

   map.clear();
   fillWalk(map, "citizen01", 721, 12);
   _animations[WG_CHILD] = map;

   map.clear();
   fillWalk(map, "citizen01", 825, 12);
   _animations[WG_MARKETLADY] = map;

   map.clear();
   fillWalk(map, "citizen01", 929, 12);
   _animations[WG_PUSHER] = map;

   map.clear();
   fillWalk(map, "citizen01", 1033, 12);
   _animations[WG_PUSHER2] = map;

   map.clear();
   fillWalk(map, "citizen01", 1137, 12);
   _animations[WG_ENGINEER] = map;

   map.clear();
   fillWalk(map, "citizen02", 1, 12);
   _animations[WG_GLADIATOR] = map;

   map.clear();
   fillWalk(map, "citizen02", 199, 12);
   _animations[WG_GLADIATOR2] = map;

   map.clear();
   fillWalk(map, "citizen02", 351, 12);
   _animations[WG_RIOTER] = map;

   map.clear();
   fillWalk(map, "citizen02", 463, 12);
   _animations[WG_BARBER] = map;

   map.clear();
   fillWalk(map, "citizen02", 615, 12);
   _animations[WG_PREFECT] = map;

   map.clear();
   fillWalk(map, "citizen02", 767, 12);
   _animations[WG_PREFECT_DRAG_WATER] = map;

   map.clear();
   fillWalk(map, "citizen02", 863, 6);
   _animations[WG_PREFECT_FIGHTS_FIRE] = map;

   map.clear();
   fillWalk(map, "citizen02", 911, 12);
   _animations[WG_HOMELESS] = map;

   map.clear();
   fillWalk(map, "citizen03", 713, 12);
   _animations[WG_RICH] = map;

   map.clear();
   fillWalk(map, "citizen03", 817, 12);
   _animations[WG_DOCTOR] = map;

   map.clear();
   fillWalk(map, "citizen03", 921, 12);
   _animations[WG_RICH2] = map;

   map.clear();
   fillWalk(map, "citizen03", 1025, 12);
   _animations[WG_LIBRARIAN] = map;

   map.clear();
   fillWalk(map, "citizen03", 553, 12);
   _animations[WG_SOLDIER] = map;

   map.clear();
   fillWalk(map, "citizen03", 241, 12);
   _animations[WG_JAVELINEER] = map;

   map.clear();
   fillWalk(map, "citizen04", 1, 12);
   _animations[WG_HORSEMAN] = map;

   map.clear();
   fillWalk(map, ResourceGroup::carts, 145, 12);
   _animations[WG_HORSE_CARAVAN] = map;

   map.clear();
   fillWalk(map, ResourceGroup::carts, 273, 12);
   _animations[WG_CAMEL_CARAVAN] = map;

   map.clear();
   fillWalk(map, ResourceGroup::carts, 369, 12);
   _animations[WG_LITTLE_HELPER] = map;

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
   return _animations[walkerGraphic];
}


class CartLoader::Impl
{
public:
  std::vector< PicturesArray > carts; // pictures[GoodType][Direction]
};

CartLoader& CartLoader::instance()
{
  static CartLoader inst;
  return inst;
}

CartLoader::CartLoader() : _d( new Impl )
{
   loadAll();
}

void CartLoader::loadAll()
{
   std::cout << "Loading cart graphics" << std::endl;

   // FIX: size of std::vector to match number of carts with goods + emmigrants + immigrants
   // _carts.resize(G_MAX);
   _d->carts.resize(CT_MAX);

   std::vector<Picture*> cart;  // key=direction
   bool frontCart = false;

   fillCart(cart, ResourceGroup::carts, 1, frontCart);
   _d->carts[G_NONE] = cart;
   fillCart(cart, ResourceGroup::carts, 9, frontCart);
   _d->carts[G_WHEAT] = cart;
   fillCart(cart, ResourceGroup::carts, 17, frontCart);
   _d->carts[G_VEGETABLE] = cart;
   fillCart(cart, ResourceGroup::carts, 25, frontCart);
   _d->carts[G_FRUIT] = cart;
   fillCart(cart, ResourceGroup::carts, 33, frontCart);
   _d->carts[G_OLIVE] = cart;
   fillCart(cart, ResourceGroup::carts, 41, frontCart);
   _d->carts[G_GRAPE] = cart;
   fillCart(cart, ResourceGroup::carts, 49, frontCart);
   _d->carts[G_MEAT] = cart;
   fillCart(cart, ResourceGroup::carts, 57, frontCart);
   _d->carts[G_WINE] = cart;
   fillCart(cart, ResourceGroup::carts, 65, frontCart);
   _d->carts[G_OIL] = cart;
   fillCart(cart, ResourceGroup::carts, 73, frontCart);
   _d->carts[G_IRON] = cart;
   fillCart(cart, ResourceGroup::carts, 81, frontCart);
   _d->carts[G_TIMBER] = cart;
   fillCart(cart, ResourceGroup::carts, 89, frontCart);
   _d->carts[G_CLAY] = cart;
   fillCart(cart, ResourceGroup::carts, 97, frontCart);
   _d->carts[G_MARBLE] = cart;
   fillCart(cart, ResourceGroup::carts, 105, frontCart);
   _d->carts[G_WEAPON] = cart;
   fillCart(cart, ResourceGroup::carts, 113, frontCart);
   _d->carts[G_FURNITURE] = cart;
   fillCart(cart, ResourceGroup::carts, 121, frontCart);
   _d->carts[G_POTTERY] = cart;
   fillCart(cart, ResourceGroup::carts, 129, !frontCart);
   _d->carts[G_SCARB1] = cart;
   fillCart(cart, ResourceGroup::carts, 137, !frontCart);
   _d->carts[G_SCARB2] = cart;
   fillCart(cart, ResourceGroup::carts, 697, frontCart);
   _d->carts[G_FISH] = cart;
}

namespace{
static const Point frontCartOffsetSouth = Point( -33, 22 );
static const Point frontCartOffsetWest  = Point( -31, 35 );
static const Point frontCartOffsetNorth = Point(  -5, 37 );
static const Point frontCartOffsetEast  = Point(  -5, 22 );
static const Point frontCartOffsetSouthEast  = Point( -20, 20 );
static const Point frontCartOffsetNorthWest  = Point( -20, 40 );
static const Point frontCartOffsetNorthEast  = Point( -5, 30 );
static const Point frontCartOffsetSouthWest  = Point( -5, 22 );

static const Point backCartOffsetSouth = Point(  -5, 40 );
static const Point backCartOffsetWest  = Point(  -5, 22 );
static const Point backCartOffsetNorth = Point( -33, 22 );
static const Point backCartOffsetEast  = Point( -31, 35 );
static const Point backCartOffsetSouthEast  = Point( -20, 40 );
static const Point backCartOffsetNorthWest  = Point( -20, 20 );
static const Point backCartOffsetNorthEast  = Point( -30, 30 );
static const Point backCartOffsetSouthWest  = Point( -20, 20 );
}

void CartLoader::fillCart(std::vector<Picture*> &ioCart, const std::string &prefix, const int start, bool back )
{
   PicLoader &picLoader = PicLoader::instance();

   ioCart.clear();
   ioCart.resize(D_MAX);
   
   ioCart[D_NORTH]      = &picLoader.getPicture(ResourceGroup::carts, start);
   ioCart[D_NORTH_EAST] = &picLoader.getPicture(ResourceGroup::carts, start + 1);
   ioCart[D_EAST]       = &picLoader.getPicture(ResourceGroup::carts, start + 2);
   ioCart[D_SOUTH_EAST] = &picLoader.getPicture(ResourceGroup::carts, start + 3);
   ioCart[D_SOUTH]      = &picLoader.getPicture(ResourceGroup::carts, start + 4);
   ioCart[D_SOUTH_WEST] = &picLoader.getPicture(ResourceGroup::carts, start + 5);
   ioCart[D_WEST]       = &picLoader.getPicture(ResourceGroup::carts, start + 6);
   ioCart[D_NORTH_WEST] = &picLoader.getPicture(ResourceGroup::carts, start + 7);

   ioCart[D_SOUTH]->setOffset( back ? backCartOffsetSouth : frontCartOffsetSouth);
   ioCart[D_WEST]->setOffset ( back ? backCartOffsetWest  : frontCartOffsetWest );
   ioCart[D_NORTH]->setOffset( back ? backCartOffsetNorth : frontCartOffsetNorth);
   ioCart[D_EAST]->setOffset ( back ? backCartOffsetEast  : frontCartOffsetEast );
   
   ioCart[D_SOUTH_EAST]->setOffset ( back ? backCartOffsetSouthEast  : frontCartOffsetSouthEast );
   ioCart[D_NORTH_WEST]->setOffset ( back ? backCartOffsetNorthWest  : frontCartOffsetNorthWest );
   ioCart[D_NORTH_EAST]->setOffset ( back ? backCartOffsetNorthEast  : frontCartOffsetNorthEast );
   ioCart[D_SOUTH_WEST]->setOffset ( back ? backCartOffsetSouthWest  : frontCartOffsetSouthWest );
}

Picture& CartLoader::getCart(const GoodStock &stock, const DirectionType &direction)
{
   Picture *res = NULL;
   if (stock._currentQty == 0)
   {
      res = _d->carts.at( G_NONE ).at(direction);
   }
   else
   {
      res = _d->carts.at(stock._goodType).at(direction);
   }

   return *res;
}

Picture& CartLoader::getCart(GoodType cart, const DirectionType &direction)
{
  Picture *res = NULL;
  res = _d->carts.at( cart ).at( direction );
  return *res;
}

Picture& CartLoader::getCart(CartTypes cart, const DirectionType &direction)
{
  Picture *res = NULL;
  res = _d->carts.at( cart ).at( direction );
  return *res;
}


