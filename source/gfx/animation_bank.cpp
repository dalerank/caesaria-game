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

#include "gfx/animation_bank.hpp"
#include "core/position.hpp"
#include "game/resourcegroup.hpp"
#include "gfx/picture.hpp"
#include "core/logger.hpp"
#include "walker/emigrant.hpp"
#include <vector>

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
  
  typedef std::vector< AnimationBank::MovementAnimation > Animations;
  Animations animations; // anim[WalkerGraphic][WalkerAction]

  // fills the cart pictures
  // prefix: image prefix
  // start: index of the first frame
  PicturesArray fillCart( const std::string &prefix, const int start, bool back );
  AnimationBank::MovementAnimation loadAnimation( const std::string& prefix, const int start, const int size);

  void loadCarts();
  void loadWalkers();
};

void AnimationBank::Impl::loadCarts()
{
  //number of animations with goods + emmigrants + immigrants
  bool frontCart = false;

  carts[Good::none] = fillCart(ResourceGroup::carts, noneGoodsPicId, frontCart);
  carts[Good::wheat] = fillCart(ResourceGroup::carts, 9, frontCart);
  carts[Good::vegetable] = fillCart(ResourceGroup::carts, 17, frontCart);
  carts[Good::fruit] = fillCart( ResourceGroup::carts, 25, frontCart);
  carts[Good::olive] = fillCart( ResourceGroup::carts, 33, frontCart);
  carts[Good::grape] = fillCart( ResourceGroup::carts, 41, frontCart);
  carts[Good::meat] = fillCart( ResourceGroup::carts, 49, frontCart);
  carts[Good::wine] = fillCart( ResourceGroup::carts, 57, frontCart);
  carts[Good::oil] = fillCart( ResourceGroup::carts, 65, frontCart);
  carts[Good::iron] = fillCart( ResourceGroup::carts, 73, frontCart);
  carts[Good::timber] = fillCart( ResourceGroup::carts, 81, frontCart);
  carts[Good::clay] =  fillCart( ResourceGroup::carts, 89, frontCart);
  carts[Good::marble] = fillCart( ResourceGroup::carts, 97, frontCart);
  carts[Good::weapon] = fillCart( ResourceGroup::carts, 105, frontCart);
  carts[Good::furniture] = fillCart( ResourceGroup::carts, 113, frontCart);
  carts[Good::pottery] = fillCart( ResourceGroup::carts, 121, frontCart);
  carts[Emigrant::G_EMIGRANT_CART1] = fillCart( ResourceGroup::carts, 129, !frontCart);
  carts[Emigrant::G_EMIGRANT_CART2] = fillCart( ResourceGroup::carts, 137, !frontCart);
  carts[Good::fish] = fillCart( ResourceGroup::carts, 697, frontCart);
}

