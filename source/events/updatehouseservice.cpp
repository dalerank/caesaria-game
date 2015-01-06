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
#include "core/priorities.hpp"

using namespace constants;

namespace events
{

namespace {
CAESARIA_LITERALCONST(population)
}

class UpdateHouseService::Impl
{
public:
  int minPopulation, maxPopulation;

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
  int population = game.city()->population();
  if( population > _d->minPopulation && population < _d->maxPopulation )
  {
    Logger::warning( "Execute update house service event" + ServiceHelper::getName( _d->type ) );
    _d->isDeleted = true;

    HouseList houses;
    houses << game.city()->overlays();

    foreach( it, houses  )
    {
      (*it)->appendServiceValue( _d->type, _d->value );
    }
  }
}

bool UpdateHouseService::_mayExec(Game&, unsigned int) const { return true; }
bool UpdateHouseService::isDeleted() const {  return _d->isDeleted; }

void UpdateHouseService::load(const VariantMap& stream)
{
  VariantList vl = stream.get( lc_population ).toList();
  _d->minPopulation = vl.get( 0, 0 ).toInt();
  _d->maxPopulation = vl.get( 1, 999999 ).toInt();
  _d->type = (Service::Type)stream.get( "type" ).toInt();
  VARIANT_LOAD_ANY_D( _d, value, stream );
}

VariantMap UpdateHouseService::save() const
{
  VariantMap ret;
  VariantList vl_pop;
  vl_pop << _d->minPopulation << _d->maxPopulation;

  ret[ lc_population ] = vl_pop;
  ret[ "type" ] = (int)_d->type;
  VARIANT_SAVE_ANY_D( ret, _d, value );

  return ret;
}

UpdateHouseService::UpdateHouseService() : _d( new Impl )
{
  _d->isDeleted = false;
  _d->maxPopulation = _d->minPopulation = 0;
}

}//end namespace events
