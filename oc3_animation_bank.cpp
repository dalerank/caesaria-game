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

#include "oc3_animation_bank.hpp"
#include "oc3_positioni.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_picture.hpp"
#include "oc3_stringhelper.hpp"
#include "oc3_walker_emigrant.hpp"

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

  static const int noneGoodsPicId = 1;
}

class AnimationBank::Impl
{
public:
  std::map< int, PicturesArray > carts;
  
  typedef std::vector< AnimationBank::WalkerAnimationMap > Animations;
  Animations animations; // anim[WalkerGraphic][WalkerAction]

  // fills the cart pictures
  // prefix: image prefix
  // start: index of the first frame
  PicturesArray fillCart( const std::string &prefix, const int start, bool back );
  AnimationBank::WalkerAnimationMap fillWalker( const std::string& prefix, const int start, const int size);

  void loadCarts();
  void loadWalkers();
};

void AnimationBank::Impl::loadCarts()
{
  //number of animations with goods + emmigrants + immigrants
  bool frontCart = false;

  carts[Good::G_NONE] = fillCart(ResourceGroup::carts, noneGoodsPicId, frontCart);
  carts[Good::G_WHEAT] = fillCart(ResourceGroup::carts, 9, frontCart);
  carts[Good::G_VEGETABLE] = fillCart(ResourceGroup::carts, 17, frontCart);
  carts[Good::G_FRUIT] = fillCart( ResourceGroup::carts, 25, frontCart);
  carts[Good::G_OLIVE] = fillCart( ResourceGroup::carts, 33, frontCart);
  carts[Good::G_GRAPE] = fillCart( ResourceGroup::carts, 41, frontCart);
  carts[Good::G_MEAT] = fillCart( ResourceGroup::carts, 49, frontCart);
  carts[Good::G_WINE] = fillCart( ResourceGroup::carts, 57, frontCart);
  carts[Good::G_OIL] = fillCart( ResourceGroup::carts, 65, frontCart);
  carts[Good::G_IRON] = fillCart( ResourceGroup::carts, 73, frontCart);
  carts[Good::G_TIMBER] = fillCart( ResourceGroup::carts, 81, frontCart);
  carts[Good::G_CLAY] =  fillCart( ResourceGroup::carts, 89, frontCart);
  carts[Good::G_MARBLE] = fillCart( ResourceGroup::carts, 97, frontCart);
  carts[Good::G_WEAPON] = fillCart( ResourceGroup::carts, 105, frontCart);
  carts[Good::G_FURNITURE] = fillCart( ResourceGroup::carts, 113, frontCart);
  carts[Good::G_POTTERY] = fillCart( ResourceGroup::carts, 121, frontCart);
  carts[Emigrant::G_EMIGRANT_CART1] = fillCart( ResourceGroup::carts, 129, !frontCart);
  carts[Emigrant::G_EMIGRANT_CART2] = fillCart( ResourceGroup::carts, 137, !frontCart);
  carts[Good::G_FISH] = fillCart( ResourceGroup::carts, 697, frontCart);
}

