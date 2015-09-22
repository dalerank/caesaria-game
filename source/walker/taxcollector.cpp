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

#include "taxcollector.hpp"
#include "city/statistic.hpp"
#include "game/funds.hpp"
#include "objects/house.hpp"
#include "name_generator.hpp"
#include "constants.hpp"
#include "pathway/pathway_helper.hpp"
#include "objects/senate.hpp"
#include "objects/forum.hpp"
#include "core/foreach.hpp"
#include "objects/house_spec.hpp"
#include "core/logger.hpp"
#include "core/utils.hpp"
#include "gfx/tilemap.hpp"
#include "core/variant_map.hpp"
#include <game/settings.hpp>
#include "walkers_factory.hpp"

REGISTER_CLASS_IN_WALKERFACTORY(walker::taxCollector, TaxCollector)

class TaxCollector::Impl
{
public:
  float money;
  bool return2base;

  std::map< TilePos, float > history;
};

void TaxCollector::_centerTile()
{
  Walker::_centerTile();

  UqBuildings<House> houses = getReachedBuildings( pos() ).select<House>();
  for( auto house : houses )
  {
    if( house->isDeleted() || _d->history.count( house->pos() ) > 0 )
      continue;

    float tax = house->collectTaxes();
    _d->money += tax;
    house->applyService( this );
    _d->history[ house->pos() ] += tax;
  }
}

std::string TaxCollector::thoughts(Thought th) const
{
  if( th == thCurrent )
  {
    TilePos offset( 2, 2 );
    HouseList houses = _city()->statistic().objects.find<House>( object::house, pos() - offset, pos() + offset );
    unsigned int poorHouseCounter=0;
    unsigned int richHouseCounter=0;

    for( auto house : houses )
    {
      HouseLevel::ID level = (HouseLevel::ID)house->spec().level();
      if( level < HouseLevel::bigDomus ) poorHouseCounter++;
      else if( level >= HouseLevel::smallVilla ) richHouseCounter++;
    }

    if( poorHouseCounter > houses.size() / 2 ) { return "##tax_collector_very_little_tax##";  }
    if( richHouseCounter > houses.size() / 2 ) { return "##tax_collector_high_tax##";  }
  }

  return ServiceWalker::thoughts(th);
}

BuildingPtr TaxCollector::base() const
{
  return _map().overlay( baseLocation() ).as<Building>();
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
  _d->return2base = false;
  _setType( walker::taxCollector );

  setName( NameGenerator::rand( NameGenerator::male ) );
}

float TaxCollector::takeMoney() const
{
  float save = _d->money;
  _d->money = 0;
  return save;
}

void TaxCollector::_reachedPathway()
{
  if( _d->return2base )
  {
    if( base().isValid() )
    {
      base()->applyService( this );
    }

    Logger::warning( "TaxCollector: path history" );
    for( auto& step : _d->history )
      Logger::warning( "       [%02dx%02d]:%f", step.first.i(), step.first.j(), step.second );

    deleteLater();
    return;
  }
  else
  {
    _d->return2base = true;

    Pathway way = PathwayHelper::create( pos(), base(), PathwayHelper::roadFirst );
    if( way.isValid() )
    {
      _updatePathway( way );
      go();
      return;
    }
  }

  ServiceWalker::_reachedPathway();
}

void TaxCollector::_noWay(){  die();  }

void TaxCollector::load(const VariantMap& stream)
{
  ServiceWalker::load( stream );

  VARIANT_LOAD_ANY_D( _d, money, stream )
  VARIANT_LOAD_ANY_D( _d, return2base, stream )
}

void TaxCollector::save(VariantMap& stream) const
{
  ServiceWalker::save( stream );
  VARIANT_SAVE_ANY_D( stream, _d, money )
  VARIANT_SAVE_ANY_D( stream, _d, return2base )
}
