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
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

#include "cityservice_roads.hpp"
#include "city/helper.hpp"
#include "game/gamedate.hpp"
#include "pathway/path_finding.hpp"
#include "gfx/tilemap.hpp"
#include "objects/house.hpp"
#include "objects/house_level.hpp"
#include "objects/road.hpp"
#include "objects/constants.hpp"

using namespace constants;

namespace city
{

class Roads::Impl
{
public:
  typedef std::pair< ConstructionPtr, int > UpdateInfo;
  typedef std::vector< UpdateInfo > Updates;

  int defaultIncreasePaved;
  int defaultDecreasePaved;

  DateTime lastTimeUpdate;
  ScopedPtr< Propagator > propagator;

  void updateRoadsAround( UpdateInfo info );
};

SrvcPtr Roads::create(PlayerCityPtr city)
{
  Roads* ret = new Roads( city );

  return SrvcPtr( ret );
}

std::string Roads::getDefaultName(){  return "roads";}

Roads::Roads(PlayerCityPtr city )
  : Srvc( *city.object(), Roads::getDefaultName() ), _d( new Impl )
{
  _d->defaultIncreasePaved = 4;
  _d->defaultDecreasePaved = -1;
  _d->lastTimeUpdate = GameDate::current();
  _d->propagator.reset( new Propagator( city ) );
}

void Roads::update( const unsigned int time )
{
  if( _d->lastTimeUpdate.month() == GameDate::current().month() )
    return;

  _d->lastTimeUpdate = GameDate::current();

  std::vector< TileOverlay::Type > btypes;
  btypes.push_back( building::senate );

  Helper helper( &_city );


  Impl::Updates positions;
  foreach( it, btypes )
  {
    BuildingList tmp = helper.find<Building>( *it );

    foreach( b, tmp )
    {
      positions.push_back( Impl::UpdateInfo( b->object(), 10 ) );
    }
  }

  HouseList houses = helper.find<House>( building::house );
  foreach( house, houses )
  {
    if( (*house)->getSpec().level() >= HouseLevel::bigMansion )
    {
      positions.push_back( Impl::UpdateInfo( house->object(), 5 ) );
    }
  }

  foreach( upos, positions ) { _d->updateRoadsAround( *upos ); }

  if( _d->lastTimeUpdate.month() % 3 == 1 )
  {
    RoadList roads = helper.find<Road>( construction::road );
    foreach( road, roads )
    {
      (*road)->appendPaved( _d->defaultDecreasePaved );
    }
  }
}

Roads::~Roads() {}

void Roads::Impl::updateRoadsAround( UpdateInfo info )
{
  propagator->init( info.first );
  PathwayList pathWayList = propagator->getWays( info.second );

  foreach( current, pathWayList )
  {
    const TilesArray& tiles = (*current)->allTiles();
    for( TilesArray::const_iterator it=tiles.begin(); it != tiles.end(); ++it )
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
