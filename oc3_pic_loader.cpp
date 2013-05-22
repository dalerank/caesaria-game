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
#include <sstream>
#include <iostream>
#include <memory>
#include <iomanip>
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

PicMetaData* PicMetaData::_instance = NULL;

PicMetaData& PicMetaData::instance()
{
   if (_instance == NULL)
   {
      _instance = new PicMetaData();
      if (_instance == NULL) THROW("Memory error, cannot instantiate object");
   }
   return *_instance;
}

PicMetaData::PicMetaData()
{
   _dummy_data.xoffset = 0;
   _dummy_data.yoffset = 0;

   // tiles
   PicInfo info;
   info.xoffset = -1;
   info.yoffset = -1;
   setRange("land1a", 1, 303, info);
   setRange("oc3_land", 1, 2, info);
   setRange( ResourceGroup::land2a, 1, 151, info);
   setRange( ResourceGroup::land2a, 187, 195, info); //burning ruins start animation
   setRange("land2a", 214, 231, info); //burning ruins middle animation
   setRange("land3a", 47, 92, info);
   setRange("plateau", 1, 44, info);
   setRange( ResourceGroup::commerce, 1, 167, info);
   setRange( "transport", 1, 93, info);
   setRange( ResourceGroup::security, 1, 61, info);
   setRange( ResourceGroup::entertaiment, 1, 116, info);
   setRange("housng1a", 1, 51, info);
   setRange( ResourceGroup::warehouse, 19, 83, info);
   setRange( ResourceGroup::utilitya, 1, 42, info);
   setRange("govt", 1, 10, info);
   setRange( ResourceGroup::sprites, 1, 8, info );

   setRange( ResourceGroup::waterbuildings, 1, 2, info); //reservoir empty/full
   
   setOne( ResourceGroup::entertaiment, 12, 37, 62); // amphitheater
   setOne( ResourceGroup::entertaiment, 35, 34, 37); // theater
   setOne( ResourceGroup::entertaiment, 50, 70, 105);  // collosseum

   // animations
   info.xoffset = 42;
   info.yoffset = 34;
   setRange(ResourceGroup::commerce, 2, 11, info);  // market poor
   info.xoffset = 66;
   info.yoffset = 44;
   setRange(ResourceGroup::commerce, 44, 53, info);  // marble
   info.xoffset = 45;
   info.yoffset = 18;
   setRange(ResourceGroup::commerce, 55, 60, info);  // iron
   info.xoffset = 15;
   info.yoffset = 32;
   setRange(ResourceGroup::commerce, 62, 71, info);  // clay
   info.xoffset = 35;
   info.yoffset = 6;
   setRange(ResourceGroup::commerce, 73, 82, info);  // timber
   info.xoffset = 14;
   info.yoffset = 36;
   setRange(ResourceGroup::commerce, 87, 98, info);  // wine
   info.xoffset = 0;
   info.yoffset = 45;
   setRange(ResourceGroup::commerce, 100, 107, info);  // oil
   info.xoffset = 42;
   info.yoffset = 36;
   setRange(ResourceGroup::commerce, 109, 116, info);  // weapons
   info.xoffset = 38;
   info.yoffset = 39;
   setRange(ResourceGroup::commerce, 118, 131, info);  // furniture
   info.xoffset = 65;
   info.yoffset = 42;
   setRange(ResourceGroup::commerce, 133, 139, info);  // pottery
   info.xoffset = 65;
   info.yoffset = 42;
   setRange(ResourceGroup::commerce, 159, 167, info);  // market rich

   // stock of input good
   setOne(ResourceGroup::commerce, 153, 45, -8);  // grapes
   setOne(ResourceGroup::commerce, 154, 37, -2);  // olive
   setOne(ResourceGroup::commerce, 155, 48, -4);  // timber
   setOne(ResourceGroup::commerce, 156, 47, -11);  // iron
   setOne(ResourceGroup::commerce, 157, 47, -9);  // clay

   // warehouse
   setOne(ResourceGroup::warehouse, 1, 60, 56);
   setOne(ResourceGroup::warehouse, 18, 56, 93);
   info.xoffset = 55;
   info.yoffset = 75;
   setRange(ResourceGroup::warehouse, 2, 17, info);
   info.xoffset = 79;
   info.yoffset = 108;
   setRange(ResourceGroup::warehouse, 84, 91, info);

   // granary
   setOne(ResourceGroup::commerce, 141, 28, 109);
   setOne(ResourceGroup::commerce, 142, 33, 75);
   setOne(ResourceGroup::commerce, 143, 56, 65);
   setOne(ResourceGroup::commerce, 144, 92, 65);
   setOne(ResourceGroup::commerce, 145, 118, 76);
   setOne(ResourceGroup::commerce, 146, 78, 69);
   setOne(ResourceGroup::commerce, 147, 78, 69);
   setOne(ResourceGroup::commerce, 148, 78, 69);
   setOne(ResourceGroup::commerce, 149, 78, 69);
   setOne(ResourceGroup::commerce, 150, 78, 69);
   setOne(ResourceGroup::commerce, 151, 78, 69);
   setOne(ResourceGroup::commerce, 152, 78, 69);

   // walkers
   info.xoffset = -2;
   info.yoffset = -2;
   setRange("citizen01", 1, 1240, info);
   setRange("citizen02", 1, 1030, info);
   setRange("citizen03", 1, 1128, info);
   setRange("citizen04", 1, 577, info);
   setRange("citizen05", 1, 184, info);
}

