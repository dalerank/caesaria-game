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

#include "oc3_building_senate.hpp"
#include "oc3_picture.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_cityfunds.hpp"
#include "oc3_walker_taxcollector.hpp"
#include "oc3_city.hpp"

// govt 4  - senate
// govt 9  - advanced senate
// govt 5 ~ 8 - senate flags

class Senate::Impl
{
public:
  int taxInLastMonth;
};

Senate::Senate() : ServiceBuilding( Service::S_SENATE, B_SENATE, Size(5) ), _d( new Impl )
{
  setWorkers( 0 );
  setPicture( Picture::load( ResourceGroup::govt, 4) );
  _d->taxInLastMonth = 0;
}

bool Senate::canBuild( CityPtr city, const TilePos& pos ) const
{
  bool mayBuild = ServiceBuilding::canBuild( city, pos );

  if( mayBuild )
  {
    CityHelper helper( city );
    bool isSenatePresent = helper.getBuildings<Building>(B_SENATE).size() > 0;
    mayBuild &= !isSenatePresent;
  }

  return mayBuild;
}

unsigned int Senate::getFunds() const
{
  return _getCity()->getFunds().getValue();
}

int Senate::collectTaxes()
{
  return 0;
}

int Senate::getPeoplesReached() const
{
  return 0;
}

void Senate::deliverService()
{
  if( getWorkers() > 0 && getWalkerList().size() == 0 )
  {
    TaxCollectorPtr walker = TaxCollector::create( _getCity() );
    walker->send2City( this );

    if( !walker->isDeleted() )
    {
      addWalker( walker.as<Walker>() );
    }
  }
}
