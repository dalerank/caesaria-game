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

#include "building.hpp"

#include "gfx/tile.hpp"
#include "walker/serviceman.hpp"
#include "core/exception.hpp"
#include "game/resourcegroup.hpp"
#include "core/variant.hpp"
#include "walker/trainee.hpp"
#include "core/stringhelper.hpp"
#include "city/city.hpp"
#include "core/foreach.hpp"
#include "gfx/tilemap.hpp"
#include "events/event.hpp"
#include "core/logger.hpp"
#include "constants.hpp"
#include "game/gamedate.hpp"

using namespace constants;
using namespace gfx;

namespace {
static Renderer::PassQueue buildingPassQueue=Renderer::PassQueue(1,Renderer::building);
}

class Building::Impl
{
public:
  typedef std::map< constants::walker::Type, int> TraineeMap;
  TraineeMap traineeMap;  // current level of trainees working in the building (0..200)
  int stateDecreaseInterval;
};

Building::Building(const TileOverlay::Type type, const Size& size )
: Construction( type, size ), _d( new Impl )
{
  setState( Construction::inflammability, 1 );
  setState( Construction::collapsibility, 1 );
  _d->stateDecreaseInterval = GameDate::ticksInMonth() / 25;
}

Building::~Building() {}

void Building::initTerrain( Tile &tile )
{
  // here goes the problem
  // when we reset tile, we delete information
  // about it's original information
  // try to fix
  bool saveMeadow = tile.getFlag( Tile::tlMeadow );
  bool saveWater = tile.getFlag( Tile::tlWater );
  tile.setFlag( Tile::clearAll, true );
  tile.setFlag( Tile::tlMeadow, saveMeadow);
  tile.setFlag( Tile::tlWater, saveWater );
}

void Building::timeStep(const unsigned long time)
{
  if( time % _d->stateDecreaseInterval == 1)
  {
    updateState( Construction::damage, getState( Construction::collapsibility ) );
    updateState( Construction::fire, getState( Construction::inflammability ) );
  }

  Construction::timeStep(time);
}

void Building::storeGoods(GoodStock &stock, const int amount)
{
  std::string bldType = getDebugName();
  Logger::warning( "This building should not store any goods %s at [%d,%d]",
                   bldType.c_str(), pos().i(), pos().j() );
  try
  {
   _CAESARIA_DEBUG_BREAK_IF("This building should not store any goods");
  }
  catch(...)
  {

  }
}

float Building::evaluateService(ServiceWalkerPtr walker)
{
   float res = 0.0;
   Service::Type service = walker->getService();
   if(_reservedServices.count(service) == 1)
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

void Building::reserveService(const Service::Type service) {   _reservedServices.insert(service);}
void Building::cancelService(const Service::Type service){   _reservedServices.erase(service);}

void Building::applyService( ServiceWalkerPtr walker)
{
   // std::cout << "apply service" << std::endl;
   // remove service reservation
   Service::Type service = walker->getService();
   _reservedServices.erase(service);

   switch( service )
   {
   case Service::engineer: setState( Construction::damage, 0 ); break;
   case Service::prefect: setState( Construction::fire, 0 ); break;
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

   int trValue = getTraineeValue( traineeType );
   if( trValue >= 0 )
   {
      res = (float)( 100 - trValue );
   }

   return res;
}

void Building::reserveTrainee(walker::Type traineeType) { _reservedTrainees.insert(traineeType); }
void Building::cancelTrainee(walker::Type traineeType) { _reservedTrainees.erase(traineeType);}

void Building::updateTrainee(  TraineeWalkerPtr walker )
{
   _reservedTrainees.erase( walker->type() );
   _d->traineeMap[ walker->type() ] += walker->getValue() ;
}

void Building::setTraineeValue(walker::Type type, int value)
{
  _d->traineeMap[ type ] = value;
}

int Building::getTraineeValue(walker::Type traineeType) const
{
  Impl::TraineeMap::iterator i = _d->traineeMap.find( traineeType );
  return i != _d->traineeMap.end() ? i->second : -1;
}

Renderer::PassQueue Building::getPassQueue() const {  return buildingPassQueue;}



// govt     1  - small statue        1 x 1
// govt     2  - medium statue       2 x 2
// govt     3  - big statue          3 x 3

// land3a 43 44 - triumphal arch
// land3a 45 46 - triumphal arch

// transport 93 - missionaire post   2 x 2
// circus    1 ~ 18 hippodrome    5x(5 x 5)

SmallStatue::SmallStatue() : Building( building::smallStatue, Size(1) )
{
  setState( Construction::inflammability, 0 );
  setState( Construction::collapsibility, 0 );

  setPicture( ResourceGroup::govt, 1 );
}

bool SmallStatue::isNeedRoadAccess() const {  return false; }

MediumStatue::MediumStatue() : Building( building::middleStatue, Size(2) )
{
  setState( Construction::inflammability, 0 );
  setState( Construction::collapsibility, 0 );

  setPicture( ResourceGroup::govt, 2);
}

bool MediumStatue::isNeedRoadAccess() const {  return false; }

BigStatue::BigStatue() : Building( building::bigStatue, Size(3))
{
  setState( Construction::inflammability, 0 );
  setState( Construction::collapsibility, 0 );

  setPicture( ResourceGroup::govt, 3 );
}

bool BigStatue::isNeedRoadAccess() const {  return false;}

// second arch pictures is land3a 45 + 46	
TriumphalArch::TriumphalArch() : Building( building::triumphalArch, Size(3) )
{
  setPicture( ResourceGroup::land3a, 43 );
  _animationRef().load("land3a", 44, 1);
  _animationRef().setOffset( Point( 63, 97 ) );
  _fgPicturesRef().resize(1);
  _fgPicturesRef()[0] = _animationRef().currentFrame();
}
