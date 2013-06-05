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
#include "oc3_walker_market_buyer.hpp"
#include "oc3_walker_cart_pusher.hpp"
#include "oc3_positioni.hpp"
#include "oc3_walkermanager.hpp"
#include "oc3_variant.hpp"
#include "oc3_stringhelper.hpp"

class Walker::Impl
{
public:
  float speed;
  TilePos pos;
  TilePos midTile;  // subtile coordinate in the current tile, at starting position
  float speedMultiplier;

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
  _action._action = WA_MOVE;
  _action._direction = D_NONE;
  _walkerType = WT_NONE;
  _walkerGraphic = WG_NONE;

  _d->speed = 1.f;  // default speed
  _d->speedMultiplier = 1.f;
  _isDeleted = false;

  _d->midTile = TilePos( 7, 7 );
  _remainMoveI = 0;
  _remainMoveJ = 0;
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
   switch (_action._action)
   {
   case WA_MOVE:
      walk();
     
      if( _animation.getPicturesCount() > 0 && _d->getSpeed() > 0.f )
      {
        _animation.update( time );
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

   _si = _d->midTile.getI();
   _sj = _d->midTile.getJ();

   _ii = 15*_d->pos.getI() + _si;
   _jj = 15*_d->pos.getJ() + _sj;
}

int Walker::getI() const
{
   return _d->pos.getI();
}

int Walker::getJ() const
{
   return _d->pos.getJ();
}

int Walker::getII() const
{
   return _ii;
}

int Walker::getJJ() const
{
   return _jj;
}

void Walker::setPathWay(PathWay &pathWay)
{
   _pathWay = pathWay;
   _pathWay.begin();

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
   if (D_NONE == _action._direction )
   {
      // nothing to do
      return;
   }

   Tile& tile = Scenario::instance().getCity().getTilemap().at( getIJ() );
    
   switch (_action._direction)
   {
   case D_NORTH:
   case D_SOUTH:
      _remainMoveJ += _d->getSpeed();
      break;
   case D_EAST:
   case D_WEST:
      _remainMoveI += _d->getSpeed();
      break;
   case D_NORTH_EAST:
   case D_SOUTH_WEST:
   case D_SOUTH_EAST:
   case D_NORTH_WEST:
      _remainMoveI += _d->getSpeed() * 0.7f;
      _remainMoveJ += _d->getSpeed() * 0.7f ;
      break;
   default:
      THROW("Invalid move direction: " << _action._direction);
      break;
   }
   

   bool newTile = false;
   bool midTile = false;
   int amountI = int(_remainMoveI);
   int amountJ = int(_remainMoveJ);
   _remainMoveI -= amountI;
   _remainMoveJ -= amountJ;

   // std::cout << "walker step, amount :" << amount << std::endl;
   while (amountI+amountJ > 0)
   {
      switch (_action._direction)
      {
      case D_NORTH:
         inc(_sj, _d->pos.rj(), amountJ, _d->midTile.getJ(), newTile, midTile);
         break;
      case D_NORTH_EAST:
         inc(_sj, _d->pos.rj(), amountJ, _d->midTile.getJ(), newTile, midTile);
         inc(_si, _d->pos.ri(), amountI, _d->midTile.getI(), newTile, midTile);
         break;
      case D_EAST:
         inc(_si, _d->pos.ri(), amountI, _d->midTile.getI(), newTile, midTile);
         break;
      case D_SOUTH_EAST:
         dec(_sj, _d->pos.rj(), amountJ, _d->midTile.getJ(), newTile, midTile);
         inc(_si, _d->pos.ri(), amountI, _d->midTile.getI(), newTile, midTile);
         break;
      case D_SOUTH:
         dec(_sj, _d->pos.rj(), amountJ, _d->midTile.getJ(), newTile, midTile);
         break;
      case D_SOUTH_WEST:
         dec(_sj, _d->pos.rj(), amountJ, _d->midTile.getJ(), newTile, midTile);
         dec(_si, _d->pos.ri(), amountI, _d->midTile.getI(), newTile, midTile);
         break;
      case D_WEST:
         dec(_si, _d->pos.ri(), amountI, _d->midTile.getI(), newTile, midTile);
         break;
      case D_NORTH_WEST:
         inc(_sj, _d->pos.rj(), amountJ, _d->midTile.getJ(), newTile, midTile);
         dec(_si, _d->pos.ri(), amountI, _d->midTile.getI(), newTile, midTile);
         break;
      default:
         THROW("Invalid move direction: " << _action._direction);
         break;
      }

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

   _ii = _d->pos.getI()*15+_si;
   _jj = _d->pos.getJ()*15+_sj;
}


void Walker::onNewTile()
{
   // std::cout << "Walker is on a new tile! coord=" << _i << "," << _j << std::endl;
   Tilemap& tilemap = Scenario::instance().getCity().getTilemap();
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
   if (_pathWay.isDestination())
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
   _action._action=WA_NONE;  // stop moving
   _animation = Animation();
}

void Walker::onNewDirection()
{
   _animation = Animation();  // need to fetch the new animation
}


void Walker::computeDirection()
{
   DirectionType lastDirection = _action._direction;
   _action._direction = _pathWay.getNextDirection();

   if (lastDirection != _action._direction)
   {
      onNewDirection();
   }
}


DirectionType Walker::getDirection()
{
   return _action._direction;
}

void Walker::getPictureList(std::vector<Picture*> &oPics)
{
   oPics.clear();
   oPics.push_back(&getMainPicture());
}

Picture& Walker::getMainPicture()
{
   if( !_animation.isValid() )
   {
      const std::map<WalkerAction, Animation>& animMap = WalkerLoader::instance().getAnimationMap(getWalkerGraphic());
      std::map<WalkerAction, Animation>::const_iterator itAnimMap;
      if (_action._action == WA_NONE || _action._direction == D_NONE)
      {
         WalkerAction action;
         action._action = WA_MOVE;       // default action
         if (_action._direction == D_NONE)
         {
            action._direction = D_NORTH;  // default direction
         }
         else
         {
            action._direction = _action._direction;  // last direction of the walker
         }
         itAnimMap = animMap.find(action);
      }
      else
      {
         itAnimMap = animMap.find(_action);
      }

      _animation = itAnimMap->second;
   }

   return *_animation.getCurrentPicture();
}

void Walker::save( VariantMap& stream ) const
{
  //stream[ "id" ] = this;
  stream[ "type" ] = (int)_walkerType;

  VariantMap vm_path;
  _pathWay.save( vm_path );
  stream[ "pathway" ] = vm_path;

  stream[ "action" ] = (int)_action._action;
  stream[ "direction" ] = (int)_action._direction;
  
  stream[ "pos" ] = _d->pos;
  stream[ "si" ] = _si;
  stream[ "sj" ] = _sj;
  stream[ "ii" ] = _ii;
  stream[ "jj" ] = _jj;
  stream[ "speed" ] = _d->speed;
  stream[ "midTile" ] = _d->midTile;
  stream[ "speedMul" ] = _d->speedMultiplier;
}

void Walker::load( const VariantMap& stream)
{
  _pathWay.load( stream.get( "pathway" ).toMap() );
  _action._action = (WalkerActionType) stream.get( "action" ).toInt();
  _action._direction = (DirectionType) stream.get( "direction" ).toInt();
  _d->pos = stream.get( "pos" ).toTilePos();
  _si = stream.get( "si" ).toInt();
  _sj = stream.get( "sj" ).toInt();
  _ii = stream.get( "ii" ).toInt();
  _jj = stream.get( "jj" ).toInt();
  _d->speedMultiplier = stream.get( "speedMul" ).toFloat();
  _d->speed = stream.get( "speed" ).toFloat();
  _d->midTile = stream.get( "midTile" ).toTilePos();
}

TilePos Walker::getIJ() const
{
    return _d->pos;
}

void Walker::deleteLater()
{
   _isDeleted = true;
}

Soldier::Soldier()
{
   _walkerType = WT_SOLDIER;
   _walkerGraphic = WG_HORSEMAN;
}

