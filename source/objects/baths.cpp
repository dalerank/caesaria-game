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

#include "baths.hpp"
#include "game/resourcegroup.hpp"
#include "game/gamedate.hpp"
#include "core/position.hpp"
#include "gfx/tilemap.hpp"
#include "city/statistic.hpp"
#include "constants.hpp"
#include "objects_factory.hpp"

using namespace gfx;

REGISTER_CLASS_IN_OVERLAYFACTORY(object::baths, Baths)

Baths::Baths() : HealthBuilding(Service::baths, object::baths, Size(2,2) )
{
  _haveReservorWater = false;
  _fgPictures().resize(1);
}

bool haveReservoirWater( const Tilemap& tm, const TilePos& pos )
{
  TilesArray possiblePlace = tm.area( pos, pos + TilePos(1,1) );
  return !possiblePlace.select( Tile::pReservoirWater ).empty();
}

unsigned int Baths::walkerDistance() const {  return 35;}

bool Baths::build( const city::AreaInfo& info )
{ 
  bool result = ServiceBuilding::build( info );
  _myArea = area();

  if (!haveReservoirWater(info.city->tilemap(), info.pos))
  {
    _setError( "##need_access_to_full_reservoir##" );
  }

  return result;
}

bool Baths::canBuild(const city::AreaInfo& areaInfo) const
{
  bool ok = HealthBuilding::canBuild( areaInfo );

  if( ok )
  {
    Baths* nc_this = const_cast<Baths*>( this );
    bool haveWater = haveReservoirWater( areaInfo.city->tilemap(), areaInfo.pos );
    nc_this->_fgPicture(0) = haveWater ? animation().frame( 0 ) : Picture::getInvalid();
  }

  return ok;
}

bool Baths::mayWork() const {  return ServiceBuilding::mayWork() && _haveReservorWater; }

void Baths::timeStep(const unsigned long time)
{
  if( game::Date::isWeekChanged() )
  {
    bool haveWater = !_myArea.select( Tile::pReservoirWater ).empty();
    _haveReservorWater = (haveWater && numberWorkers() > 0);
  }

  ServiceBuilding::timeStep( time );
}

void Baths::deliverService()
{
  if( _haveReservorWater && numberWorkers() > 0 && walkers().empty() )
  {
    HealthBuilding::deliverService();
  }
}
