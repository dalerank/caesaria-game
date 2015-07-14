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
#include "statistic.hpp"
#include "city.hpp"
#include "core/variant_map.hpp"
#include "game/gamedate.hpp"
#include "objects/house.hpp"
#include "core/logger.hpp"
#include "events/dispatcher.hpp"
#include "cityservice_factory.hpp"

namespace city
{

REGISTER_SERVICE_IN_FACTORY(HealthUpdater,health_updater)

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
    HouseList houses = city::statistic::getHouses( _city() );

    for( auto house : houses )
      house->updateState( pr::health, _d->value );
  }
}

std::string HealthUpdater::defaultName() { return "health_updater"; }
bool HealthUpdater::isDeleted() const {  return _d->isDeleted; }

void HealthUpdater::load(const VariantMap& stream)
{
  VARIANT_LOAD_TIME_D( _d, endTime, stream )
  VARIANT_LOAD_ANY_D( _d, value, stream )
}

VariantMap HealthUpdater::save() const
{
  VariantMap ret;
  VARIANT_SAVE_ANY_D( ret, _d, endTime )
  VARIANT_SAVE_ANY_D( ret, _d, value )

  return ret;
}

HealthUpdater::HealthUpdater(PlayerCityPtr city )
  : Srvc( city, HealthUpdater::defaultName() ), _d( new Impl )
{
  _d->isDeleted = false;
}

}//end namespace city
