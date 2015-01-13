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
#include "city/helper.hpp"
#include "game/gamedate.hpp"
#include "pathway/path_finding.hpp"
#include "gfx/tilemap.hpp"
#include "objects/house.hpp"
#include "objects/house_level.hpp"
#include "objects/road.hpp"
#include "objects/constants.hpp"

using namespace constants;
using namespace gfx;

namespace city
{

class Roads::Impl
{
public:
  typedef std::pair< ConstructionPtr, int > UpdateInfo;
  typedef std::vector< UpdateInfo > Updates;
  typedef std::pair<TileOverlay::Type, int> UpdateBuilding;

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

std::string Roads::defaultName(){  return "roads";}

Roads::Roads( PlayerCityPtr city )
  : Srvc( city, Roads::defaultName() ), _d( new Impl )
{
  _d->defaultIncreasePaved = 4;
  _d->defaultDecreasePaved = -1;
  _d->lastTimeUpdate = game::Date::current();
}

void Roads::timeStep( const unsigned int time )
{
  if( _d->lastTimeUpdate.month() == game::Date::current().month() )
    return;

  _d->lastTimeUpdate = game::Date::current();

  std::vector< Impl::UpdateBuilding > btypes;
  btypes.push_back( Impl::UpdateBuilding(objects::senate, 10) );
  btypes.push_back( Impl::UpdateBuilding(objects::small_ceres_temple, 4));
  btypes.push_back( Impl::UpdateBuilding(objects::small_mars_temple, 4));
  btypes.push_back( Impl::UpdateBuilding(objects::small_mercury_temple, 4));
  btypes.push_back( Impl::UpdateBuilding(objects::small_neptune_temple, 4));
  btypes.push_back( Impl::UpdateBuilding(objects::small_venus_temple, 4));

  Helper helper( _city() );

  Impl::Updates positions;
  foreach( it, btypes )
  {
    BuildingList tmp = helper.find<Building>( it->first );

    foreach( b, tmp )
    {
      positions.push_back( Impl::UpdateInfo( b->object(), it->second ) );
    }
  }

  HouseList houses = helper.find<House>( objects::house );
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
    RoadList roads = helper.find<Road>( objects::road );
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
