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

#include "oc3_walker_taxcollector.hpp"
#include "oc3_city.hpp"
#include "oc3_house.hpp"

class TaxCollector::Impl
{
public:
  int money;
  int peoplesReached;
};

void TaxCollector::onMidTile()
{
  ServiceWalker::onMidTile();

  ReachedBuildings buildings = getReachedBuildings( getIJ() );
  for( ReachedBuildings::iterator it=buildings.begin(); it != buildings.end(); it++ )
  {
    HousePtr house = (*it).as<House>();
    if( house.isValid() )
    {
      int money = house->collectTaxes();
      _d->money += money;
      _d->peoplesReached += money > 0 ? house->getNbHabitants() : 0;
    }
  }
}

TaxCollectorPtr TaxCollector::create( CityPtr city )
{
  TaxCollectorPtr tc( new TaxCollector( city ) );
  tc->drop();

  return tc;
}

TaxCollector::TaxCollector( CityPtr city ) : ServiceWalker( city, S_FORUM ), _d( new Impl )
{
  _d->money = 0;
  _walkerType = WT_TAXCOLLECTOR;
}

int TaxCollector::getMoney() const
{
  return _d->money;
}

int TaxCollector::getCitizensReachedCount() const
{
  return _d->peoplesReached;
}