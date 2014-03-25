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
// You should have received a copy of the GNU General Public License
// along with CaesarIA.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com

#include "gfx/animation_bank.hpp"
#include "core/position.hpp"
#include "game/resourcegroup.hpp"
#include "gfx/picture.hpp"
#include "core/logger.hpp"
#include "walker/emigrant.hpp"
#include "core/saveadapter.hpp"
#include "gfx/typehelper.hpp"
#include <map>

using namespace constants;

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
  
  typedef std::map< gfx::Type, AnimationBank::MovementAnimation > Animations;
  Animations animations; // anim[WalkerGraphic][WalkerAction]

  // fills the cart pictures
  // prefix: image prefix
  // start: index of the first frame
  PicturesArray fillCart( const std::string &prefix, const int start, bool back );
  AnimationBank::MovementAnimation loadAnimation(const std::string& prefix,
                                                 const int start, const int size,
                                                 Walker::Action wa=Walker::acMove );
  AnimationBank::MovementAnimation loadAnimation( const VariantList& desc );

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
  animations[gfx::unknown          ] = AnimationBank::MovementAnimation();
  animations[gfx::citizenGo        ] = loadAnimation( ResourceGroup::citizen1, 1, 12 );
  animations[gfx::bathladyGo       ] = loadAnimation( ResourceGroup::citizen1, 105, 12);
  animations[gfx::priest           ] = loadAnimation( ResourceGroup::citizen1, 209, 12);
  animations[gfx::actor            ] = loadAnimation( ResourceGroup::citizen1, 313, 12);
  animations[gfx::tamer            ] = loadAnimation( ResourceGroup::citizen1, 417, 12);
  animations[gfx::taxCollector     ] = loadAnimation( ResourceGroup::citizen1, 617, 12);
  animations[gfx::scholar          ] = loadAnimation( ResourceGroup::citizen1, 721, 12);
  animations[gfx::marketlady       ] = loadAnimation( ResourceGroup::citizen1, 825, 12);
  animations[gfx::cartPusher       ] = loadAnimation( ResourceGroup::citizen1, 929, 12);
  animations[gfx::cartPusher2      ] = loadAnimation( ResourceGroup::citizen1, 1033, 12);
  animations[gfx::engineer         ] = loadAnimation( ResourceGroup::citizen1, 1137, 12);
  animations[gfx::gladiator        ] = loadAnimation( ResourceGroup::citizen2, 1, 12);
  animations[gfx::gladiator2       ] = loadAnimation( ResourceGroup::citizen2, 199, 12);
  animations[gfx::protestor        ] = loadAnimation( ResourceGroup::citizen2, 351, 12);
  animations[gfx::barber           ] = loadAnimation( ResourceGroup::citizen2, 463, 12);
  animations[gfx::prefect          ] = loadAnimation( ResourceGroup::citizen2, 615, 12);
  animations[gfx::prefectDragWater ] = loadAnimation( ResourceGroup::citizen2, 767, 12);
  animations[gfx::prefectFightFire ] = loadAnimation( ResourceGroup::citizen2, 863, 6, Walker::acFight );
  animations[gfx::prefectFight     ] = loadAnimation( ResourceGroup::citizen2, 719, 6, Walker::acFight );
  animations[gfx::homeless         ] = loadAnimation( ResourceGroup::citizen2, 911, 12);
  animations[gfx::patrician        ] = loadAnimation( ResourceGroup::citizen3, 713, 12);
  animations[gfx::doctor           ] = loadAnimation( ResourceGroup::citizen3, 817, 12);
  animations[gfx::patrician2       ] = loadAnimation( ResourceGroup::citizen3, 921, 12);
  animations[gfx::teacher          ] = loadAnimation( ResourceGroup::citizen3, 1025, 12);
  animations[gfx::soldier          ] = loadAnimation( ResourceGroup::citizen3, 553, 12);
  animations[gfx::javelineer       ] = loadAnimation( ResourceGroup::citizen3, 241, 12);
  animations[gfx::horseman         ] = loadAnimation( ResourceGroup::citizen4, 1, 12);
  animations[gfx::horseMerchant    ] = loadAnimation( ResourceGroup::carts, 145, 12);
  animations[gfx::camelMerchant    ] = loadAnimation( ResourceGroup::carts, 273, 12);
  animations[gfx::marketkid        ] = loadAnimation( ResourceGroup::carts, 369, 12);
  animations[gfx::sheep            ] = loadAnimation( ResourceGroup::animals, 153, 5 );
  animations[gfx::fishingBoat      ] = loadAnimation( ResourceGroup::carts, 249, 1 );
  animations[gfx::fishingBoatWork  ] = loadAnimation( ResourceGroup::carts, 257, 1 );
  animations[gfx::homelessSit      ] = loadAnimation( ResourceGroup::citizen2, 1015, 1 );
  animations[gfx::lion             ] = loadAnimation( ResourceGroup::lion, 1, 12 );
  animations[gfx::charioterMove        ] = loadAnimation( ResourceGroup::citizen5, 1, 12 );
  animations[gfx::britonSoldier    ] = loadAnimation( ResourceGroup::celts, 249, 12 );
  animations[gfx::britonSoldierFight]= loadAnimation( ResourceGroup::celts, 345, 6, Walker::acFight );
  animations[gfx::soldier          ] = loadAnimation( ResourceGroup::citizen3, 97, 12 );
  animations[gfx::legionaryMove     ] = loadAnimation( ResourceGroup::citizen3, 553, 12 );
  animations[gfx::legionaryFight   ] = loadAnimation( ResourceGroup::citizen3, 649, 6, Walker::acFight );
  animations[gfx::guardMove          ] = loadAnimation( ResourceGroup::citizen3, 97, 12 );
  animations[gfx::guardFigth       ] = loadAnimation( ResourceGroup::citizen3, 192, 6, Walker::acFight );
  animations[gfx::seaMerchantMove      ] = loadAnimation( ResourceGroup::carts, 241, 1 );
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

  _d->loadCarts();
}

