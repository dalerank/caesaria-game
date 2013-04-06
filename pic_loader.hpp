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


#ifndef PIC_LOADER_HPP
#define PIC_LOADER_HPP

#include <string>
#include <map>
#include <vector>

#include "picture.hpp"
#include "gfx_engine.hpp"
#include "enums.hpp"
#include "good.hpp"


class PicInfo
{
public:
   int xoffset, yoffset;  // if (-1) : this is a tile picture
};

// contains data needed for loading pictures
class PicMetaData
{
public:
   static PicMetaData& instance();

   PicInfo& get_data(const std::string &resource_name);   // image name ("Govt_00005")

private:
   PicMetaData();
   void setRange(const std::string &preffix, const int first, const int last, PicInfo &data);
   void setOne(const std::string &preffix, const int index, PicInfo &data);
   void setOne(const std::string &preffix, const int index, const int xoffset, const int yoffset);
   static PicMetaData* _instance;

   std::map<std::string, PicInfo> _data;   // key=image name (Govt_00005)
   PicInfo _dummy_data;
};


// loads pictures from files
class PicLoader
{
public:
   static PicLoader& instance();

   // set the current picture
   void set_picture(const std::string &name, SDL_Surface &surface);

   // show resource
   Picture& get_picture(const std::string &name);

   // show resource
   Picture& get_picture(const std::string &prefix, const int idx);

   // returns the picture of the given good type
   Picture& get_picture_good(const GoodType goodType);

   // show all resources
   std::list<Picture*> get_pictures();

   // loads all resources during load
   void load_wait();
   // loads all resources
   void load_all();

   // loads all resources of the given archive file
   void load_archive(const std::string &filename);

   // used for game save. land1a_00004.png returns 244+4=248
   int get_pic_id_by_name(std::string &pic_name);

   // used for game load. 248 => land1a_00004.png
   Picture& get_pic_by_id(const int imgId);


private:
   PicLoader();
   static PicLoader* _instance;

   Picture make_picture(SDL_Surface *surface, const std::string& resource_name) const;

   std::map<std::string, Picture> _resources;  // key=image name, value=picture
};



class AnimLoader
{
public:
   AnimLoader(PicLoader &loader);

   // get animation from resources. example: prefix=buildings/Commerce , start=18, number=5, step=1
   // it will return 5 Pictures (18, 19, 20, 21, 22)
   void fill_animation(Animation& oAnim, const std::string &prefix, const int start, const int number, const int step = 1);
   void change_offset(Animation& ioAnim, const int xoffset, const int yoffset);
   void fill_animation_reverse(Animation&, const std::string&, const int, const int, const int step = 1);
private:
   PicLoader* _loader;
};


struct WalkerAction
{
   WalkerActionType _action;
   DirectionType _direction;
};
bool operator<(const WalkerAction &v1, const WalkerAction &v2);

class WalkerLoader
{
public:
   static WalkerLoader& instance();

   // loads all walker animations
   void loadAll();

   // fills the walking animations
   // prefix: image prefix
   // start: index of the first frame
   // size: number of frames for the walking movement
   void fillWalk(std::map<WalkerAction, Animation> &ioMap, const std::string &prefix, const int start, const int size);

   const std::map<WalkerAction, Animation>& getAnimationMap(const WalkerGraphicType walkerGraphic);

private:
   WalkerLoader();
   static WalkerLoader* _instance;

   std::vector<std::map<WalkerAction, Animation> > _animations; // anim[WalkerGraphic][WalkerAction]
};


class CartLoader
{
public:
   static CartLoader& instance();

   // loads all cart graphics
   void loadAll();

   // fills the cart pictures
   // prefix: image prefix
   // start: index of the first frame
   void fillCart(std::vector<Picture*> &ioCart, const std::string &prefix, const int start, bool back );

   Picture& getCart(const GoodStock &stock, const DirectionType &direction);
   Picture& getCart(GoodType cart, const DirectionType &direction );
   // for emmigration & immigration
   Picture& getCart(CartTypes cart, const DirectionType &direction);

private:
   CartLoader();
   static CartLoader* _instance;

   std::vector<std::vector<Picture*> > _carts; // pictures[GoodType][Direction]
};


class FontLoader
{
public:
   FontLoader();

   void load_all();
};


#endif