void AnimationBank::Impl::loadWalkers()
{
  animations.resize( WG_MAX );

  animations[WG_NONE              ] = AnimationBank::MovementAnimation();
  animations[WG_POOR              ] = loadAnimation( ResourceGroup::citizen1, 1, 12 );
  animations[WG_BATH              ] = loadAnimation( ResourceGroup::citizen1, 105, 12);
  animations[WG_PRIEST            ] = loadAnimation( ResourceGroup::citizen1, 209, 12);
  animations[WG_ACTOR             ] = loadAnimation( ResourceGroup::citizen1, 313, 12);
  animations[WG_TAMER             ] = loadAnimation( ResourceGroup::citizen1, 417, 12);
  animations[WG_TAX               ] = loadAnimation( ResourceGroup::citizen1, 617, 12);
  animations[WG_CHILD             ] = loadAnimation( ResourceGroup::citizen1, 721, 12);
  animations[WG_MARKETLADY        ] = loadAnimation( ResourceGroup::citizen1, 825, 12);
  animations[WG_PUSHER            ] = loadAnimation( ResourceGroup::citizen1, 929, 12);
  animations[WG_PUSHER2           ] = loadAnimation( ResourceGroup::citizen1, 1033, 12);
  animations[WG_ENGINEER          ] = loadAnimation( ResourceGroup::citizen1, 1137, 12);
  animations[WG_GLADIATOR         ] = loadAnimation( ResourceGroup::citizen2, 1, 12);
  animations[WG_GLADIATOR2        ] = loadAnimation( ResourceGroup::citizen2, 199, 12);
  animations[WG_RIOTER            ] = loadAnimation( ResourceGroup::citizen2, 351, 12);
  animations[WG_BARBER            ] = loadAnimation( ResourceGroup::citizen2, 463, 12);
  animations[WG_PREFECT           ] = loadAnimation( ResourceGroup::citizen2, 615, 12);
  animations[WG_PREFECT_DRAG_WATER] = loadAnimation( ResourceGroup::citizen2, 767, 12);
  animations[WG_PREFECT_FIGHTS_FIRE]= loadAnimation( ResourceGroup::citizen2, 863, 6);
  animations[WG_PREFECT_FIGHT     ] = loadAnimation( ResourceGroup::citizen2, 719, 6);
  animations[WG_HOMELESS          ] = loadAnimation( ResourceGroup::citizen2, 911, 12);
  animations[WG_RICH              ] = loadAnimation( ResourceGroup::citizen3, 713, 12);
  animations[WG_DOCTOR            ] = loadAnimation( ResourceGroup::citizen3, 817, 12);
  animations[WG_RICH2             ] = loadAnimation( ResourceGroup::citizen3, 921, 12);
  animations[WG_LIBRARIAN         ] = loadAnimation( ResourceGroup::citizen3, 1025, 12);
  animations[WG_SOLDIER           ] = loadAnimation( ResourceGroup::citizen3, 553, 12);
  animations[WG_JAVELINEER        ] = loadAnimation( ResourceGroup::citizen3, 241, 12);
  animations[WG_HORSEMAN          ] = loadAnimation( ResourceGroup::citizen4, 1, 12);
  animations[WG_HORSE_CARAVAN     ] = loadAnimation( ResourceGroup::carts, 145, 12);
  animations[WG_CAMEL_CARAVAN     ] = loadAnimation( ResourceGroup::carts, 273, 12);
  animations[WG_MARKETLADY_HELPER ] = loadAnimation( ResourceGroup::carts, 369, 12);
  animations[WG_ANIMAL_SHEEP_WALK ] = loadAnimation( ResourceGroup::animals, 153, 5 );
  animations[WG_FISHING_BOAT      ] = loadAnimation( ResourceGroup::carts, 249, 1 );
  animations[WG_FISHING_BOAT_WORK ] = loadAnimation( ResourceGroup::carts, 257, 1 );
  animations[WG_HOMELESS_SIT      ] = loadAnimation( ResourceGroup::citizen2, 1015, 1 );
  animations[WG_LION              ] = loadAnimation( ResourceGroup::lion, 1, 12 );
  animations[WG_CHARIOT           ] = loadAnimation( ResourceGroup::citizen5, 1, 12 );
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
  Logger::warning( "Loading cart graphics" );

  instance()._d->loadCarts();  
}

AnimationBank::MovementAnimation AnimationBank::Impl::loadAnimation( const std::string& prefix, const int start, const int size)
{
  MovementAnimation ioMap;
  DirectedAction action= { Walker::acMove, D_NORTH };

  action.direction = D_NORTH;      ioMap[action].load( prefix, start,   size, Animation::straight, 8);
  action.direction = D_NORTH_EAST; ioMap[action].load( prefix, start+1, size, Animation::straight, 8);
  action.direction = D_EAST;       ioMap[action].load( prefix, start+2, size, Animation::straight, 8);
  action.direction = D_SOUTH_EAST; ioMap[action].load( prefix, start+3, size, Animation::straight, 8);
  action.direction = D_SOUTH;      ioMap[action].load( prefix, start+4, size, Animation::straight, 8);
  action.direction = D_SOUTH_WEST; ioMap[action].load( prefix, start+5, size, Animation::straight, 8);
  action.direction = D_WEST;       ioMap[action].load( prefix, start+6, size, Animation::straight, 8);
  action.direction = D_NORTH_WEST; ioMap[action].load( prefix, start+7, size, Animation::straight, 8);

  return ioMap;
}

const AnimationBank::MovementAnimation& AnimationBank::getWalker(const WalkerGraphicType walkerGraphic)
{
  AnimationBank& inst = instance();
  if( walkerGraphic >= inst._d->animations.size() )
  {
    Logger::warning( "Can't find animation map for type %d", walkerGraphic );
    return inst._d->animations[ WG_NONE ];
  }

  return inst._d->animations[ walkerGraphic ];
}

void AnimationBank::loadWalkers()
{
  Logger::warning( "Start loading walkers graphics" );
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
