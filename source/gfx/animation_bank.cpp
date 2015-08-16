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
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com


#include "gfx/animation_bank.hpp"
#include "core/position.hpp"
#include "game/resourcegroup.hpp"
#include "gfx/picture.hpp"
#include "core/logger.hpp"
#include "core/saveadapter.hpp"
#include "good/helper.hpp"
#include "walker/helper.hpp"
#include "picture_info_bank.hpp"
#include "core/utils.hpp"
#include "core/variant_map.hpp"
#include "core/hash.hpp"

using namespace direction;

namespace gfx
{

namespace
{
  CAESARIA_LITERALCONST( small )
  CAESARIA_LITERALCONST( big )
  CAESARIA_LITERALCONST( mega )

struct CartInfo
{
  typedef std::map< Direction, Point > Offsets;
  struct
  {
    Offsets front;
    Offsets back;
  } offset;

  CartInfo()
  {
#define FR_OFFSET(dir,x,y) offset.front[ dir ] = Point(x,y);
#define BK_OFFSET(dir,x,y) offset.back[ dir ] = Point(x,y);
    FR_OFFSET( south, -33, 22 )
    FR_OFFSET( west, -31, 35 )
    FR_OFFSET( north, -5, 37 )
    FR_OFFSET( east, -5, 22 )
    FR_OFFSET( southEast, -20, 20 )
    FR_OFFSET( northWest, -20, 40 )
    FR_OFFSET( northEast, -5, 30 )
    FR_OFFSET( southWest, -5, 22 )

    BK_OFFSET( south, -5, 40 )
    BK_OFFSET( west, -5, 22 )
    BK_OFFSET( north, -33, 22 )
    BK_OFFSET( east, -31, 35 )
    BK_OFFSET( southEast, -20, 40 )
    BK_OFFSET( northWest, -20, 20 )
    BK_OFFSET( northEast, -30, 30 )
    BK_OFFSET( southWest,  -0, 30 )
  }

  Point getOffset( bool back, Direction dir )
  {
    return (back ? offset.back : offset.front)[ dir ];
  }
};

}

static const int defaultStepInFrame = 8;
static CartInfo cartInfo;

struct ActionAnimation
{
  int ownerType;
  bool isBack;
  AnimationBank::MovementAnimation actions;
};

class AnimationBank::Impl
{
public:
  typedef enum { stgSimple, stgObjects, stgCarts } LoadingStage;
  typedef std::map< unsigned int, Animation > SimpleAnimations;
  typedef std::map< unsigned int, ActionAnimation > DirectedAnimations;
  typedef std::map< unsigned int, VariantMap > AnimationConfigs;

  AnimationConfigs animConfigs;
  
  DirectedAnimations objects;
  DirectedAnimations carts;
  SimpleAnimations simpleAnimations;

  // fills the cart pictures
  // prefix: image prefix
  // start: index of the first frame
  void fixCartOffset(unsigned int type, bool back, int addh);
  void loadGroup( unsigned int who, const VariantMap& desc, LoadingStage stage);

  void loadStage( DirectedAnimations& refMap, unsigned int who, const std::string& prefix,
                  const int start, const int size,
                  Walker::Action wa=Walker::acMove,
                  const int step = defaultStepInFrame, int delay=0);

  void loadStage( unsigned int type, const std::string& stageName, const VariantMap& stageInfo, LoadingStage stage);

  const AnimationBank::MovementAnimation& tryLoadAnimations( unsigned int wtype );

