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

#include "oc3_walker.hpp"

#include "oc3_tile.hpp"
#include "oc3_enums_helper.hpp"
#include "oc3_building_data.hpp"
#include "oc3_exception.hpp"
#include "oc3_scenario.hpp"
#include "oc3_positioni.hpp"
#include "oc3_walkermanager.hpp"
#include "oc3_variant.hpp"
#include "oc3_stringhelper.hpp"
#include "oc3_path_finding.hpp"
#include "oc3_city.hpp"
#include "oc3_animation_bank.hpp"
#include "oc3_gettext.hpp"

class Walker::Impl
{
public:
  WalkerType walkerType;
  WalkerGraphicType walkerGraphic;
  bool isDeleted;
  float speed;
  TilePos pos;
  UniqueId uid;
  Point midTilePos;  // subtile coordinate in the current tile, at starting position
  float speedMultiplier;
  Point tileOffset; // subtile coordinate in the current tile: 0..15
  Animation animation;  // current animation
  Point posOnMap; // subtile coordinate across all tiles: 0..15*mapsize (ii=15*i+si)
  PointF remainMove;  // remaining movement
  PathWay pathWay;
  DirectedAction action;
  std::string name;

  float getSpeed() const
  {
    return speedMultiplier * speed;
  }

  void updateSpeedMultiplier( const Tile& tile ) 
  {
    speedMultiplier = (tile.getTerrain().isRoad() || tile.getTerrain().isGarden()) ? 1.f : 0.5f;
  }
};

Walker::Walker() : _d( new Impl )
{
  _d->action.action = Walker::acMove;
  _d->action.direction = D_NONE;
  _d->walkerType = WT_NONE;
  _d->walkerGraphic = WG_NONE;

  _d->speed = 1.f;  // default speed
  _d->speedMultiplier = 1.f;
  _d->isDeleted = false;

  _d->midTilePos = Point( 7, 7 );
  _d->remainMove = PointF( 0, 0 );
};

Walker::~Walker()
{
}

int Walker::getType() const
{
   return _d->walkerType;
}

void Walker::timeStep(const unsigned long time)
{
   switch (_d->action.action)
   {
   case Walker::acMove:
      walk();
     
      if( _d->animation.getPicturesCount() > 0 && _d->getSpeed() > 0.f )
      {
        _d->animation.update( time );
      }
      break;

   default:
      break;
   }
}


bool Walker::isDeleted() const
{
   return _d->isDeleted;
}

void Walker::setIJ( const TilePos& pos )
{
   _d->pos = pos;

   _d->tileOffset = _d->midTilePos;

   _d->posOnMap = Point( _d->pos.getI(), _d->pos.getJ() ) * 15 + _d->tileOffset ;
}

int Walker::getI() const
{
   return _d->pos.getI();
}

int Walker::getJ() const
{
   return _d->pos.getJ();
}

Point Walker::getPosition() const
{
  return Point( 2*(_d->posOnMap.getX() + _d->posOnMap.getY()),
                   _d->posOnMap.getX() - _d->posOnMap.getY() );
} 

void Walker::setPathWay( const PathWay &pathWay)
{
   _d->pathWay = pathWay;
   _d->pathWay.begin();

   onMidTile();
}

void Walker::setSpeed(const float speed)
{
   _d->speed = speed;
}

WalkerGraphicType Walker::getWalkerGraphic() const
{
   return _d->walkerGraphic;
}


// ioSI: subtile index, ioI: tile index, ioAmount: distance, iMidPos: subtile offset 0, oNewTile: true if tile change, oMidTile: true if on tile center
void Walker::inc(int &ioSI, int &ioI, int &ioAmount, const int iMidPos, bool &oNewTile, bool &oMidTile)
{
   int delta = ioAmount;
   if ((ioSI<iMidPos) && (ioSI+delta>=iMidPos))  // midpos is ahead and inside the current movement
   {
      // we will stop at the mid tile!
      delta = iMidPos - ioSI;
      oMidTile = true;
   }

   if (ioSI+delta>15)  // the start of next tile is inside the current movement
   {
      // we will stop at the beginning of the new tile!
      delta = 16 - ioSI;
      oNewTile = true;
      ioI += 1;  // next tile
      ioSI = ioSI - 15;
   }

   ioAmount -= delta;
   ioSI += delta;
}

