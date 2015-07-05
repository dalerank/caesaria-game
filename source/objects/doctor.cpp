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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#include "doctor.hpp"
#include "game/resourcegroup.hpp"
#include "game/gamedate.hpp"
#include "core/position.hpp"
#include "gfx/tilemap.hpp"
#include "city/statistic.hpp"
#include "constants.hpp"
#include "core/variant_list.hpp"
#include "walker/serviceman.hpp"
#include "house.hpp"
#include "objects_factory.hpp"

using namespace gfx;

REGISTER_CLASS_IN_OVERLAYFACTORY(object::clinic, Doctor)

class ServedHouses : public std::map<int, int>
{
public:
  VariantList save() const
  {
    VariantList ret;

    foreach( it, *this )
      ret << it->first << it->second;

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

class Doctor::Impl
{
public:
  unsigned int patientsNumber;
  ServedHouses servedHouses;
  DateTime lastDateResults;
};

Doctor::Doctor()
  : HealthBuilding(Service::doctor, object::clinic, Size(1)),
    _d( new Impl )
{
  _d->patientsNumber = 0;
}

void Doctor::buildingsServed(const std::set<BuildingPtr>& buildings, ServiceWalkerPtr walker)
{
  foreach( it, buildings )
  {
    if( (*it)->type() == object::house )
    {
      HousePtr house = it->as<House>();
      TilePos pos = house->pos();
      int hash = (pos.i() << 8) | pos.i();
      _d->servedHouses[ hash ] = house->habitants().count();
    }
  }

  HealthBuilding::buildingsServed( buildings, walker );
}

unsigned int Doctor::walkerDistance() const{ return 26; }

void Doctor::deliverService()
{
  if( _d->lastDateResults.monthsTo( game::Date::current() ) > 0 )
  {
    _d->lastDateResults = game::Date::current();
    _d->patientsNumber = 0;
    foreach( it, _d->servedHouses )
      _d->patientsNumber += it->second;

    _d->servedHouses.clear();
  }

  if( numberWorkers() > 0 && walkers().size() == 0 )
  {
    ServiceBuilding::deliverService();
  }
}

void Doctor::save(VariantMap& stream) const
{
  HealthBuilding::save( stream );
  VARIANT_SAVE_ANY_D( stream, _d, patientsNumber )
  VARIANT_SAVE_ANY_D( stream, _d, lastDateResults )
  VARIANT_SAVE_CLASS_D( stream, _d, servedHouses )
}

void Doctor::load(const VariantMap &stream)
{
  HealthBuilding::load( stream );
  VARIANT_LOAD_ANY_D( _d, patientsNumber, stream )
  VARIANT_LOAD_TIME_D( _d, lastDateResults, stream )
  VARIANT_LOAD_CLASS_D_LIST( _d, servedHouses, stream )
}

unsigned int Doctor::patientsNumber() const
{
  return _d->patientsNumber;
}