void PicMetaData::setRange(const std::string &preffix, const int first, const int last, PicInfo &data)
{
   for (int i = first; i<=last; ++i)
   {
      setOne(preffix, i, data);
   }
}

void PicMetaData::setOne(const std::string &preffix, const int index, PicInfo &data)
{
   std::ostringstream oss;
   oss << preffix << "_" << std::setw(5) << std::setfill('0') << index;
   std::string resource_name = oss.str();
   _data[resource_name] = data;
}

void PicMetaData::setOne(const std::string &preffix, const int index, const int xoffset, const int yoffset)
{
   std::ostringstream oss;
   oss << preffix << "_" << std::setw(5) << std::setfill('0') << index;
   std::string resource_name = oss.str();
   PicInfo data;
   data.xoffset = xoffset;
   data.yoffset = yoffset;
   _data[resource_name] = data;
}

PicInfo& PicMetaData::get_data(const std::string &resource_name)
{
   std::map<std::string, PicInfo>::iterator it = _data.find(resource_name);
   if (it == _data.end())
   {
      return _dummy_data;
      // THROW("Invalid resource name: " << resource_name);
   }

   return (*it).second;
}

PicLoader& PicLoader::instance()
{
  static PicLoader inst; 
  return inst;
}

void PicLoader::set_picture(const std::string &name, SDL_Surface &surface)
{
   // first: we deallocate the current picture, if any
   std::map<std::string, Picture>::iterator it = _resources.find(name);
   if (it != _resources.end())
   {
      Picture &pic = it->second;
      SDL_FreeSurface(pic.get_surface());
   }

   _resources[name] = make_picture(&surface, name);
}

Picture& PicLoader::get_picture(const std::string &name)
{
   std::map<std::string, Picture>::iterator it = _resources.find(name);
   if (it == _resources.end()) THROW("Unknown resource " << name);
   return it->second;
}

Picture& PicLoader::get_picture(const std::string &prefix, const int idx)
{
   std::stringstream ss;
   ss << prefix << "_" << std::setw(5) << std::setfill('0') << idx << ".png";
   std::string resource_name = ss.str();

   return get_picture(resource_name);
}


Picture& PicLoader::get_picture_good(const GoodType goodType)
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

   return get_picture( ResourceGroup::panelBackground, pic_index);
}

std::list<Picture*> PicLoader::get_pictures()
{
   std::list<Picture*> pictures;
   for (std::map<std::string, Picture>::iterator it = _resources.begin(); it != _resources.end(); ++it)
   {
      // for every resource
      Picture& pic = (*it).second;
      pictures.push_back(&pic);
   }
   return pictures;
}


void PicLoader::load_wait()
{
  std::string aPath = AppConfig::get( AppConfig::resourcePath ).toString() + "/pics/";
  load_archive(aPath+"pics_wait.zip");

  StringHelper::debug( 0xff, "number of images loaded: %d", _resources.size() );
}

void PicLoader::load_all()
{
  std::string aPath = AppConfig::get( AppConfig::resourcePath ).toString() + "/pics/";
  load_archive(aPath + "pics.zip");
  load_archive(aPath + "pics_oc3.zip");	
  StringHelper::debug( 0xff, "number of images loaded: %d", _resources.size() );
}

void PicLoader::load_archive(const std::string &filename)
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
    set_picture(entryname, *surface);
    SDL_FreeRW(rw);
  }

  rc = archive_read_finish(a);
  if (rc != ARCHIVE_OK)
    THROW("Error while reading archive " << filename);
}