// ioSI: subtile index, ioI: tile index, ioAmount: distance, iMidPos: subtile offset 0, oNewTile: true if tile change, oMidTile: true if on tile center
void Walker::dec(int &ioSI, int &ioI, int &ioAmount, const int iMidPos, bool &oNewTile, bool &oMidTile)
{
   int delta = ioAmount;
   if ((ioSI>iMidPos) && (ioSI-delta<=iMidPos))  // midpos is ahead and inside the current movement
   {
      // we will stop at the mid tile!
      delta = ioSI - iMidPos;
      oMidTile = true;
   }

   if (ioSI-delta<0)  // the start of next tile is inside the current movement
   {
      // we will stop at the beginning of the new tile!
      delta = ioSI+1;
      oNewTile = true;
      ioI -= 1;  // next tile
      ioSI = ioSI + 15;
   }

   ioAmount -= delta;
   ioSI -= delta;
}

void Walker::walk()
{
   if (D_NONE == _d->action.direction )
   {
      // nothing to do
      return;
   }

   Tile& tile = Scenario::instance().getCity()->getTilemap().at( getIJ() );
    
   switch (_d->action.direction)
   {
   case D_NORTH:
   case D_SOUTH:
      _d->remainMove += PointF( 0, _d->getSpeed() );
      break;
   case D_EAST:
   case D_WEST:
      _d->remainMove += PointF( _d->getSpeed(), 0 );
      break;
   case D_NORTH_EAST:
   case D_SOUTH_WEST:
   case D_SOUTH_EAST:
   case D_NORTH_WEST:
      _d->remainMove += PointF( _d->getSpeed() * 0.7f, _d->getSpeed() * 0.7f );
      break;
   default:
      _OC3_DEBUG_BREAK_IF( "Invalid move direction: " || _d->action.direction);
      break;
   }
   

   bool newTile = false;
   bool midTile = false;
   int amountI = int(_d->remainMove.getX());
   int amountJ = int(_d->remainMove.getY());
   _d->remainMove -= Point( amountI, amountJ ).toPointF();

   // std::cout << "walker step, amount :" << amount << std::endl;
   int tmpX = _d->tileOffset.getX();
   int tmpY = _d->tileOffset.getY();
   int tmpJ = _d->pos.getJ();
   int tmpI = _d->pos.getI();
   while (amountI+amountJ > 0)
   {
      switch (_d->action.direction)
      {
      case D_NORTH:
         inc(tmpY, tmpJ, amountJ, _d->midTilePos.getY(), newTile, midTile);
         break;
      case D_NORTH_EAST:
         inc(tmpY, tmpJ, amountJ, _d->midTilePos.getY(), newTile, midTile);
         inc(tmpX, tmpI, amountI, _d->midTilePos.getX(), newTile, midTile);
         break;
      case D_EAST:
         inc(tmpX, tmpI, amountI, _d->midTilePos.getX(), newTile, midTile);
         break;
      case D_SOUTH_EAST:
         dec(tmpY, tmpJ, amountJ, _d->midTilePos.getY(), newTile, midTile);
         inc(tmpX, tmpI, amountI, _d->midTilePos.getX(), newTile, midTile);
         break;
      case D_SOUTH:
         dec(tmpY, tmpJ, amountJ, _d->midTilePos.getY(), newTile, midTile);
         break;
      case D_SOUTH_WEST:
         dec(tmpY, tmpJ, amountJ, _d->midTilePos.getY(), newTile, midTile);
         dec(tmpX, tmpI, amountI, _d->midTilePos.getX(), newTile, midTile);
         break;
      case D_WEST:
         dec(tmpX, tmpI, amountI, _d->midTilePos.getX(), newTile, midTile);
         break;
      case D_NORTH_WEST:
         inc(tmpY, tmpJ, amountJ, _d->midTilePos.getY(), newTile, midTile);
         dec(tmpX, tmpI, amountI, _d->midTilePos.getX(), newTile, midTile);
         break;
      default:
         _OC3_DEBUG_BREAK_IF("Invalid move direction: " || _d->action.direction);
         break;
      }

      _d->tileOffset = Point( tmpX, tmpY );
      _d->pos = TilePos( tmpI, tmpJ );

      if (newTile)
      {
         // walker is now on a new tile!
         onNewTile();
      }

      if (midTile)
      {
         // walker is now on the middle of the tile!
         onMidTile();
      }

      // if (midTile) std::cout << "walker mid tile" << std::endl;
      // if (newTile) std::cout << "walker new tile" << std::endl;
      // if (amount != 0) std::cout << "walker remaining step :" << amount << std::endl;
   }

   Point overlayOffset = tile.getTerrain().getOverlay().isValid() 
                              ? tile.getTerrain().getOverlay()->getOffset( _d->tileOffset ) 
                              : Point( 0, 0 );

   _d->posOnMap = Point( _d->pos.getI(), _d->pos.getJ() )*15 + _d->tileOffset + overlayOffset;
}


void Walker::onNewTile()
{
   Tilemap& tilemap = Scenario::instance().getCity()->getTilemap();
   Tile& currentTile = tilemap.at( _d->pos );
   _d->updateSpeedMultiplier( currentTile );
}


