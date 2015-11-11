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

#include "patrician.hpp"
#include "core/variant.hpp"
#include "city/city.hpp"
#include "pathway/pathway_helper.hpp"
#include "core/gettext.hpp"
#include "gfx/tilemap.hpp"
#include "constants.hpp"
#include "objects/construction.hpp"
#include "corpse.hpp"
#include "ability.hpp"
#include "objects/house_spec.hpp"
#include "city/statistic.hpp"
#include "core/priorities.hpp"
#include "objects/house.hpp"
#include "core/variant_map.hpp"
#include "game/resourcegroup.hpp"
#include "name_generator.hpp"
#include "core/logger.hpp"

class Patrician::Impl
{
public:
  TilePos destination;
  HousePtr house;
};

Patrician::Patrician(PlayerCityPtr city )
  : Human( city, walker::patrician ), _d( new Impl )
{
  setName( NameGenerator::rand( NameGenerator::patricianMale ) );
}

Patrician::~Patrician() {}

void Patrician::save( VariantMap& stream ) const
{
  Walker::save( stream );
  VARIANT_SAVE_ANY_D( stream, _d, destination )
  stream[ "house" ] = _d->house.isValid() ? _d->house->pos() : gfx::tilemap::invalidLocation();
}

void Patrician::load( const VariantMap& stream )
{
  Walker::load( stream );
  VARIANT_LOAD_ANY_D( _d, destination, stream )

  TilePos housePos = stream.get( "house" );
  _d->house = _map().overlay<House>( housePos );
}

bool Patrician::_findNewWay( const TilePos& pos )
{
  std::map<int,object::Type> servicesNeed;

  ConstructionList buildings;
  if( !_d->house.isValid() )
  {
    Logger::warning( "WARNING !!! Patrician have no house" );
    return false;
  }
  else
  {
    int srvcValue = _d->house->getServiceValue( Service::senate );
    servicesNeed[ srvcValue ] = object::senate;

    srvcValue = _d->house->getServiceValue( Service::library );
    servicesNeed[ srvcValue ] = object::library;

    srvcValue = _d->house->getServiceValue( Service::forum );
    servicesNeed[ srvcValue ] = object::forum;

    srvcValue = _d->house->getServiceValue( Service::patrician );
    servicesNeed[ srvcValue ] = object::house;

    object::Type type = servicesNeed.begin()->second;
    switch( type )
    {
    case object::house:
    {
      buildings = _city()->statistic().houses
                                      .patricians( true )
                                      .select<Construction>();
    }
    break;
    case object::senate:
    {
      object::TypeSet bTypes;
      bTypes << object::governorHouse << object::governorVilla
             << object::governorPalace << object::senate;
      buildings = _city()->statistic().objects.find<Construction>( bTypes );
    }
    break;

    default:
      buildings = _city()->statistic().objects.find<Construction>( type );
    break;
    }
  }  

  Pathway pathway;
  for( size_t k=0; k < std::min<size_t>( 3, buildings.size() ); k++ )
  {
    ConstructionPtr building = buildings.random();
    pathway = PathwayHelper::create( pos, building, PathwayHelper::roadOnly );

    if( pathway.isValid() )
    {
      _d->destination = building->pos();
      break;
    }
  }

  if( !pathway.isValid() )
  {
    pathway = PathwayHelper::randomWay( _city(), pos, 10 );
  }

  bool wayFound = true;
  if( pathway.isValid() )
  {
    setPos( pos );
    setPathway( pathway );
    go();
    wayFound = true;
  }
  else
  {
    Logger::warning( "WARNING !!! Patrician cant find way" );
    die();
  }

  return wayFound;
}

void Patrician::_reachedPathway()
{
  if( _pathway().isReverse() )
  {
    deleteLater();
  }
  else
  {
    _pathway().toggleDirection();
    go();
  }
}

bool Patrician::die()
{
  return Walker::die();
}

void Patrician::send2City( HousePtr house )
{
  if( !house.isValid() )
  {
    deleteLater();
    Logger::warning( "WARNING !!! Cant start patrician from null house" );
    return;
  }

  _d->house = house;
  if( _findNewWay( house->roadside().locations().random() ) )
  {
    attach();
  }
  else
  {
    Logger::warning( "WARNING !!! Cant start patrician from house [{},{}]", house->pos().i(), house->pos().j() );
    deleteLater();
  }
}
