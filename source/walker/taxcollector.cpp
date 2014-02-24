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

#include "taxcollector.hpp"
#include "city/city.hpp"
#include "city/funds.hpp"
#include "objects/house.hpp"
#include "name_generator.hpp"
#include "constants.hpp"
#include "pathway/pathway.hpp"
#include "objects/senate.hpp"
#include "objects/forum.hpp"
#include "core/foreach.hpp"

using namespace constants;

class TaxCollector::Impl
{
public:
  float money;
};

void TaxCollector::_centerTile()
{
  ServiceWalker::_centerTile();

  ReachedBuildings buildings = getReachedBuildings( pos() );
  foreach( it, buildings )
  {
    HousePtr house = ptr_cast<House>( *it );
    _d->money += house.isValid() ? house->collectTaxes() : 0;
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
  if( _pathwayRef().isReverse() )
  {
    if( getBase().isValid() )
    {
      getBase()->applyService( this );
    }
  }

  ServiceWalker::_reachedPathway();  
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