void Walker::onMidTile()
{
   // std::cout << "Walker is on mid tile! coord=" << _i << "," << _j << std::endl;
   if (_d->pathWay.isDestination())
   {
      onDestination();
   }
   else
   {
      // compute the direction to reach the destination
      computeDirection();
   }
}


void Walker::onDestination()
{
   // std::cout << "Walker arrived at destination! coord=" << _i << "," << _j << std::endl;
   _d->action.action = acNone;  // stop moving
   _d->animation = Animation();
}

void Walker::onNewDirection()
{
   _d->animation = Animation();  // need to fetch the new animation
}


void Walker::computeDirection()
{
   DirectionType lastDirection = _d->action.direction;
   _d->action.direction = _d->pathWay.getNextDirection();

   if (lastDirection != _d->action.direction)
   {
      onNewDirection();
   }
}


DirectionType Walker::getDirection()
{
  return _d->action.direction;
}

void Walker::setName(const std::string &name)
{
  _d->name = name;
}

const std::string &Walker::getName() const
{
  return _d->name;
}

std::string Walker::getThinks() const
{
  return "";
}

void Walker::getPictureList(std::vector<Picture> &oPics)
{
   oPics.clear();
   oPics.push_back( getMainPicture() );
}

const Picture& Walker::getMainPicture()
{
   if( !_d->animation.isValid() )
   {
     const AnimationBank::WalkerAnimationMap& animMap = AnimationBank::getWalker( getWalkerGraphic() );
     std::map<DirectedAction, Animation>::const_iterator itAnimMap;
     if (_d->action.action == acNone || _d->action.direction == D_NONE)
     {
        DirectedAction action;
        action.action = acMove;       // default action
        if (_d->action.direction == D_NONE)
        {
           action.direction = D_NORTH;  // default direction
        }
        else
        {
           action.direction = _d->action.direction;  // last direction of the walker
        }
        itAnimMap = animMap.find(action);
     }
     else
     {
        itAnimMap = animMap.find(_d->action);
     }

     _d->animation = itAnimMap->second;
   }

   return _d->animation.getCurrentPicture();
}

void Walker::save( VariantMap& stream ) const
{
  stream[ "name" ] = Variant( _d->name );
  stream[ "type" ] = (int)_d->walkerType;
  stream[ "pathway" ] =  _d->pathWay.save();
  stream[ "action" ] = (int)_d->action.action;
  stream[ "direction" ] = (int)_d->action.direction;
  stream[ "pos" ] = _d->pos;
  stream[ "tileoffset" ] = _d->tileOffset;
  stream[ "mappos" ] = _d->posOnMap;
  stream[ "speed" ] = _d->speed;
  stream[ "midTile" ] = _d->midTilePos;
  stream[ "speedMul" ] = (float)_d->speedMultiplier;
  stream[ "uid" ] = (unsigned int)_d->uid;
  stream[ "remainmove" ] = _d->remainMove;
}

void Walker::load( const VariantMap& stream)
{
  Tilemap& tmap = Scenario::instance().getCity()->getTilemap();

  _d->pathWay.init( tmap, tmap.at( 0, 0 ) );
  _d->pathWay.load( stream.get( "pathway" ).toMap() );
  _d->action.action = (Walker::Action) stream.get( "action" ).toInt();
  _d->action.direction = (DirectionType) stream.get( "direction" ).toInt();
  _d->pos = stream.get( "pos" ).toTilePos();
  _d->tileOffset = stream.get( "tileoffset" ).toPoint();
  _d->posOnMap = stream.get( "mappos" ).toPoint();
  _d->uid = (UniqueId)stream.get( "uid" ).toInt();
  _d->speedMultiplier = stream.get( "speedMul" ).toFloat();
  _d->name = stream.get( "name" ).toString();
  
  _OC3_DEBUG_BREAK_IF( _d->speedMultiplier < 0.1 );
  if( _d->speedMultiplier < 0.1 ) //Sometime this have this error in save file
  {
    _d->speedMultiplier = 1;
  }

  _d->speed = stream.get( "speed" ).toFloat();
  _d->midTilePos = stream.get( "midTile" ).toPoint();
  _d->remainMove = stream.get( "remainmove" ).toPointF();
}

TilePos Walker::getIJ() const
{
    return _d->pos;
}

void Walker::deleteLater()
{
   _d->isDeleted = true;
}

void Walker::setUniqueId( const UniqueId uid )
{
  _d->uid = uid;
}

PathWay& Walker::_getPathway()
{
  return _d->pathWay;
}

const PathWay& Walker::getPathway() const
{
  return _d->pathWay;
}

