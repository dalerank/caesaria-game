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
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "fountain.hpp"

#include "core/utils.hpp"
#include "core/time.hpp"
#include "core/position.hpp"
#include "game/resourcegroup.hpp"
#include "core/safetycast.hpp"
#include "watersupply.hpp"
#include "objects/road.hpp"
#include "gfx/tile.hpp"
#include "walker/serviceman.hpp"
#include "city/city.hpp"
#include "core/foreach.hpp"
#include "gfx/tilemap.hpp"
#include "core/variant_map.hpp"
#include "core/logger.hpp"
#include "constants.hpp"
#include "game/gamedate.hpp"
#include "walker/workerhunter.hpp"
#include "events/returnworkers.hpp"
#include "gfx/tilearea.hpp"
#include "objects_factory.hpp"
#include "city/states.hpp"

using namespace gfx;
using namespace events;

REGISTER_CLASS_IN_OVERLAYFACTORY(object::fountain, Fountain)

namespace {
static const unsigned int fillDistanceNormal = 4;
static const unsigned int fillDistanceDesert = 3;
}

typedef enum { prettyFountain=2, fontainEmpty = 3, fontainFull = 4, simpleFountain = 10, fontainSizeAnim = 7,
               awesomeFountain=18, patricianFountain=26, testFountain=10 } FontainConstant;

class Fountain::Impl
{
public:
  bool haveReservoirWater;
  int  waterIncreaseInterval;
  int  lastPicId;
  int  fillDistance;
};

Fountain::Fountain()
  : ServiceBuilding(Service::fountain, object::fountain, Size(1,1)),
    _d(new Impl)
{  
  _picture().load( ResourceGroup::utilitya, 10 );
  _d->haveReservoirWater = false;
  _d->lastPicId = simpleFountain;
  _fgPictures().resize(1);
  _initAnimation();
  _d->fillDistance = 4;
}

void Fountain::deliverService()
{
  if( !_d->haveReservoirWater )
    return;

  auto serviceMan = Walker::create<ServiceWalker>( _city(), serviceType() );
  serviceMan->setBase( this );
  serviceMan->setReachDistance( 4 );
  ReachedBuildings reachedBuildings = serviceMan->getReachedBuildings( tile().pos() );

  for( auto b : reachedBuildings )
    b->applyService( serviceMan );
}

void Fountain::timeStep(const unsigned long time)
{
  //filled area, that fontain present and work
  if( game::Date::isDayChanged() )
  {
    _dayUpdate();
  }

  if( game::Date::isWeekChanged() )
  {
    int desPic[] = { simpleFountain, testFountain, prettyFountain, awesomeFountain, patricianFountain };
    int currentId = desPic[ math::clamp<int>( tile().param( Tile::pDesirability ) / 20, 0, 4 ) ];
    if( currentId != _d->lastPicId )
    {
      _d->lastPicId = currentId;
      _picture().load( ResourceGroup::utilitya, currentId );
      _initAnimation();
    }

    if( needWorkers() > 0 )
    {
      auto recruter = Walker::create<Recruter>( _city() );
      recruter->once( this, needWorkers(), _d->fillDistance * 2);
    }
  }  

  ServiceBuilding::timeStep( time );
}

bool Fountain::canBuild( const city::AreaInfo& areaInfo ) const
{
  bool ret = Construction::canBuild( areaInfo );

  Tilemap& tmap = areaInfo.city->tilemap();
  const Tile& tile = tmap.at( areaInfo.pos );
  Fountain* thisp = const_cast< Fountain* >( this );
  thisp->_fgPictures().clear();
  thisp->setPicture( ResourceGroup::utilitya, 10 );

  if( tile.param( Tile::pReservoirWater ) )
  {
    thisp->_fgPictures().push_back( Picture( ResourceGroup::utilitya, 11 ) );
  }

  return ret;
}

bool Fountain::build( const city::AreaInfo& info )
{
  ServiceBuilding::build( info );

  _picture().load( ResourceGroup::utilitya, 10 );
  _d->lastPicId = simpleFountain;
  _initAnimation();

  _d->fillDistance = (info.city->climate() == game::climate::desert)
                     ? fillDistanceDesert
                     : fillDistanceNormal;

  setState( pr::inflammability, 0 );
  setState( pr::collapsibility, 0 );
  return true;
}

bool Fountain::isNeedRoad() const { return false; }

bool Fountain::haveReservoirAccess() const
{
  return TilesArea( _map(), 10, pos() ).overlays().count<Reservoir>() > 0;
}

void Fountain::destroy()
{
  ServiceBuilding::destroy();

  TilesArea reachedTiles( _map(), _d->fillDistance, pos() );

  for( auto tile : reachedTiles )
    tile->setParam( Tile::pFountainWater, 0 );

  if( numberWorkers() > 0 )
  {
    events::dispatch<ReturnWorkers>( pos(), numberWorkers() );
  }
}

bool Fountain::mayWork() const {  return ServiceBuilding::mayWork() && ServiceBuilding::isActive() && _d->haveReservoirWater; }

unsigned int Fountain::fillRange() const { return _d->fillDistance; }

void Fountain::load(const VariantMap& stream)
{
  ServiceBuilding::load( stream );

  VARIANT_LOAD_ANYDEF_D( _d, lastPicId, simpleFountain, stream )
  VARIANT_LOAD_ANY_D( _d, haveReservoirWater, stream );
  setPicture( ResourceGroup::utilitya, _d->lastPicId );
  _initAnimation();
  //check animation
  timeStep( 1 );
}

void Fountain::save(VariantMap& stream) const
{
  ServiceBuilding::save( stream );
  VARIANT_SAVE_ANY_D( stream, _d, lastPicId )
  VARIANT_SAVE_ANY_D( stream, _d, haveReservoirWater );
}

void Fountain::_initAnimation()
{
  _animation().clear();
  _animation().load( ResourceGroup::utilitya, _d->lastPicId+1, fontainSizeAnim );
  _animation().setDelay( 2 );
  _fgPicture( 0 ) = Picture::getInvalid();
  _animation().stop();
}

void Fountain::_dayUpdate()
{
  _d->haveReservoirWater = tile().param( Tile::pReservoirWater ) > 0;

  if( mayWork() )
  {
    TilesArea reachedTiles( _map(), _d->fillDistance, pos() );

    for( auto tile : reachedTiles )
    {
      int value = tile->param( Tile::pFountainWater );
      tile->setParam( Tile::pFountainWater, math::clamp( value+1, 0, 20 ) );
    }
  }
  else
  {

  }
}