AnimationBank::MovementAnimation AnimationBank::Impl::loadAnimation( const std::string& prefix,
                                                                     const int start, const int size,
                                                                     Walker::Action wa )
{
  MovementAnimation ioMap;
  DirectedAction action={ wa, north };

  action.direction = north;      ioMap[action].load( prefix, start,   size, Animation::straight, 8);
  action.direction = northEast;  ioMap[action].load( prefix, start+1, size, Animation::straight, 8);
  action.direction = east;       ioMap[action].load( prefix, start+2, size, Animation::straight, 8);
  action.direction = southEast;  ioMap[action].load( prefix, start+3, size, Animation::straight, 8);
  action.direction = south;      ioMap[action].load( prefix, start+4, size, Animation::straight, 8);
  action.direction = southWest;  ioMap[action].load( prefix, start+5, size, Animation::straight, 8);
  action.direction = west;       ioMap[action].load( prefix, start+6, size, Animation::straight, 8);
  action.direction = northWest;  ioMap[action].load( prefix, start+7, size, Animation::straight, 8);

  return ioMap;
}

AnimationBank::MovementAnimation AnimationBank::Impl::loadAnimation(const VariantList& desc)
{
  std::string rcGroup = desc.get( 0 ).toString();
  int startIndex = desc.get( 1 );
  int frameNumber = desc.get( 2 );

  return loadAnimation( rcGroup, startIndex, frameNumber );
}

const AnimationBank::MovementAnimation& AnimationBank::getWalker( gfx::Type anim)
{
  AnimationBank& inst = instance();
  if( anim >= inst._d->animations.size() )
  {
    Logger::warning( "Can't find animation map for type %d", anim );
    return inst._d->animations[ gfx::unknown ];
  }

  return inst._d->animations[ anim ];
}

void AnimationBank::loadWalkers()
{
  Logger::warning( "Start loading walkers graphics" );
  _d->loadWalkers();
}

void AnimationBank::loadAnimation(vfs::Path model)
{
  VariantMap items = SaveAdapter::load( model );

  foreach( i, items )
  {
    std::string baseType = i->first;

    VariantMap actions = i->second.toMap();

    foreach( ac, actions )
    {
      std::string actionName = baseType + ac->first;

      gfx::Type type = GfxTypeHelper::instance().findType( actionName );
      if( type != gfx::unknown )
      {
        _d->animations[ type ] = _d->loadAnimation( ac->second.toList() );
      }
    }

  }
}

PicturesArray AnimationBank::Impl::fillCart( const std::string &prefix, const int start, bool back )
{
  PicturesArray ioCart;

  ioCart.resize(countDirection);

  ioCart[north]      = Picture::load(ResourceGroup::carts, start);
  ioCart[northEast]  = Picture::load(ResourceGroup::carts, start + 1);
  ioCart[east]       = Picture::load(ResourceGroup::carts, start + 2);
  ioCart[southEast]  = Picture::load(ResourceGroup::carts, start + 3);
  ioCart[south]      = Picture::load(ResourceGroup::carts, start + 4);
  ioCart[southWest]  = Picture::load(ResourceGroup::carts, start + 5);
  ioCart[west]       = Picture::load(ResourceGroup::carts, start + 6);
  ioCart[northWest]  = Picture::load(ResourceGroup::carts, start + 7);

  ioCart[south].setOffset( back ? backCartOffsetSouth : frontCartOffsetSouth);
  ioCart[west].setOffset ( back ? backCartOffsetWest  : frontCartOffsetWest );
  ioCart[north].setOffset( back ? backCartOffsetNorth : frontCartOffsetNorth);
  ioCart[east].setOffset ( back ? backCartOffsetEast  : frontCartOffsetEast );

  ioCart[southEast].setOffset( back ? backCartOffsetSouthEast : frontCartOffsetSouthEast );
  ioCart[northWest].setOffset( back ? backCartOffsetNorthWest : frontCartOffsetNorthWest );
  ioCart[northEast].setOffset( back ? backCartOffsetNorthEast : frontCartOffsetNorthEast );
  ioCart[southWest].setOffset( back ? backCartOffsetSouthWest : frontCartOffsetSouthWest );

  return ioCart;
}

const Picture& AnimationBank::getCart(int cartId, constants::Direction direction)
{
  return instance()._d->carts[ cartId ][ direction ];
}
