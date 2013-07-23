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
#include "oc3_building_data.hpp"
#include "oc3_exception.hpp"
#include "oc3_scenario.hpp"
#include "oc3_positioni.hpp"
#include "oc3_walkermanager.hpp"
#include "oc3_variant.hpp"
#include "oc3_stringhelper.hpp"
#include "oc3_path_finding.hpp"
#include "oc3_picture_bank.hpp"
#include "oc3_city.hpp"

class Walker::Impl
{
public:
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
  WalkerAction action;

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
  _d->action._action = WA_MOVE;
  _d->action._direction = D_NONE;
  _walkerType = WT_NONE;
  _walkerGraphic = WG_NONE;

  _d->speed = 1.f;  // default speed
  _d->speedMultiplier = 1.f;
  _isDeleted = false;

  _d->midTilePos = Point( 7, 7 );
  _d->remainMove = PointF( 0, 0 );
};

Walker::~Walker()
{
}

int Walker::getType() const
{
   return _walkerType;
}

void Walker::timeStep(const unsigned long time)
{
   switch (_d->action._action)
   {
   case WA_MOVE:
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
   return _isDeleted;
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
   return _walkerGraphic;
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
   if (D_NONE == _d->action._direction )
   {
      // nothing to do
      return;
   }

   Tile& tile = Scenario::instance().getCity()->getTilemap().at( getIJ() );
    
   switch (_d->action._direction)
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
      _OC3_DEBUG_BREAK_IF( "Invalid move direction: " ||  _d->action._direction);
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
      switch (_d->action._direction)
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
         _OC3_DEBUG_BREAK_IF("Invalid move direction: " || _d->action._direction);
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
   // std::cout << "Walker is on a new tile! coord=" << _i << "," << _j << std::endl;
   Tilemap& tilemap = Scenario::instance().getCity()->getTilemap();
   Tile& currentTile = tilemap.at( _d->pos );
   _d->updateSpeedMultiplier( currentTile );
   if( !currentTile.getTerrain().isRoad() )
   {
     StringHelper::debug( 0xff, "Walker at (%d, %d) is not on a road!!!", _d->pos.getI(), _d->pos.getJ() );
   }
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
   _d->action._action=WA_NONE;  // stop moving
   _d->animation = Animation();
}

void Walker::onNewDirection()
{
   _d->animation = Animation();  // need to fetch the new animation
}


void Walker::computeDirection()
{
   DirectionType lastDirection = _d->action._direction;
   _d->action._direction = _d->pathWay.getNextDirection();

   if (lastDirection != _d->action._direction)
   {
      onNewDirection();
   }
}


DirectionType Walker::getDirection()
{
   return _d->action._direction;
}

void Walker::getPictureList(std::vector<Picture*> &oPics)
{
   oPics.clear();
   oPics.push_back(&getMainPicture());
}

Picture& Walker::getMainPicture()
{
   if( !_d->animation.isValid() )
   {
      const std::map<WalkerAction, Animation>& animMap = WalkerLoader::instance().getAnimationMap(getWalkerGraphic());
      std::map<WalkerAction, Animation>::const_iterator itAnimMap;
      if (_d->action._action == WA_NONE || _d->action._direction == D_NONE)
      {
         WalkerAction action;
         action._action = WA_MOVE;       // default action
         if (_d->action._direction == D_NONE)
         {
            action._direction = D_NORTH;  // default direction
         }
         else
         {
            action._direction = _d->action._direction;  // last direction of the walker
         }
         itAnimMap = animMap.find(action);
      }
      else
      {
         itAnimMap = animMap.find(_d->action);
      }

      _d->animation = itAnimMap->second;
   }

   return *_d->animation.getCurrentPicture();
}

void Walker::save( VariantMap& stream ) const
{
  //stream[ "id" ] = this;
  stream[ "type" ] = (int)_walkerType;
  stream[ "pathway" ] =  _d->pathWay.save();
  stream[ "action" ] = (int)_d->action._action;
  stream[ "direction" ] = (int)_d->action._direction;
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
  _d->action._action = (WalkerActionType) stream.get( "action" ).toInt();
  _d->action._direction = (DirectionType) stream.get( "direction" ).toInt();
  _d->pos = stream.get( "pos" ).toTilePos();
  _d->tileOffset = stream.get( "tileoffset" ).toPoint();
  _d->posOnMap = stream.get( "mappos" ).toPoint();
  _d->uid = (UniqueId)stream.get( "uid" ).toInt();
  _d->speedMultiplier = stream.get( "speedMul" ).toFloat();
  
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
   _isDeleted = true;
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

void Walker::_setAction( WalkerActionType action )
{
  _d->action._action = action;
}

void Walker::_setDirection( DirectionType direction )
{
  _d->action._direction = direction;
}
Soldier::Soldier()
{
   _walkerType = WT_SOLDIER;
   _walkerGraphic = WG_HORSEMAN;
}

