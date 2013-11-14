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

#include "taxcollector.hpp"
#include "game/city.hpp"
#include "game/cityfunds.hpp"
#include "building/house.hpp"
#include "game/name_generator.hpp"
#include "constants.hpp"
#include "game/pathway.hpp"
#include "building/senate.hpp"
#include "building/forum.hpp"

using namespace constants;

class TaxCollector::Impl
{
public:
  float money;
};

void TaxCollector::_centerTile()
{
  ServiceWalker::_centerTile();

  float taxRate = _getCity()->getFunds().getTaxRate() / 100.f;

  ReachedBuildings buildings = getReachedBuildings( getIJ() );
  foreach( BuildingPtr building, buildings )
  {
    HousePtr house = building.as<House>();
    if( house.isValid() && house->ready2Taxation() )
    {
      float money = house->collectTaxes() * taxRate;
      _d->money += money;
    }
  }
}

TaxCollectorPtr TaxCollector::create(PlayerCityPtr city )
{
  TaxCollectorPtr tc( new TaxCollector( city ) );
  tc->drop();

  return tc;
}

TaxCollector::TaxCollector(PlayerCityPtr city ) : ServiceWalker( city, Service::forum ), _d( new Impl )
{
  _d->money = 0;
  _setType( walker::taxCollector );

  setName( NameGenerator::rand( NameGenerator::male ) );
}

int TaxCollector::getMoney() const
{
  int save = _d->money;
  _d->money = 0;
  return save;
}

void TaxCollector::_reachedPathway()
{
  ServiceWalker::_reachedPathway();

  if( _pathwayRef().isReverse() )
  {
    if( getBase().isValid() )
    {
      getBase()->evaluateService( this );
    }
  }
}

void TaxCollector::load(const VariantMap& stream)
{
  ServiceWalker::load( stream );

  _d->money = stream.get( "money" );
}

void TaxCollector::save(VariantMap& stream) const
{
  ServiceWalker::save( stream );
  stream[ "money" ] = _d->money;
}