Picture PicLoader::make_picture(SDL_Surface *surface, const std::string& resource_name) const
{
   int xoffset = 0;
   int yoffset = 0;


   // decode the picture name => to set the offset manually
   // resource_name = "buildings/Govt_00005.png"
   int dot_pos = resource_name.find('.');
   std::string filename = resource_name.substr(0, dot_pos);

   PicInfo& pic_info = PicMetaData::instance().get_data(filename);

   if (pic_info.xoffset == -1 && pic_info.yoffset == -1)
   {
      // this is a tiled picture=> automatic offset correction
      yoffset= surface->h-15*((surface->w+2)/60);   // (w+2)/60 is the size of the tile: (1x1, 2x2, 3x3, ...)
   }
   else if (pic_info.xoffset == -2 && pic_info.yoffset == -2)
   {
      // this is a walker picture=> automatic offset correction
      xoffset= -surface->w/2;
      yoffset= int(surface->h*3./4.);
   }
   else
   {
      xoffset= pic_info.xoffset;
      yoffset= pic_info.yoffset;
   }

   Picture pic;
   pic.init(surface, xoffset, yoffset);
   pic.set_name(filename);

   return pic;
}

void PicLoader::createResources()
{
  Picture& originalPic = get_picture( ResourceGroup::utilitya, 34 );
  Picture& emptyReservoir = originalPic.copy();
  set_picture( std::string( ResourceGroup::waterbuildings ) + "_00001.png", *emptyReservoir.get_surface() );

  Picture& fullReservoir = originalPic.copy();
  Picture& water = get_picture( ResourceGroup::utilitya, 35 );
  fullReservoir.draw( water, 47, 8 );

  set_picture( std::string( ResourceGroup::waterbuildings ) + "_00002.png", *fullReservoir.get_surface() );
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


CartLoader* CartLoader::_instance = NULL;

CartLoader& CartLoader::instance()
{
   if (_instance == NULL)
   {
      _instance = new CartLoader();
      if (_instance == NULL) THROW("Memory error, cannot instantiate object");
   }
   return *_instance;
}

CartLoader::CartLoader()
{
   loadAll();
}

void CartLoader::loadAll()
{
   std::cout << "Loading cart graphics" << std::endl;

   // FIX: size of std::vector to match number of carts with goods + emmigrants + immigrants
   // _carts.resize(G_MAX);
   _carts.resize(CT_MAX);

   std::vector<Picture*> cart;  // key=direction
   bool frontCart = false;

   fillCart(cart, ResourceGroup::carts, 1, frontCart);
   _carts[G_NONE] = cart;
   fillCart(cart, ResourceGroup::carts, 9, frontCart);
   _carts[G_WHEAT] = cart;
   fillCart(cart, ResourceGroup::carts, 17, frontCart);
   _carts[G_VEGETABLE] = cart;
   fillCart(cart, ResourceGroup::carts, 25, frontCart);
   _carts[G_FRUIT] = cart;
   fillCart(cart, ResourceGroup::carts, 33, frontCart);
   _carts[G_OLIVE] = cart;
   fillCart(cart, ResourceGroup::carts, 41, frontCart);
   _carts[G_GRAPE] = cart;
   fillCart(cart, ResourceGroup::carts, 49, frontCart);
   _carts[G_MEAT] = cart;
   fillCart(cart, ResourceGroup::carts, 57, frontCart);
   _carts[G_WINE] = cart;
   fillCart(cart, ResourceGroup::carts, 65, frontCart);
   _carts[G_OIL] = cart;
   fillCart(cart, ResourceGroup::carts, 73, frontCart);
   _carts[G_IRON] = cart;
   fillCart(cart, ResourceGroup::carts, 81, frontCart);
   _carts[G_TIMBER] = cart;
   fillCart(cart, ResourceGroup::carts, 89, frontCart);
   _carts[G_CLAY] = cart;
   fillCart(cart, ResourceGroup::carts, 97, frontCart);
   _carts[G_MARBLE] = cart;
   fillCart(cart, ResourceGroup::carts, 105, frontCart);
   _carts[G_WEAPON] = cart;
   fillCart(cart, ResourceGroup::carts, 113, frontCart);
   _carts[G_FURNITURE] = cart;
   fillCart(cart, ResourceGroup::carts, 121, frontCart);
   _carts[G_POTTERY] = cart;
   fillCart(cart, ResourceGroup::carts, 129, !frontCart);
   _carts[G_SCARB1] = cart;
   fillCart(cart, ResourceGroup::carts, 137, !frontCart);
   _carts[G_SCARB2] = cart;
   fillCart(cart, ResourceGroup::carts, 697, frontCart);
   _carts[G_FISH] = cart;
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
   
   ioCart[D_NORTH]      = &picLoader.get_picture(ResourceGroup::carts, start);
   ioCart[D_NORTH_EAST] = &picLoader.get_picture(ResourceGroup::carts, start + 1);
   ioCart[D_EAST]       = &picLoader.get_picture(ResourceGroup::carts, start + 2);
   ioCart[D_SOUTH_EAST] = &picLoader.get_picture(ResourceGroup::carts, start + 3);
   ioCart[D_SOUTH]      = &picLoader.get_picture(ResourceGroup::carts, start + 4);
   ioCart[D_SOUTH_WEST] = &picLoader.get_picture(ResourceGroup::carts, start + 5);
   ioCart[D_WEST]       = &picLoader.get_picture(ResourceGroup::carts, start + 6);
   ioCart[D_NORTH_WEST] = &picLoader.get_picture(ResourceGroup::carts, start + 7);

   ioCart[D_SOUTH]->set_offset( back ? backCartOffsetSouth : frontCartOffsetSouth);
   ioCart[D_WEST]->set_offset ( back ? backCartOffsetWest  : frontCartOffsetWest );
   ioCart[D_NORTH]->set_offset( back ? backCartOffsetNorth : frontCartOffsetNorth);
   ioCart[D_EAST]->set_offset ( back ? backCartOffsetEast  : frontCartOffsetEast );
   
   ioCart[D_SOUTH_EAST]->set_offset ( back ? backCartOffsetSouthEast  : frontCartOffsetSouthEast );
   ioCart[D_NORTH_WEST]->set_offset ( back ? backCartOffsetNorthWest  : frontCartOffsetNorthWest );
   ioCart[D_NORTH_EAST]->set_offset ( back ? backCartOffsetNorthEast  : frontCartOffsetNorthEast );
   ioCart[D_SOUTH_WEST]->set_offset ( back ? backCartOffsetSouthWest  : frontCartOffsetSouthWest );
}

Picture& CartLoader::getCart(const GoodStock &stock, const DirectionType &direction)
{
   Picture *res = NULL;
   if (stock._currentQty == 0)
   {
      res = _carts.at( G_NONE ).at(direction);
   }
   else
   {
      res = _carts.at(stock._goodType).at(direction);
   }

   return *res;
}

Picture& CartLoader::getCart(GoodType cart, const DirectionType &direction)
{
  Picture *res = NULL;
  res = _carts.at( cart ).at( direction );
  return *res;
}

Picture& CartLoader::getCart(CartTypes cart, const DirectionType &direction)
{
  Picture *res = NULL;
  res = _carts.at( cart ).at( direction );
  return *res;
}

FontLoader::FontLoader()
{ }


void FontLoader::load_all(const std::string &resourcePath)
{
   std::string full_font_path = resourcePath + "/FreeSerif.ttf";
   TTF_Font *ttf;

   SDL_Color black = {0, 0, 0, 255};
   SDL_Color red = {160, 0, 0, 255};  // dim red
   SDL_Color white = {215, 215, 215, 255};  // dim white
   SDL_Color yellow = {160, 160, 0, 255}; 

   ttf = TTF_OpenFont(full_font_path.c_str(), 12);
   if (ttf == NULL) THROW("Cannot load font file:" << full_font_path << ", error:" << TTF_GetError());
   Font font0(*ttf, black);
   FontCollection::instance().setFont(FONT_0, font0);

   ttf = TTF_OpenFont(full_font_path.c_str(), 18);
   if (ttf == NULL) THROW("Cannot load font file:" << full_font_path << ", error:" << TTF_GetError());
   Font font2(*ttf, black);
   FontCollection::instance().setFont(FONT_2, font2);

   ttf = TTF_OpenFont(full_font_path.c_str(), 18);
   if (ttf == NULL) THROW("Cannot load font file:" << full_font_path << ", error:" << TTF_GetError());
   Font font2red(*ttf, red);
   FontCollection::instance().setFont(FONT_2_RED, font2red);

   ttf = TTF_OpenFont(full_font_path.c_str(), 18);
   if (ttf == NULL) THROW("Cannot load font file:" << full_font_path << ", error:" << TTF_GetError());
   Font font2white(*ttf, white);
   FontCollection::instance().setFont(FONT_2_WHITE, font2white);

   ttf = TTF_OpenFont(full_font_path.c_str(), 18);
   if (ttf == NULL) THROW("Cannot load font file:" << full_font_path << ", error:" << TTF_GetError());
   Font font2yellow(*ttf, yellow);
   FontCollection::instance().setFont(FONT_2_YELLOW, font2yellow);

   ttf = TTF_OpenFont(full_font_path.c_str(), 28);
   if (ttf == NULL) THROW("Cannot load font file:" << full_font_path << ", error:" << TTF_GetError());
   Font font3(*ttf, black);
   FontCollection::instance().setFont(FONT_3, font3);
}
