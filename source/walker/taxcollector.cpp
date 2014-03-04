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
#include "city/helper.hpp"
#include "city/funds.hpp"
#include "objects/house.hpp"
#include "name_generator.hpp"
#include "constants.hpp"
#include "pathway/pathway.hpp"
#include "objects/senate.hpp"
#include "objects/forum.hpp"
#include "core/foreach.hpp"
#include "objects/house_level.hpp"
#include "core/logger.hpp"
#include "core/stringhelper.hpp"

using namespace constants;

class TaxCollector::Impl
{
public:
  float money;

  std::map< std::string, float > history;
};

void TaxCollector::_changeTile()
{
  Walker::_changeTile();

  ReachedBuildings buildings = getReachedBuildings( pos() );
  foreach( it, buildings )
  {
    HousePtr house = ptr_cast<House>( *it );

    if( house.isValid() )
    {
      float tax = house->collectTaxes();
      _d->money += tax;
      house->applyService( this );

      std::string posStr = StringHelper::format( 0xff, "%02dx%02d", house->pos().i(), house->pos().j() );
      _d->history[ posStr ] += tax;
    }
  }
}

std::string TaxCollector::getThinks() const
{
  city::Helper helper( _getCity() );
  TilePos offset( 2, 2 );
  HouseList houses = helper.find<House>( building::house, pos() - offset, pos() + offset );
  unsigned int poorHouseCounter=0;

  foreach( h, houses )
  {
    if( (*h)->getSpec().taxRate() < 10 )
    {
      poorHouseCounter++;
    }
  }

  if( poorHouseCounter > houses.size() / 2 )
  {
    return "##tax_collector_very_little_tax##";
  }

  return ServiceWalker::getThinks();
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

    Logger::warning( "TaxCollector: path history" );
    foreach( it, _d->history )
    {
      Logger::warning( "       [%s]:%f", it->first.c_str(), it->second );
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
