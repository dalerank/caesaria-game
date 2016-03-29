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

#include "updatehouseservice.hpp"
#include "game/game.hpp"
#include "city/city.hpp"
#include "game/gamedate.hpp"
#include "objects/road.hpp"
#include "objects/house.hpp"
#include "core/variant_map.hpp"
#include "events/dispatcher.hpp"
#include "core/logger.hpp"
#include "city/statistic.hpp"
#include "city/states.hpp"
#include "core/priorities.hpp"

using namespace city;

namespace events
{

class UpdateHouseService::Impl
{
public:
  Ranged::Range popRange;

  bool isDeleted;
  Service::Type type;
  int value;
};

GameEventPtr UpdateHouseService::create( Service::Type type, int value )
{
  UpdateHouseService* e = new UpdateHouseService();
  e->_d->value = value;
  e->_d->type = type;

  GameEventPtr ret( e );
  ret->drop();

  return ret;
}

void UpdateHouseService::_exec( Game& game, unsigned int time )
{
  int population = game.city()->states().population;
  if( _d->popRange.contain( population ) )
  {
    Logger::info( "Execute update house service event" + ServiceHelper::getName( _d->type ) );
    _d->isDeleted = true;

    HouseList houses = game.city()->statistic().houses.find();

    for( auto house : houses  )
      house->appendServiceValue( _d->type, (float)_d->value );
  }
}

bool UpdateHouseService::_mayExec(Game&, unsigned int) const { return true; }
bool UpdateHouseService::isDeleted() const {  return _d->isDeleted; }

void UpdateHouseService::load(const VariantMap& stream)
{
  Ranged::load( stream );
  VARIANT_LOAD_ENUM_D( _d, type, stream )
  VARIANT_LOAD_ANY_D( _d, value, stream )
}

VariantMap UpdateHouseService::save() const
{
  VariantMap ret = Ranged::save();
  VARIANT_SAVE_ENUM_D( ret, _d, type )
  VARIANT_SAVE_ANY_D( ret, _d, value )

  return ret;
}

UpdateHouseService::UpdateHouseService() : _d( new Impl )
{
  _d->isDeleted = false;
  _d->popRange.max = _d->popRange.min = 0;
}

}//end namespace events
