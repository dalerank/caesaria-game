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


#include "pic_loader.hpp"

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

#include "oc3_positioni.h"
#include "exception.hpp"
#include "sdl_facade.hpp"

namespace {
static const char* rcCartsGroup    = "carts";
static const char* rcCommerceGroup = "commerce";
static const char* rcEntertaimentGroup  = "entertainment";
static const char* rcWarehouseGroup  = "warehouse";
}

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
   setRange("land2a", 1, 151, info);
   setRange("land2a", 187, 195, info); //burning ruins start animation
   setRange("land2a", 214, 231, info); //burning ruins middle animation
   setRange("land3a", 47, 92, info);
   setRange("plateau", 1, 44, info);
   setRange(rcCommerceGroup, 1, 167, info);
   setRange("transport", 1, 93, info);
   setRange("security", 1, 61, info);
   setRange(rcEntertaimentGroup, 1, 116, info);
   setRange("housng1a", 1, 51, info);
   setRange(rcWarehouseGroup, 19, 83, info);
   setRange("utilitya", 1, 42, info);
   setRange("govt", 1, 10, info);

   setOne(rcEntertaimentGroup, 12, 37, 62); // amphitheater
   setOne(rcEntertaimentGroup, 35, 34, 37); // theater
   setOne(rcEntertaimentGroup, 50, 70, 105);  // collosseum

   // animations
   info.xoffset = 42;
   info.yoffset = 34;
   setRange(rcCommerceGroup, 2, 11, info);  // market poor
   info.xoffset = 66;
   info.yoffset = 44;
   setRange(rcCommerceGroup, 44, 53, info);  // marble
   info.xoffset = 45;
   info.yoffset = 18;
   setRange(rcCommerceGroup, 55, 60, info);  // iron
   info.xoffset = 15;
   info.yoffset = 32;
   setRange(rcCommerceGroup, 62, 71, info);  // clay
   info.xoffset = 35;
   info.yoffset = 6;
   setRange(rcCommerceGroup, 73, 82, info);  // timber
   info.xoffset = 14;
   info.yoffset = 36;
   setRange(rcCommerceGroup, 87, 98, info);  // wine
   info.xoffset = 0;
   info.yoffset = 45;
   setRange(rcCommerceGroup, 100, 107, info);  // oil
   info.xoffset = 42;
   info.yoffset = 36;
   setRange(rcCommerceGroup, 109, 116, info);  // weapons
   info.xoffset = 38;
   info.yoffset = 39;
   setRange(rcCommerceGroup, 118, 131, info);  // furniture
   info.xoffset = 65;
   info.yoffset = 42;
   setRange(rcCommerceGroup, 133, 139, info);  // pottery
   info.xoffset = 65;
   info.yoffset = 42;
   setRange(rcCommerceGroup, 159, 167, info);  // market rich

   // stock of input good
   setOne(rcCommerceGroup, 153, 45, -8);  // grapes
   setOne(rcCommerceGroup, 154, 37, -2);  // olive
   setOne(rcCommerceGroup, 155, 48, -4);  // timber
   setOne(rcCommerceGroup, 156, 47, -11);  // iron
   setOne(rcCommerceGroup, 157, 47, -9);  // clay

   // warehouse
   setOne(rcWarehouseGroup, 1, 60, 56);
   setOne(rcWarehouseGroup, 18, 56, 93);
   info.xoffset = 55;
   info.yoffset = 75;
   setRange(rcWarehouseGroup, 2, 17, info);
   info.xoffset = 79;
   info.yoffset = 108;
   setRange(rcWarehouseGroup, 84, 91, info);

   // granary
   setOne(rcCommerceGroup, 141, 28, 109);
   setOne(rcCommerceGroup, 142, 33, 75);
   setOne(rcCommerceGroup, 143, 56, 65);
   setOne(rcCommerceGroup, 144, 92, 65);
   setOne(rcCommerceGroup, 145, 118, 76);
   setOne(rcCommerceGroup, 146, 78, 69);
   setOne(rcCommerceGroup, 147, 78, 69);
   setOne(rcCommerceGroup, 148, 78, 69);
   setOne(rcCommerceGroup, 149, 78, 69);
   setOne(rcCommerceGroup, 150, 78, 69);
   setOne(rcCommerceGroup, 151, 78, 69);
   setOne(rcCommerceGroup, 152, 78, 69);

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


PicLoader* PicLoader::_instance = NULL;

PicLoader& PicLoader::instance()
{
   if (_instance == NULL)
   {
      _instance = new PicLoader();
      if (_instance == NULL) THROW("Memory error, cannot instantiate object");
   }
   return *_instance;
}

