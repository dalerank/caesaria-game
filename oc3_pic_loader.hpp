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


#ifndef __OPENCAESAR3_PICLOADER_H_INCLUDED__
#define __OPENCAESAR3_PICLOADER_H_INCLUDED__

#include <string>
#include <map>
#include <vector>

#include "oc3_picture.hpp"
#include "oc3_gfx_engine.hpp"
#include "oc3_enums.hpp"
#include "oc3_good.hpp"
#include "oc3_scopedptr.hpp"


// contains data needed for loading pictures
class PicMetaData
{
public:
  static PicMetaData& instance();
  ~PicMetaData();

  Point get(const std::string &resource_name);   // image name ("Govt_00005")

private:
  PicMetaData();

  class Impl;
  ScopedPtr< Impl > _d;
};


// loads pictures from files
class PicLoader
{
public:
  static PicLoader& instance();

  // set the current picture
  void setPicture(const std::string &name, SDL_Surface &surface);
  void setPicture(const std::string &name, const Picture& picture);

  // show resource
  Picture& getPicture(const std::string &name);

  // show resource
  Picture& getPicture(const std::string &prefix, const int idx);

  // returns the picture of the given good type
  Picture& getPicture(const GoodType goodType);

  // show all resources
  PicturesArray getPictures();

  // loads all resources during load
  void loadWaitPics();
  // loads all resources
  void loadAllPics();

  // create runtime resources
  void createResources();

  // loads all resources of the given archive file
  void loadArchive(const std::string &filename);

private:
  PicLoader();
  static PicLoader* _instance;

  Picture makePicture(SDL_Surface *surface, const std::string& resource_name) const;

  std::map<std::string, Picture> _resources;  // key=image name, value=picture
};

class Animation;

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
   void fillWalk( std::map<WalkerAction, Animation> &ioMap, const std::string &prefix, const int start, const int size);

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

  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //__OPENCAESAR3_PICLOADER_H_INCLUDED__
