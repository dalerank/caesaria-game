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

#include "walker.hpp"

#include "gfx/tile.hpp"
#include "core/enumerator.hpp"
#include "building/metadata.hpp"
#include "core/exception.hpp"
#include "core/position.hpp"
#include "core/variant.hpp"
#include "core/stringhelper.hpp"
#include "game/path_finding.hpp"
#include "game/city.hpp"
#include "gfx/animation_bank.hpp"
#include "core/gettext.hpp"
#include "game/tilemap.hpp"
#include "core/logger.hpp"
#include "ability.hpp"

using namespace constants;

class Walker::Impl
{
public:
  CityPtr city;
  walker::Type walkerType;
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
  int health;
  AbilityList abilities;

  float getSpeed() const
  {
    return speedMultiplier * speed;
  }

  void updateSpeedMultiplier( const Tile& tile ) 
  {
    speedMultiplier = (tile.getFlag( Tile::tlRoad ) || tile.getFlag( Tile::tlGarden )) ? 1.f : 0.5f;
  }
};

Walker::Walker( CityPtr city ) : _d( new Impl )
{
  _d->city = city;
  _d->action.action = Walker::acMove;
  _d->action.direction = D_NONE;
  _d->walkerType = walker::unknown;
  _d->walkerGraphic = WG_NONE;
  _d->health = 100;

  _d->speed = 1.f;  // default speed
  _d->speedMultiplier = 1.f;
  _d->isDeleted = false;

  _d->midTilePos = Point( 7, 7 );
  _d->remainMove = PointF( 0, 0 );
}

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

    if( _d->speed > 0.f )
    {
      _updateAnimation( time );
    }
  break;

  case Walker::acFight:
    _updateAnimation( time );
  break;

  default:
  break;
  }

  foreach( AbilityPtr ab, _d->abilities)
  {
    ab->run( this, time );
  }

  if( getHealth() <= 0 )
  {
    die();
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

   _d->posOnMap = Point( _d->pos.getI(), _d->pos.getJ() ) * 15 + _d->tileOffset;
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

Point Walker::getSubPosition() const
{
  return _d->tileOffset;
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

   Tile& tile = _d->city->getTilemap().at( getIJ() );
    
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
      Logger::warning( "Invalid move direction: %d", _d->action.direction );
      _d->action.direction = D_NONE;
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
         Logger::warning( "Invalid move direction: %d", _d->action.direction);
         _d->action.direction = D_NONE;
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

   Point overlayOffset = tile.getOverlay().isValid()
                              ? tile.getOverlay()->getOffset( _d->tileOffset )
                              : Point( 0, 0 );

   _d->posOnMap = Point( _d->pos.getI(), _d->pos.getJ() )*15 + _d->tileOffset + overlayOffset;
}


void Walker::onNewTile()
{
   Tilemap& tilemap = _d->city->getTilemap();
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

   if( lastDirection != _d->action.direction )
   {
      onNewDirection();
   }
}


DirectionType Walker::getDirection()
{
  return _d->action.direction;
}

double Walker::getHealth() const
{
  return _d->health;
}

void Walker::updateHealth(double value)
{
  _d->health = math::clamp( _d->health + value, -100.0, 100.0 );
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
    const AnimationBank::MovementAnimation& animMap = AnimationBank::getWalker( getWalkerGraphic() );
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

    if( itAnimMap != animMap.end() )
    {
      _d->animation = itAnimMap->second;
    }
    else
    {
      _d->animation = animMap.begin()->second;
      Logger::warning( "Wrong walker direction detected" );
    }
  }

  return _d->animation.getFrame();
}

void Walker::save( VariantMap& stream ) const
{
  stream[ "name" ] = Variant( _d->name );
  stream[ "type" ] = (int)_d->walkerType;
  stream[ "pathway" ] =  _d->pathWay.save();
  stream[ "health" ] = _d->health;
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
  Tilemap& tmap = _getCity()->getTilemap();

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
  _d->health = (double)stream.get( "health" );
}

