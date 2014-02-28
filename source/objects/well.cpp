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

#include "well.hpp"
#include "game/resourcegroup.hpp"
#include "walker/serviceman.hpp"
#include "gfx/tile.hpp"
#include "city/helper.hpp"
#include "constants.hpp"

using namespace constants;

namespace {
 const unsigned int wellServiceRange = 2;
}

Well::Well() : ServiceBuilding( Service::well, building::well, Size(1) )
{
  updateState( Construction::inflammability, 0, false );
  updateState( Construction::collapsibility, 0, false );

  setWorkers( 0 );
}

void Well::deliverService()
{
  ServiceWalkerPtr walker = ServiceWalker::create( _getCity(), getService() );
  walker->setBase( BuildingPtr( this ) );

  ServiceWalker::ReachedBuildings reachedBuildings = walker->getReachedBuildings( getTile().pos() );

  foreach( it, reachedBuildings) { (*it)->applyService( walker ); }
}

bool Well::isNeedRoadAccess() const {  return false; }
bool Well::isDestructible() const{  return true; }

TilesArray Well::getCoverageArea() const
{
  TilesArray ret;

  TilePos offset( wellServiceRange, wellServiceRange );
  CityHelper helper( _getCity() );
  ret = helper.getArea( pos() - offset, pos() + offset );
  return ret;
}
