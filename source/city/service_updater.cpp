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
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

#include "service_updater.hpp"
#include "game/game.hpp"
#include "objects/construction.hpp"
#include "helper.hpp"
#include "city.hpp"
#include "core/variant_map.hpp"
#include "game/gamedate.hpp"
#include "objects/house.hpp"
#include "game/service.hpp"
#include "core/logger.hpp"
#include "events/dispatcher.hpp"
#include "objects/house_level.hpp"

using namespace constants;

namespace city
{

namespace {
CAESARIA_LITERALCONST(endTime)
CAESARIA_LITERALCONST(value)
CAESARIA_LITERALCONST(service)
}

class ServiceUpdater::Impl
{
public:
  DateTime endTime;
  bool isDeleted;
  Service::Type stype;
  int value;
};

SrvcPtr ServiceUpdater::create( PlayerCityPtr city )
{
  SrvcPtr ret( new ServiceUpdater( city ) );
  ret->drop();

  return ret;
}

void ServiceUpdater::timeStep( const unsigned int time)
{
  if( game::Date::isWeekChanged() )
  {
    _d->isDeleted = (_d->endTime < game::Date::current());

    Logger::warning( "ServiceUpdater: execute service" );
    Helper helper( _city() );
    HouseList houses = helper.find<House>( objects::house );

    foreach( it, houses )
    {
      (*it)->setServiceValue( _d->stype, _d->value );
    }
  }
}

std::string ServiceUpdater::defaultName() { return "service_updater"; }
bool ServiceUpdater::isDeleted() const {  return _d->isDeleted; }

void ServiceUpdater::load(const VariantMap& stream)
{
  _d->endTime = stream.get( lc_endTime ).toDateTime();
  _d->value = stream.get( lc_value );
  _d->stype = (Service::Type)ServiceHelper::getType( stream.get( lc_service ).toString() );
}

VariantMap ServiceUpdater::save() const
{
  VariantMap ret;
  ret[ lc_endTime ] = _d->endTime;
  ret[ lc_value   ] = _d->value;
  ret[ lc_service    ] = Variant( ServiceHelper::getName( _d->stype ) );

  return ret;
}

ServiceUpdater::ServiceUpdater( PlayerCityPtr city )
  : Srvc( city, ServiceUpdater::defaultName() ), _d( new Impl )
{
  _d->isDeleted = false;
  _d->stype = Service::srvCount;
}

}//end namespace city