void Walker::addAbility(AbilityPtr ability)
{
  _d->abilities.push_back( ability );
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

WalkerGraphicType Walker::_getGraphic() const
{
  return _d->walkerGraphic;
}

void Walker::_setType(walker::Type type)
{
  _d->walkerType = type;
}

CityPtr Walker::_getCity() const
{
  return _d->city;
}

void Walker::_setHealth(double value)
{
  _d->health = value;
}

void Walker::_updateAnimation( const unsigned int time )
{
  if( _d->animation.size() > 0 )
  {
    _d->animation.update( time );
  }
}

void Walker::go()
{
  _d->action.action = acMove;       // default action
}

void Walker::die()
{
  _d->health = 0;
  deleteLater();
}

Soldier::Soldier( CityPtr city ) : Walker( city )
{
  _setType( walker::soldier );
  _setGraphic( WG_HORSEMAN );
}

class WalkerHelper::Impl : public EnumsHelper<walker::Type>
{
public:
  typedef std::map< walker::Type, std::string > PrettyNames;
  PrettyNames prettyTypenames;

  void append( walker::Type type, const std::string& typeName, const std::string& prettyTypename )
  {
    EnumsHelper<walker::Type>::append( type, typeName );
    prettyTypenames[ type ] = prettyTypename;
  }

  Impl() : EnumsHelper<walker::Type>( walker::unknown )
  {
    append( walker::unknown, "none", _("##wt_none##"));
    append( walker::immigrant, "immigrant", _("##wt_immigrant##") );
    append( walker::emigrant, "emmigrant", _("##wt_emmigrant##") );
    append( walker::soldier, "soldier", _("##wt_soldier##") );
    append( walker::cartPusher, "cart_pusher", _("##wt_cart_pushher##") );
    append( walker::WT_MARKETLADY, "market_lady", _("##wt_market_lady##") );
    append( walker::marketLady, "market_lady_helper", _("##wt_market_lady_helper##") );
    append( walker::WT_SERVICE, "serviceman", _("##wt_serviceman##") );
    append( walker::trainee, "trainee", _("##wt_trainee##") );
    append( walker::recruter, "workers_hunter", _("##wt_workers_hunter##") );
    append( walker::prefect, "prefect", _("##wt_prefect##") );
    append( walker::taxCollector, "tax_collector", _("##wt_tax_collector##") );
    append( walker::WT_MERCHANT, "merchant", _("##wt_merchant##") );
    append( walker::engineer, "engineer", _("##wt_engineer##") );
    append( walker::doctor, "doctor", _("##wt_doctor##") );
    append( walker::sheep, "sheep", _("##wt_animal_sheep##") );
    append( walker::bathlady, "bathlady", _("##wt_bathlady##") );
    append( walker::actor, "actor", _("##wt_actor##") );
    append( walker::gladiator, "gladiator", _("##wt_gladiator##") );
    append( walker::barber, "barber", _("##wt_barber##" ) );
    append( walker::surgeon, "surgeon", _("##wt_surgeon##") );
    append( walker::WT_ALL, "unknown", _("##wt_unknown##") );
  }
};

WalkerHelper& WalkerHelper::instance()
{
  static WalkerHelper inst;
  return inst;
}

std::string WalkerHelper::getName( walker::Type type )
{
  std::string name = instance()._d->findName( type );

  if( name.empty() )
  {
    Logger::warning( "Can't find walker typeName for %d", type );
    //_OC3_DEBUG_BREAK_IF( "Can't find walker typeName by WalkerType" );
  }

  return name;
}

walker::Type WalkerHelper::getType(const std::string &name)
{
  walker::Type type = instance()._d->findType( name );

  if( type == instance()._d->getInvalid() )
  {
    Logger::warning( "Can't find walker type for %s", name.c_str() );
    //_OC3_DEBUG_BREAK_IF( "Can't find walker type by typeName" );
  }

  return type;
}

std::string WalkerHelper::getPrettyTypeName(walker::Type type)
{
  Impl::PrettyNames::iterator it = instance()._d->prettyTypenames.find( type );
  return it != instance()._d->prettyTypenames.end() ? it->second : "";
}

Picture WalkerHelper::getBigPicture(walker::Type type)
{
  int index = -1;
  switch( type )
  {
  case walker::immigrant: index=9; break;
  case walker::emigrant: index=13; break;
  case walker::doctor: index = 2; break;
  case walker::cartPusher: index=51; break;
  case walker::WT_MARKETLADY: index=12; break;
  case walker::marketLady: index=38; break;
  case walker::WT_MERCHANT: index=25; break;
  case walker::prefect: index=19; break;
  case walker::engineer: index=7; break;
  case walker::taxCollector: index=6; break;
  case walker::sheep: index = 54; break;
  case walker::recruter: index=13; break;

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
