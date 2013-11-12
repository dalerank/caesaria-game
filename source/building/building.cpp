
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

#include "building.hpp"

#include "gfx/tile.hpp"
#include "walker/serviceman.hpp"
#include "core/exception.hpp"
#include "building/metadata.hpp"
#include "game/resourcegroup.hpp"
#include "core/variant.hpp"
#include "core/stringhelper.hpp"
#include "game/city.hpp"
#include "core/foreach.hpp"
#include "game/tilemap.hpp"
#include "events/event.hpp"
#include "core/logger.hpp"
#include "constants.hpp"

using namespace constants;

Building::Building(const TileOverlay::Type type, const Size& size )
: Construction( type, size )
{
   _damageIncrement = 1;
   _fireIncrement = 1;
}

void Building::initTerrain( Tile &tile )
{
  // here goes the problem
  // when we reset tile, we delete information
  // about it's original information
  // try to fix
  bool saveMeadow = tile.getFlag( Tile::tlMeadow );
  tile.setFlag( Tile::clearAll, true );
  tile.setFlag( Tile::tlBuilding, true);
  tile.setFlag( Tile::tlMeadow, saveMeadow);
}

void Building::timeStep(const unsigned long time)
{
   if (time % 64 == 0)
   {
      updateState( Construction::damage, _damageIncrement );
      updateState( Construction::fire, _fireIncrement );
   }

   Construction::timeStep(time);
}

void Building::storeGoods(GoodStock &stock, const int amount)
{
   _OC3_DEBUG_BREAK_IF("This building should not store any goods");
}

float Building::evaluateService(ServiceWalkerPtr walker)
{
   float res = 0.0;
   Service::Type service = walker->getService();
   if (_reservedServices.count(service) == 1)
   {
      // service is already reserved
      return 0.0;
   }

   switch(service)
   {
   case Service::engineer: res = getState( Construction::damage ); break;
   case Service::prefect: res = getState( Construction::fire ); break;
   default: break;
   }
   return res;
}

void Building::reserveService(const Service::Type service)
{
   // std::cout << "reserved service" << std::endl;
   _reservedServices.insert(service);
}

void Building::cancelService(const Service::Type service)
{
   // std::cout << "cancel service" << std::endl;
   _reservedServices.erase(service);
}

void Building::applyService( ServiceWalkerPtr walker)
{
   // std::cout << "apply service" << std::endl;
   // remove service reservation
   Service::Type service = walker->getService();
   _reservedServices.erase(service);

   switch( service )
   {
   case Service::engineer: updateState( Construction::damage, 0, false ); break;
   case Service::prefect: updateState( Construction::fire, 0, false ); break;
   default: break;
   }
}

float Building::evaluateTrainee(walker::Type traineeType)
{
   float res = 0.0;

   if( _reservedTrainees.count(traineeType) == 1 )
   {
      // don't allow two reservations of the same type
      return 0.0;
   }

   if( _traineeMap.count(traineeType) == 1 )
   {
      int currentLevel = _traineeMap[traineeType];
      res = (float)( 101 - currentLevel );
   }

   return res;
}

void Building::reserveTrainee(walker::Type traineeType)
{
   _reservedTrainees.insert(traineeType);
}

void Building::cancelTrainee(walker::Type traineeType)
{
   _reservedTrainees.erase(traineeType);
}

void Building::applyTrainee(walker::Type traineeType)
{
   _reservedTrainees.erase(traineeType);
   _traineeMap[traineeType] += 100;
}



// govt     1  - small statue        1 x 1
// govt     2  - medium statue       2 x 2
// govt     3  - big statue          3 x 3

// land3a 43 44 - triumphal arch
// land3a 45 46 - triumphal arch

// transport 93 - missionaire post   2 x 2
// circus    1 ~ 18 hippodrome    5x(5 x 5)

SmallStatue::SmallStatue() : Building( building::B_STATUE1, Size(1) )
{
  _fireIncrement = 0;
  _damageIncrement = 0;
  setPicture( ResourceGroup::govt, 1 );
}

bool SmallStatue::isNeedRoadAccess() const
{
  return false;
}

MediumStatue::MediumStatue() : Building( building::B_STATUE2, Size(2) )
{
  _fireIncrement = 0;
  _damageIncrement = 0;
  setPicture( ResourceGroup::govt, 2);
}

bool MediumStatue::isNeedRoadAccess() const
{
  return false;
}

BigStatue::BigStatue() : Building( building::B_STATUE3, Size(3))
{
  _fireIncrement = 0;
  _damageIncrement = 0;
  setPicture( ResourceGroup::govt, 3 );
}

bool BigStatue::isNeedRoadAccess() const
{
  return false;
}

Shipyard::Shipyard() : Building( building::shipyard, Size(2) )
{
  setPicture( ResourceGroup::transport, 1);
  // also transport 2 3 4 check position of river on map
}

// dock pictures
// transport 5        animation = 6~16
// transport 17       animation = 18~28
// transport 29       animation = 30~40
// transport 41       animation = 42~51

Dock::Dock() : Building( building::dock, Size(2) )
{
  setPicture( ResourceGroup::transport, 5);

  _getAnimation().load( ResourceGroup::transport, 6, 11);
  // now fill in reverse order
  _getAnimation().load( ResourceGroup::transport, 15, 10, Animation::reverse );
  
  _getAnimation().setOffset( Point( 107, 61 ) );
  _getFgPictures().resize(1);
}

void Dock::timeStep(const unsigned long time)
{
  _getAnimation().update( time );
  
  // takes current animation frame and put it into foreground
  _getFgPictures().at(0) = _getAnimation().getFrame();
}

// second arch pictures is land3a 45 + 46	

TriumphalArch::TriumphalArch() : Building( building::triumphalArch, Size(3) )
{
  setPicture( ResourceGroup::land3a, 43 );
  _getAnimation().load("land3a", 44, 1);
  _getAnimation().setOffset( Point( 63, 97 ) );
  _getFgPictures().resize(1);
  _getFgPictures().at(0) = _getAnimation().getFrame();
}