PicLoader::PicLoader() { }


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

   return get_picture("paneling", pic_index);
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
   std::string aPath = "resources/pics/";
   load_archive(aPath+"pics_wait.zip");

   std::cout << "number of images loaded: " << _resources.size() << std::endl;
}

void PicLoader::load_all()
{
   std::string aPath = "resources/pics/";
   load_archive(aPath+"pics.zip");
   load_archive(aPath+"pics_oc3.zip");	
   std::cout << "number of images loaded: " << _resources.size() << std::endl;
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
   if (rc != ARCHIVE_OK) THROW("Error while reading archive " << filename);
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


int PicLoader::get_pic_id_by_name(std::string &pic_name)
{
   // example: for land1a_00004.png, return 244+4=248
   std::string res_pfx;  // resource name prefix = land1a
   int res_id;   // idx of resource = 4

   // extract the name and idx from name (ex: [land1a, 4])
   int pos = pic_name.find("_");
   res_pfx = pic_name.substr(0, pos);
   std::stringstream ss(pic_name.substr(pos+1));
   ss >> res_id;

   if (res_pfx == "plateau")
   {
      res_id += 200;
   }
   else if (res_pfx == "land1a")
   {
      res_id += 244;
   }
   else if (res_pfx == "land2a")
   {
      res_id += 547;
   }
   else if (res_pfx == "land3a")
   {
      res_id += 778;
   }
   else
   {
      THROW("Unknown image " << pic_name);
   }

   return res_id;
}


Picture& PicLoader::get_pic_by_id(const int imgId)
{
   // example: for land1a_00004.png, pfx=land1a and id=4
   std::string res_pfx;  // resource name prefix
   int res_id = imgId;   // id of resource

   if (201<=imgId && imgId <245)
   {
      res_pfx = "plateau";
      res_id = imgId-200;
   }
   else if (245<=imgId && imgId <548)
   {
      res_pfx = "land1a";
      res_id = imgId-244;
   }
   else if (548<=imgId && imgId <779)
   {
      res_pfx = "land2a";
      res_id = imgId-547;
   }
   else if (779<=imgId && imgId <871)
   {
      res_pfx = "land3a";
      res_id = imgId-778;
   }
   else
   {
      res_pfx = "land1a";
      res_id = 1;
      // THROW("Unknown image Id " << imgId);
   }

   return get_picture(res_pfx, res_id);
}


AnimLoader::AnimLoader(PicLoader &loader)
{
   _loader = &loader;
}

void AnimLoader::fill_animation(Animation &oAnim, const std::string &prefix, const int start, const int number, const int step)
{
   std::vector<Picture*> pictures;
   for (int i = 0; i < number; ++i)
   {
      Picture &pic = _loader->get_picture(prefix, start+i*step);
      pictures.push_back(&pic);
   }
   oAnim.init(pictures);
}

void AnimLoader::fill_animation_reverse(Animation &oAnim, const std::string &prefix, const int start, const int number, const int step)
{
   std::vector<Picture*> pictures;
   for (int i = 0; i < number; ++i)
   {
      Picture &pic = _loader->get_picture(prefix, start - i * step);
      pictures.push_back(&pic);
   }
   oAnim.init(pictures);
}

void AnimLoader::change_offset(Animation &ioAnim, const int xoffset, const int yoffset)
{
   std::vector<Picture*>::iterator it;
   for (it = ioAnim.get_pictures().begin(); it != ioAnim.get_pictures().end(); ++it)
   {
      Picture &pic = **it;
      pic.set_offset(xoffset, yoffset);
   }
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
   fillWalk(map, rcCartsGroup, 145, 12);
   _animations[WG_HORSE_CARAVAN] = map;

   map.clear();
   fillWalk(map, rcCartsGroup, 273, 12);
   _animations[WG_CAMEL_CARAVAN] = map;

   map.clear();
   fillWalk(map, rcCartsGroup, 369, 12);
   _animations[WG_LITTLE_HELPER] = map;

}

void WalkerLoader::fillWalk(std::map<WalkerAction, Animation> &ioMap, const std::string &prefix, const int start, const int size)
{
   AnimLoader animLoader(PicLoader::instance());

   WalkerAction action;
   action._action = WA_MOVE;

   action._direction = D_NORTH;
   animLoader.fill_animation(ioMap[action], prefix, start  , size, 8);
   action._direction = D_NORTH_EAST;
   animLoader.fill_animation(ioMap[action], prefix, start+1, size, 8);
   action._direction = D_EAST;
   animLoader.fill_animation(ioMap[action], prefix, start+2, size, 8);
   action._direction = D_SOUTH_EAST;
   animLoader.fill_animation(ioMap[action], prefix, start+3, size, 8);
   action._direction = D_SOUTH;
   animLoader.fill_animation(ioMap[action], prefix, start+4, size, 8);
   action._direction = D_SOUTH_WEST;
   animLoader.fill_animation(ioMap[action], prefix, start+5, size, 8);
   action._direction = D_WEST;
   animLoader.fill_animation(ioMap[action], prefix, start+6, size, 8);
   action._direction = D_NORTH_WEST;
   animLoader.fill_animation(ioMap[action], prefix, start+7, size, 8);
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

   fillCart(cart, rcCartsGroup, 1, frontCart);
   _carts[G_NONE] = cart;
   fillCart(cart, rcCartsGroup, 9, frontCart);
   _carts[G_WHEAT] = cart;
   fillCart(cart, rcCartsGroup, 17, frontCart);
   _carts[G_VEGETABLE] = cart;
   fillCart(cart, rcCartsGroup, 25, frontCart);
   _carts[G_FRUIT] = cart;
   fillCart(cart, rcCartsGroup, 33, frontCart);
   _carts[G_OLIVE] = cart;
   fillCart(cart, rcCartsGroup, 41, frontCart);
   _carts[G_GRAPE] = cart;
   fillCart(cart, rcCartsGroup, 49, frontCart);
   _carts[G_MEAT] = cart;
   fillCart(cart, rcCartsGroup, 57, frontCart);
   _carts[G_WINE] = cart;
   fillCart(cart, rcCartsGroup, 65, frontCart);
   _carts[G_OIL] = cart;
   fillCart(cart, rcCartsGroup, 73, frontCart);
   _carts[G_IRON] = cart;
   fillCart(cart, rcCartsGroup, 81, frontCart);
   _carts[G_TIMBER] = cart;
   fillCart(cart, rcCartsGroup, 89, frontCart);
   _carts[G_CLAY] = cart;
   fillCart(cart, rcCartsGroup, 97, frontCart);
   _carts[G_MARBLE] = cart;
   fillCart(cart, rcCartsGroup, 105, frontCart);
   _carts[G_WEAPON] = cart;
   fillCart(cart, rcCartsGroup, 113, frontCart);
   _carts[G_FURNITURE] = cart;
   fillCart(cart, rcCartsGroup, 121, frontCart);
   _carts[G_POTTERY] = cart;
   fillCart(cart, rcCartsGroup, 129, !frontCart);
   _carts[G_SCARB1] = cart;
   fillCart(cart, rcCartsGroup, 137, !frontCart);
   _carts[G_SCARB2] = cart;
   fillCart(cart, rcCartsGroup, 697, frontCart);
   _carts[G_FISH] = cart;
}

namespace{
static const Point frontCartOffsetSouth = /*Point( -33, 35 )*/Point(-33,22);
static const Point frontCartOffsetWest  = Point( -31, 35 );
static const Point frontCartOffsetNorth = Point( -5, 37 );
static const Point frontCartOffsetEast  = Point( -5, 22 );

static const Point backCartOffsetSouth = Point( -5, 40 );
static const Point backCartOffsetWest  = Point( -5, 22 );
static const Point backCartOffsetNorth = Point( -33, 22 );
static const Point backCartOffsetEast  = Point( -31, 35 );
}

void CartLoader::fillCart(std::vector<Picture*> &ioCart, const std::string &prefix, const int start, bool back )
{
   PicLoader &picLoader = PicLoader::instance();

   ioCart.clear();
   ioCart.resize(D_MAX);
   
   ioCart[D_NORTH]      = &picLoader.get_picture(rcCartsGroup, start);
   ioCart[D_NORTH_EAST] = &picLoader.get_picture(rcCartsGroup, start + 1);
   ioCart[D_EAST]       = &picLoader.get_picture(rcCartsGroup, start + 2);
   ioCart[D_SOUTH_EAST] = &picLoader.get_picture(rcCartsGroup, start + 3);
   ioCart[D_SOUTH]      = &picLoader.get_picture(rcCartsGroup, start + 4);
   ioCart[D_SOUTH_WEST] = &picLoader.get_picture(rcCartsGroup, start + 5);
   ioCart[D_WEST]       = &picLoader.get_picture(rcCartsGroup, start + 6);
   ioCart[D_NORTH_WEST] = &picLoader.get_picture(rcCartsGroup, start + 7);

   ioCart[D_SOUTH]->set_offset( back ? backCartOffsetSouth : frontCartOffsetSouth);
   ioCart[D_WEST]->set_offset ( back ? backCartOffsetWest  : frontCartOffsetWest );
   ioCart[D_NORTH]->set_offset( back ? backCartOffsetNorth : frontCartOffsetNorth);
   ioCart[D_EAST]->set_offset ( back ? backCartOffsetEast  : frontCartOffsetEast );
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


void FontLoader::load_all()
{
   std::string full_font_path = "FreeSerif.ttf";
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