void AnimationBank::Impl::loadWalkers()
{
  animations.resize(30);  // number of walker types

  animations[WG_POOR] =     fillWalker( "citizen01", 1, 12 );
  animations[WG_BATH] =     fillWalker( "citizen01", 105, 12);
  animations[WG_PRIEST] =   fillWalker( "citizen01", 209, 12);
  animations[WG_ACTOR] =    fillWalker( "citizen01", 313, 12);
  animations[WG_TAMER] =    fillWalker( "citizen01", 417, 12);
  animations[WG_TAX] =      fillWalker( "citizen01", 617, 12);
  animations[WG_CHILD] =    fillWalker("citizen01", 721, 12);
  animations[WG_MARKETLADY] = fillWalker( "citizen01", 825, 12);
  animations[WG_PUSHER] =     fillWalker( "citizen01", 929, 12);
  animations[WG_PUSHER2] =    fillWalker( "citizen01", 1033, 12);
  animations[WG_ENGINEER] =   fillWalker( "citizen01", 1137, 12);
  animations[WG_GLADIATOR] =  fillWalker( "citizen02", 1, 12);
  animations[WG_GLADIATOR2] =         fillWalker( "citizen02", 199, 12);
  animations[WG_RIOTER] =             fillWalker( "citizen02", 351, 12);
  animations[WG_BARBER] =             fillWalker( "citizen02", 463, 12);
  animations[WG_PREFECT] =            fillWalker( "citizen02", 615, 12);
  animations[WG_PREFECT_DRAG_WATER] = fillWalker( "citizen02", 767, 12);
  animations[WG_PREFECT_FIGHTS_FIRE] =fillWalker( "citizen02", 863, 6);
  animations[WG_HOMELESS] =         fillWalker( "citizen02", 911, 12);
  animations[WG_RICH] =             fillWalker( "citizen03", 713, 12);
  animations[WG_DOCTOR] =           fillWalker( "citizen03", 817, 12);
  animations[WG_RICH2] =            fillWalker( "citizen03", 921, 12);
  animations[WG_LIBRARIAN] =        fillWalker( "citizen03", 1025, 12);
  animations[WG_SOLDIER] =          fillWalker( "citizen03", 553, 12);
  animations[WG_JAVELINEER] =       fillWalker( "citizen03", 241, 12);
  animations[WG_HORSEMAN] =         fillWalker( "citizen04", 1, 12);
  animations[WG_HORSE_CARAVAN] =    fillWalker( ResourceGroup::carts, 145, 12);
  animations[WG_CAMEL_CARAVAN] =    fillWalker( ResourceGroup::carts, 273, 12);
  animations[WG_MARKETLADY_HELPER] =fillWalker( ResourceGroup::carts, 369, 12);
  animations[WG_ANIMAL_SHEEP] =     fillWalker( ResourceGroup::animals, 153, 12 );
}

AnimationBank& AnimationBank::instance()
{
  static AnimationBank inst;
  return inst;
}

AnimationBank::AnimationBank() : _d( new Impl )
{
}

void AnimationBank::loadCarts()
{
  StringHelper::debug( 0xff, "Loading cart graphics" );

  instance()._d->loadCarts();  
}

AnimationBank::WalkerAnimationMap AnimationBank::Impl::fillWalker( const std::string& prefix, const int start, const int size)
{
  WalkerAnimationMap ioMap;
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

  return ioMap;
}

const AnimationBank::WalkerAnimationMap& AnimationBank::getWalker(const WalkerGraphicType walkerGraphic)
{
  return instance()._d->animations[walkerGraphic];
}

void AnimationBank::loadWalkers()
{
  StringHelper::debug( 0xff, "Loading cart graphics" );

  instance()._d->loadWalkers();
}

PicturesArray AnimationBank::Impl::fillCart( const std::string &prefix, const int start, bool back )
{
  PicturesArray ioCart;

  ioCart.resize(D_MAX);

  ioCart[D_NORTH]      = Picture::load(ResourceGroup::carts, start);
  ioCart[D_NORTH_EAST] = Picture::load(ResourceGroup::carts, start + 1);
  ioCart[D_EAST]       = Picture::load(ResourceGroup::carts, start + 2);
  ioCart[D_SOUTH_EAST] = Picture::load(ResourceGroup::carts, start + 3);
  ioCart[D_SOUTH]      = Picture::load(ResourceGroup::carts, start + 4);
  ioCart[D_SOUTH_WEST] = Picture::load(ResourceGroup::carts, start + 5);
  ioCart[D_WEST]       = Picture::load(ResourceGroup::carts, start + 6);
  ioCart[D_NORTH_WEST] = Picture::load(ResourceGroup::carts, start + 7);

  ioCart[D_SOUTH].setOffset( back ? backCartOffsetSouth : frontCartOffsetSouth);
  ioCart[D_WEST].setOffset ( back ? backCartOffsetWest  : frontCartOffsetWest );
  ioCart[D_NORTH].setOffset( back ? backCartOffsetNorth : frontCartOffsetNorth);
  ioCart[D_EAST].setOffset ( back ? backCartOffsetEast  : frontCartOffsetEast );

  ioCart[D_SOUTH_EAST].setOffset ( back ? backCartOffsetSouthEast  : frontCartOffsetSouthEast );
  ioCart[D_NORTH_WEST].setOffset ( back ? backCartOffsetNorthWest  : frontCartOffsetNorthWest );
  ioCart[D_NORTH_EAST].setOffset ( back ? backCartOffsetNorthEast  : frontCartOffsetNorthEast );
  ioCart[D_SOUTH_WEST].setOffset ( back ? backCartOffsetSouthWest  : frontCartOffsetSouthWest );

  return ioCart;
}

const Picture& AnimationBank::getCart( int cartId, const DirectionType &direction)
{
  return instance()._d->carts.at( cartId ).at( direction );
}
