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
#include "house.hpp"
#include "city/statistic.hpp"
#include "constants.hpp"
#include "objects_factory.hpp"

using namespace gfx;

REGISTER_CLASS_IN_OVERLAYFACTORY(object::well, Well)

namespace {
const unsigned int wellServiceRange = 2;
}

Well::Well() : ServiceBuilding( Service::well, object::well, Size(1) )
{
  setWorkers( 0 );
}

void Well::deliverService()
{
  ServiceWalkerPtr walker = ServiceWalker::create( _city(), serviceType() );
  walker->setBase( BuildingPtr( this ) );

  ServiceWalker::ReachedBuildings reachedBuildings = walker->getReachedBuildings( tile().pos() );

  unsigned int lowHealth = 100;
  HouseList houses;
  foreach( it, reachedBuildings)
  {
    (*it)->applyService( walker );
    HousePtr house = ptr_cast<House>( *it );
    if( house.isValid() )
    {
      lowHealth = std::min<unsigned int>( lowHealth, house->state(pr::health ) );
      houses << house;
    }
  }

  if( lowHealth < 30 )
  {
    lowHealth = (100 - lowHealth) / 10;
    foreach( it, houses)
    {
      if( (*it)->state( pr::health ) > 10 )
      {
        (*it)->updateState( pr::health, -lowHealth );
      }
    }
  }
}

bool Well::isNeedRoad() const {  return false; }
void Well::burn() { collapse(); }
bool Well::isDestructible() const{  return true; }

std::string Well::sound() const
{
  return ServiceBuilding::sound();
}

bool Well::build( const city::AreaInfo& info )
{
  ServiceBuilding::build( info );

  Picture rpic = MetaDataHolder::randomPicture( type(), size() );
  if( !rpic.isValid() )
    rpic.load( ResourceGroup::utilitya, 1 );

  setPicture( rpic );

  setState( pr::inflammability, 0 );
  setState( pr::collapsibility, 0 );
  return true;
}

TilesArea Well::coverageArea() const
{
  TilesArea ret( _city()->tilemap(), pos(), wellServiceRange );
  return ret;
}