Animation& Walker::_getAnimation()
{
  return _d->animation;
}

void Walker::_setAction( Walker::Action action )
{
  _d->action.action = action;
}

void Walker::_setDirection( DirectionType direction )
{
  _d->action.direction = direction;
}

void Walker::_setGraphic(WalkerGraphicType type)
{
  _d->walkerGraphic = type;
}

void Walker::_setType(WalkerType type)
{
  _d->walkerType = type;
}

void Walker::go()
{
  _d->action.action = acMove;       // default action
}

void Walker::die()
{

}

Soldier::Soldier()
{
  _setType( WT_SOLDIER );
  _setGraphic( WG_HORSEMAN );
}

class WalkerHelper::Impl : public EnumsHelper<WalkerType>
{
public:
  typedef std::map< WalkerType, std::string > PrettyNames;
  PrettyNames prettyTypenames;

  void append( WalkerType type, const std::string& typeName, const std::string& prettyTypename )
  {
    EnumsHelper<WalkerType>::append( type, typeName );
    prettyTypenames[ type ] = prettyTypename;
  }

  Impl() : EnumsHelper<WalkerType>( WT_NONE )
  {
    append( WT_NONE, "none", _("##wt_none##"));
    append( WT_IMMIGRANT, "immigrant", _("##wt_immigrant##") );
    append( WT_EMIGRANT, "emmigrant", _("##wt_emmigrant##") );
    append( WT_SOLDIER, "soldier", _("##wt_soldier##") );
    append( WT_CART_PUSHER, "cart_pusher", _("##wt_cart_pushher##") );
    append( WT_MARKETLADY, "market_lady", _("##wt_market_lady##") );
    append( WT_MARKETLADY_HELPER, "market_lady_helper", _("##wt_market_lady_helper##") );
    append( WT_SERVICE, "serviceman", _("##wt_serviceman##") );
    append( WT_TRAINEE, "trainee", _("##wt_trainee##") );
    append( WT_WORKERS_HUNTER, "workers_hunter", _("##wt_workers_hunter##") );
    append( WT_PREFECT, "prefect", _("##wt_prefect##") );
    append( WT_TAXCOLLECTOR, "tax_collector", _("##wt_tax_collector##") );
    append( WT_MERCHANT, "merchant", _("##wt_merchant##") );
    append( WT_ENGINEER, "engineer", _("##wt_engineer##") );
    append( WT_DOCTOR, "doctor", _("##wt_doctor##") );
    append( WT_ANIMAL_SHEEP, "sheep", _("##wt_animal_sheep##") );
    append( WT_MAX, "unknown", _("##wt_unknown##") );
  }
};

WalkerHelper& WalkerHelper::instance()
{
  static WalkerHelper inst;
  return inst;
}

std::string WalkerHelper::getName( WalkerType type )
{
  std::string name = instance()._d->findName( type );

  if( name.empty() )
  {
    StringHelper::debug( 0xff, "Can't find walker typeName for %d", type );
    _OC3_DEBUG_BREAK_IF( "Can't find walker typeName by WalkerType" );
  }

  return name;
}

WalkerType WalkerHelper::getType(const std::string &name)
{
  WalkerType type = instance()._d->findType( name );

  if( type == instance()._d->getInvalid() )
  {
    StringHelper::debug( 0xff, "Can't find walker type for %s", name.c_str() );
    _OC3_DEBUG_BREAK_IF( "Can't find walker type by typeName" );
  }

  return type;
}

std::string WalkerHelper::getPrettyTypeName(WalkerType type)
{
  Impl::PrettyNames::iterator it = instance()._d->prettyTypenames.find( type );
  return it != instance()._d->prettyTypenames.end() ? it->second : "";
}

Picture WalkerHelper::getBigPicture(WalkerType type)
{
  int index = -1;
  switch( type )
  {
  case WT_IMMIGRANT: index=9; break;
  case WT_EMIGRANT: index=13; break;
  case WT_DOCTOR: index = 2; break;
  case WT_CART_PUSHER: index=51; break;
  case WT_MARKETLADY: index=12; break;
  case WT_MARKETLADY_HELPER: index=38; break;
  case WT_MERCHANT: index=25; break;
  case WT_PREFECT: index=19; break;
  case WT_ENGINEER: index=7; break;
  case WT_TAXCOLLECTOR: index=6; break;
  case WT_ANIMAL_SHEEP: index = 54; break;
  case WT_WORKERS_HUNTER: index=13; break;

  default: index=8; break;
  break;
  }

  return index >= 0 ? Picture::load( "bigpeople", index ) : Picture::getInvalid();
}

WalkerHelper::~WalkerHelper()
{
}

WalkerHelper::WalkerHelper() : _d( new Impl )
{

}