  void loadCarts( vfs::Path model );
};

void AnimationBank::Impl::loadCarts(vfs::Path model)
{
  //number of animations with goods + emmigrants + immigrants
  //bool frontCart = false;

  VariantMap config = config::load( model );
  foreach( it, config )
  {
    good::Product gtype = good::Helper::getType( it->first );

    if( gtype != good::none )
    {
      VariantMap cartInfo = it->second.toMap();      
      Variant smallInfo = cartInfo.get( literals::small );
      if( smallInfo.isValid() ) loadStage( gtype, it->first + literals::small, smallInfo.toMap(), stgCarts );

      Variant bigInfo = cartInfo.get( literals::big );
      if( bigInfo.isValid() ) loadStage( gtype + animBigCart, it->first + literals::big, bigInfo.toMap(), stgCarts );

      Variant megaInfo = cartInfo.get( literals::mega );
      if( megaInfo.isValid() ) loadStage( gtype + animMegaCart, it->first + literals::mega, megaInfo.toMap(), stgCarts );
    }
  }

  VariantMap imPureCart = config.get( "none" ).toMap();
  loadGroup( good::none, imPureCart, stgCarts );

  VariantMap imScarbInfo = config.get( "immigrantScarb" ).toMap();
  loadGroup( animImmigrantCart, imScarbInfo, stgCarts );

  VariantMap circusInfo = config.get( "circusCart" ).toMap();
  loadGroup( animCircusCart, circusInfo, stgCarts );
}

AnimationBank::AnimationBank() : _d( new Impl )
{
}

void AnimationBank::loadCarts(vfs::Path model)
{
  Logger::warning( "AnimationBank: loading cart graphics" );

  _d->loadCarts( model );
}

void AnimationBank::Impl::loadStage(DirectedAnimations& refMap , unsigned int who, const std::string& prefix,
                                     const int start, const int size,
                                     Walker::Action wa, const int step,
                                     int delay)
{
  MovementAnimation& ioMap = refMap[ who ].actions;
  DirectedAction action( wa, direction::none );

  if( step == 0 )
  {
    action.direction = north;      ioMap[action].load( prefix, start,   size, Animation::straight, 1 );
  }
  else
  {
    action.direction = north;      ioMap[action].load( prefix, start,   size, Animation::straight, step); ioMap[action].setDelay( delay );
    action.direction = northEast;  ioMap[action].load( prefix, start+1, size, Animation::straight, step); ioMap[action].setDelay( delay );
    action.direction = east;       ioMap[action].load( prefix, start+2, size, Animation::straight, step); ioMap[action].setDelay( delay );
    action.direction = southEast;  ioMap[action].load( prefix, start+3, size, Animation::straight, step); ioMap[action].setDelay( delay );
    action.direction = south;      ioMap[action].load( prefix, start+4, size, Animation::straight, step); ioMap[action].setDelay( delay );
    action.direction = southWest;  ioMap[action].load( prefix, start+5, size, Animation::straight, step); ioMap[action].setDelay( delay );
    action.direction = west;       ioMap[action].load( prefix, start+6, size, Animation::straight, step); ioMap[action].setDelay( delay );
    action.direction = northWest;  ioMap[action].load( prefix, start+7, size, Animation::straight, step); ioMap[action].setDelay( delay );
  }
}

void AnimationBank::Impl::loadStage( unsigned int type, const std::string& stageName, const VariantMap& stageInfo, LoadingStage stage )
{
  PictureInfoBank& pib = PictureInfoBank::instance();

  VARIANT_INIT_STR( rc, stageInfo )
  VARIANT_INIT_ANY( int, start, stageInfo )
  VARIANT_INIT_ANY( bool, reverse, stageInfo )
  VARIANT_INIT_ANY( int, frames, stageInfo )
  VARIANT_INIT_ANY( int, action, stageInfo )
  VARIANT_INIT_ANY( int, delay, stageInfo )
  VARIANT_INIT_ANY( int, step, stageInfo )

  switch( stage )
  {
  case stgSimple:
    {
      VARIANT_LOAD_ANY( type, stageInfo )
      VARIANT_INIT_ANY( Variant, offset, stageInfo )

      if( type == 0 )
        type = Hash( utils::localeLower( stageName ) );

      Logger::warning( "AnimationBank: load simple animations for " + stageName );
      Animation& animation = simpleAnimations[ type ];
      animation.load( rc, start, frames, reverse, step );
      animation.setDelay( delay );
      if( offset.isValid() )
      {
        if( offset.type() == Variant::String )
        {
          Point p;
          if( offset.toString() == "center" )
          {
            Size s = animation.frame( 0 ).size();
            p = Point( -s.width()/2, s.height() );
          }
          animation.setOffset( p );
        }
        else
          animation.setOffset( offset.toPoint() );
      }
    }
  break;

  case stgObjects:
    {
      //creating information about animation offset
      Point offset = pib.getDefaultOffset( PictureInfoBank::walkerOffset );
      VARIANT_LOAD_ANYDEF( offset, stageInfo, offset );
      pib.setOffset( rc, start, frames * (step == 0 ? 1 : step), offset );

      std::string typeName = WalkerHelper::getTypename( (walker::Type)type );
      Logger::warning( "AnimationBank: load animations for " + typeName + ":" + stageName );
      loadStage( objects, type, rc, start, frames, (Walker::Action)action, step, delay );
    }
  break;

  case stgCarts:
    {
      Logger::warning( "AnimationBank: load animations for %d:%s", type, stageName.c_str() );
      loadStage( carts, type, rc, start, frames, Walker::acMove, step, delay );

      VARIANT_INIT_ANY( int, back, stageInfo )
      VARIANT_INIT_ANY( int, addh, stageInfo )

      carts[ type ].isBack = back;
      fixCartOffset( type, back, addh );
    }
  break;
  }
}

void AnimationBank::Impl::loadGroup( unsigned int type, const VariantMap& desc, LoadingStage stage )
{  
  foreach( ac, desc )
  {   
    loadStage( type, ac->first, ac->second.toMap(), stage );
  }
}

const AnimationBank::MovementAnimation& AnimationBank::Impl::tryLoadAnimations( unsigned int wtype)
{
  AnimationConfigs::iterator configIt = animConfigs.find( wtype );

  if( configIt != animConfigs.end() )
  {
    loadGroup( wtype, configIt->second, stgObjects );
    animConfigs.erase( configIt );
  }

  DirectedAnimations::iterator it = objects.find( wtype );
  if( it == objects.end() )
  {
    Logger::warning( "!!! WARNING: AnimationBank can't find config for type %d", wtype );
    const AnimationBank::MovementAnimation& elMuleta = objects[ walker::unknown ].actions;
    objects[ wtype ].ownerType = wtype;
    objects[ wtype ].actions = elMuleta;
    return elMuleta;
  }

  return it->second.actions;
}

const AnimationBank::MovementAnimation& AnimationBank::find( unsigned int type )
{
  Impl::DirectedAnimations& dAnim = instance()._d->objects;

  Impl::DirectedAnimations::iterator it = dAnim.find( type );
  if( it == dAnim.end() )
  {
    const AnimationBank::MovementAnimation& ret = instance()._d->tryLoadAnimations( type );
    return ret;
  }

  return it->second.actions;
}

AnimationBank::~AnimationBank()
{

}

void AnimationBank::loadAnimation(vfs::Path model, vfs::Path basic)
{
  Logger::warning( "AnimationBank: start loading animations from " + model.toString() );
  _d->loadStage( _d->objects, 0, ResourceGroup::citizen1, 1, 12, Walker::acMove );

  VariantMap items = config::load( model );

  foreach( i, items )
  {
    walker::Type wtype = WalkerHelper::getType( i->first );
    if( wtype != walker::unknown )
    {
      Logger::warning( "Load config animations for " + i->first );
      _d->animConfigs[ wtype ] = i->second.toMap();
    }
    else
    {
      Logger::warning( "AnimationBank: cannot find type " + i->first );
    }
  }

  items = config::load( basic );
  _d->loadGroup( animUnknown, items, Impl::stgSimple );
}

void AnimationBank::Impl::fixCartOffset( unsigned int who, bool back, int addh )
{

#define __CDA(a) DirectedAction(Walker::acMove,a)
  Point addPointH( 0, addh );
  MovementAnimation& ma = carts[who].actions;
  ma[ __CDA(south) ].setOffset( addPointH + cartInfo.getOffset( back, south ) );
  ma[ __CDA(west)  ].setOffset ( addPointH + cartInfo.getOffset( back, west ) );
  ma[ __CDA(north) ].setOffset( addPointH + cartInfo.getOffset( back, north ) );
  ma[ __CDA(east)  ].setOffset ( addPointH + cartInfo.getOffset( back, east ) );

  ma[ __CDA(southEast) ].setOffset( addPointH + cartInfo.getOffset( back, southEast ) );
  ma[ __CDA(northWest) ].setOffset( addPointH + cartInfo.getOffset( back, northWest ) );
  ma[ __CDA(northEast) ].setOffset( addPointH + cartInfo.getOffset( back, northEast ) );
  ma[ __CDA(southWest) ].setOffset( addPointH + cartInfo.getOffset( back, southWest ) );
#undef __CDA
}

const Animation& AnimationBank::getCart(int good, int capacity, Direction direction, bool& isBack)
{
  int index = 0;
  if( good != good::none )
  {
    if( capacity > animBigCart ) index = animMegaCart;
    else if( capacity > animSimpleCart ) index = animBigCart;
  }

  ActionAnimation& dAction = instance()._d->carts[ index + good ];
  MovementAnimation& ma = dAction.actions;
  isBack = dAction.isBack;
  return ma[ DirectedAction( Walker::acMove, direction ) ];
}

const Animation& AnimationBank::simple(unsigned int type)
{
  Impl::SimpleAnimations& sAnim = instance()._d->simpleAnimations;

  Impl::SimpleAnimations::iterator it = sAnim.find( type );
  if( it != sAnim.end() )
  {
    return it->second;
  }

  return sAnim[ animUnknown ];
}

const Animation& AnimationBank::simple(const std::string& name)
{
  return simple( Hash( name ) );
}

}//end namespace gfx
