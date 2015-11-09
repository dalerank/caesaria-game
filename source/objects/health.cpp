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

#include "health.hpp"
#include "game/resourcegroup.hpp"
#include "game/gamedate.hpp"
#include "core/position.hpp"
#include "gfx/tilemap.hpp"
#include "house.hpp"
#include "city/statistic.hpp"
#include "walker/serviceman.hpp"
#include "core/variant_list.hpp"
#include "constants.hpp"
#include "objects_factory.hpp"

using namespace gfx;

class ServedHouses : public std::map<int, int>
{
public:
  VariantList save() const
  {
    VariantList ret;

    for( auto& it : *this )
      ret << it.first << it.second;

    return ret;
  }

  void load( const VariantList& stream )
  {
    VariantListReader r( stream );

    while( !r.atEnd() )
    {
      int type = r.next();
      (*this)[ type ] = r.next();
    }
  }
};

class HealthBuilding::Impl
{
public:
  struct {
    unsigned int current=0;
    unsigned int served=0;
    unsigned int max=0;
  } patients;
  ServedHouses servedHouses;
  DateTime lastDateResults;
};

HealthBuilding::HealthBuilding(const Service::Type service, const object::Type type, const Size &size)
  : ServiceBuilding( service, type, size ), _d( new Impl )
{

}

void HealthBuilding::buildingsServed(const std::set<BuildingPtr>& buildings, ServiceWalkerPtr walker)
{  
  for( auto building : buildings )
  {
    if( building->type() == object::house )
    {
      HousePtr house = building.as<House>();
      int hash = gfx::tile::hash( house->pos() );
      _d->servedHouses[ hash ] = house->habitants().count();
    }
  }

  _d->patients.served = 0;
  for( auto item : _d->servedHouses )
    _d->patients.served += item.second;

  ServiceBuilding::buildingsServed( buildings, walker );
}

unsigned int HealthBuilding::walkerDistance() const { return 26; }

void HealthBuilding::deliverService()
{
  if( _d->lastDateResults.monthsTo( game::Date::current() ) > 0 )
  {
    _d->lastDateResults = game::Date::current();
    _d->patients.current = _d->patients.served;
    _d->patients.served = 0;
    _d->servedHouses.clear();
  }

  bool haveWorkers = numberWorkers() > 0;
  bool inPatrol = walkers().empty();
  bool servedMaxPeople = _d->patients.max == 0
                          ? false
                          : _d->patients.served >= _d->patients.max;

  if( haveWorkers && !inPatrol && !servedMaxPeople )
  {
    ServiceBuilding::deliverService();
  }
}

void HealthBuilding::save(VariantMap& stream) const
{
  ServiceBuilding::save( stream );
  VARIANT_SAVE_ANY_D( stream, _d, patients.current )
  VARIANT_SAVE_ANY_D( stream, _d, patients.max )
  VARIANT_SAVE_ANY_D( stream, _d, patients.served )
  VARIANT_SAVE_ANY_D( stream, _d, lastDateResults )
  VARIANT_SAVE_CLASS_D( stream, _d, servedHouses )
}

void HealthBuilding::load(const VariantMap &stream)
{
  ServiceBuilding::load( stream );
  VARIANT_LOAD_ANY_D( _d, patients.current, stream )
  VARIANT_LOAD_ANY_D( _d, patients.max, stream )
  VARIANT_LOAD_ANY_D( _d, patients.served, stream )
  VARIANT_LOAD_TIME_D( _d, lastDateResults, stream )
  VARIANT_LOAD_CLASS_D_LIST( _d, servedHouses, stream )
}

void HealthBuilding::initialize(const object::Info& mdata)
{
  ServiceBuilding::initialize( mdata );
  _d->patients.max = mdata.getOption( "patients.max" );
}

HealthBuilding::~HealthBuilding() {}

unsigned int HealthBuilding::patientsMax() const { return _d->patients.max; }
unsigned int HealthBuilding::patientsCurrent() const { return _d->patients.current; }
