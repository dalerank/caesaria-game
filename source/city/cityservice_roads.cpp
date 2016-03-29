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
  struct {
    int increase;
    int decrease;
  } paved;

  DateTime lastTimeUpdate;

  void updateRoadsAround(Propagator& propagator, UpdateInfo info );
};

std::string Roads::defaultName(){  return TEXT(Roads);}

Roads::Roads( PlayerCityPtr city )
  : Srvc( city, Roads::defaultName() ), _d( new Impl )
{
  _d->paved.increase = 4;
  _d->paved.decrease = -1;
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
  for( auto type : _d->btypes )
  {
    BuildingList tmp = _city()->statistic().objects.find<Building>( type.first );

    for( auto b : tmp )
    {
      positions.push_back( Impl::UpdateInfo( b.object(), type.second ) );
    }
  }

  HouseList houses = _city()->statistic().houses.find();
  for( auto house : houses )
  {
    if( house->level() >= HouseLevel::bigMansion )
    {
      positions.push_back( Impl::UpdateInfo( house.object(), 5 ) );
    }
  }

  Propagator propagator( _city() );
  for( auto& upos : positions )
  {
    _d->updateRoadsAround( propagator, upos );
  }

  if( (int)_d->lastTimeUpdate.month() % 3 == 1 )
  {
    RoadList roads = _city()->statistic().objects.find<Road>( object::road );
    for( auto road : roads )
    {
      road->appendPaved( _d->paved.decrease );
    }
  }
}

Roads::~Roads() {}

void Roads::Impl::updateRoadsAround( Propagator& propagator, UpdateInfo info )
{
  propagator.init( info.first );
  PathwayList pathWayList = propagator.getWays( info.second );

  for( auto& path : pathWayList )
  {
    RoadList roads = path->allTiles().overlays<Road>();
    for( auto road : roads )
      road->appendPaved( paved.increase );
  }
}

}//end namesapce city
