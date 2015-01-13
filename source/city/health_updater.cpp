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

#include "health_updater.hpp"
#include "game/game.hpp"
#include "objects/construction.hpp"
#include "helper.hpp"
#include "city.hpp"
#include "core/variant_map.hpp"
#include "game/gamedate.hpp"
#include "objects/house.hpp"
#include "core/logger.hpp"
#include "events/dispatcher.hpp"

using namespace constants;

namespace city
{

class HealthUpdater::Impl
{
public:
  DateTime endTime;
  bool isDeleted;
  int value;
};

SrvcPtr HealthUpdater::create( PlayerCityPtr city )
{
  SrvcPtr ret( new HealthUpdater( city ) );
  ret->drop();

  return ret;
}

void HealthUpdater::timeStep( const unsigned int time)
{
  if( game::Date::isMonthChanged() )
  {
    _d->isDeleted = (_d->endTime < game::Date::current());

    Logger::warning( "HealthUpdater: execute service" );
    Helper helper( _city() );
    HouseList houses = helper.find<House>( objects::house );

    foreach( it, houses )
    {
      (*it)->updateState( House::health, _d->value );
    }
  }
}

std::string HealthUpdater::defaultName() { return "health_updater"; }
bool HealthUpdater::isDeleted() const {  return _d->isDeleted; }

void HealthUpdater::load(const VariantMap& stream)
{
  _d->endTime = stream.get( "endTime" ).toDateTime();
  _d->value = stream.get( "value" );
}

VariantMap HealthUpdater::save() const
{
  VariantMap ret;
  ret[ "endTime" ] = _d->endTime;
  ret[ "value" ] = _d->value;

  return ret;
}

HealthUpdater::HealthUpdater(PlayerCityPtr city )
  : Srvc( city, HealthUpdater::defaultName() ), _d( new Impl )
{
  _d->isDeleted = false;
}

}//end namespace city
