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

#include "cityservice_roads.hpp"
#include "objects/construction.hpp"
#include "city/statistic.hpp"
#include "game/gamedate.hpp"
#include "pathway/path_finding.hpp"
#include "gfx/tilemap.hpp"
#include "objects/house.hpp"
#include "objects/house_spec.hpp"
#include "objects/road.hpp"
#include "objects/constants.hpp"
#include "cityservice_factory.hpp"
#include "config.hpp"

using namespace gfx;
using namespace config;

namespace city
{

REGISTER_SERVICE_IN_FACTORY(Roads,roads)

class Roads::Impl
{
public:
  typedef std::pair< ConstructionPtr, int > UpdateInfo;
  typedef std::vector< UpdateInfo > Updates;
  typedef std::pair<object::Type, int> UpdateBuilding;
  typedef std::vector< UpdateBuilding > BuildingInfo;

  BuildingInfo btypes;
  int defaultIncreasePaved;
  int defaultDecreasePaved;

  DateTime lastTimeUpdate;

  void updateRoadsAround(Propagator& propagator, UpdateInfo info );
};

SrvcPtr Roads::create(PlayerCityPtr city)
{
  SrvcPtr ret( new Roads( city ) );
  ret->drop();
  return ret;
}

std::string Roads::defaultName(){  return CAESARIA_STR_A(Roads);}

Roads::Roads( PlayerCityPtr city )
  : Srvc( city, Roads::defaultName() ), _d( new Impl )
{
  _d->defaultIncreasePaved = 4;
  _d->defaultDecreasePaved = -1;
  _d->lastTimeUpdate = game::Date::current();

  _d->btypes.push_back( Impl::UpdateBuilding(object::senate, desirability::senateInfluence) );
  _d->btypes.push_back( Impl::UpdateBuilding(object::small_ceres_temple, desirability::normalInfluence));
  _d->btypes.push_back( Impl::UpdateBuilding(object::small_mars_temple, desirability::normalInfluence));
  _d->btypes.push_back( Impl::UpdateBuilding(object::small_mercury_temple, desirability::normalInfluence));
  _d->btypes.push_back( Impl::UpdateBuilding(object::small_neptune_temple, desirability::normalInfluence));
  _d->btypes.push_back( Impl::UpdateBuilding(object::small_venus_temple, desirability::normalInfluence));
}

void Roads::timeStep( const unsigned int time )
{
  if( _d->lastTimeUpdate.month() == game::Date::current().month() )
    return;

  _d->lastTimeUpdate = game::Date::current();  

  Impl::Updates positions;
  foreach( it, _d->btypes )
  {
    BuildingList tmp = statistic::getObjects<Building>( _city(), it->first );

    foreach( b, tmp )
    {
      positions.push_back( Impl::UpdateInfo( b->object(), it->second ) );
    }
  }

  HouseList houses = city::statistic::getHouses( _city() );
  foreach( house, houses )
  {
    if( (*house)->spec().level() >= HouseLevel::bigMansion )
    {
      positions.push_back( Impl::UpdateInfo( house->object(), 5 ) );
    }
  }

  Propagator propagator( _city() );
  foreach( upos, positions )
  {
    _d->updateRoadsAround( propagator, *upos );
  }

  if( _d->lastTimeUpdate.month() % 3 == 1 )
  {
    RoadList roads = statistic::getObjects<Road>( _city(), object::road );
    foreach( road, roads )
    {
      (*road)->appendPaved( _d->defaultDecreasePaved );
    }
  }
}

Roads::~Roads() {}

void Roads::Impl::updateRoadsAround( Propagator& propagator, UpdateInfo info )
{
  propagator.init( info.first );
  PathwayList pathWayList = propagator.getWays( info.second );

  foreach( current, pathWayList )
  {
    const TilesArray& tiles = (*current)->allTiles();
    foreach( it, tiles )
    {
      RoadPtr road = ptr_cast<Road>( (*it)->overlay() );
      if( road.isValid() )
      {
        road->appendPaved( defaultIncreasePaved );
      }
    }
  }
}

}//end namesapce city
