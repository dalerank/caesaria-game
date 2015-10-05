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

#include "contaminated_water.hpp"
#include "game/game.hpp"
#include "city/statistic.hpp"
#include "game/gamedate.hpp"
#include "objects/house.hpp"
#include "events/dispatcher.hpp"
#include "core/logger.hpp"
#include "core/variant_map.hpp"
#include "objects/well.hpp"
#include "factory.hpp"

namespace events
{

REGISTER_EVENT_IN_FACTORY(ContaminatedWater, "contaminated_water")

class ContaminatedWater::Impl
{
public:
  bool isDeleted;
  DateTime endDate;
  int value;
};

GameEventPtr ContaminatedWater::create()
{
  GameEventPtr ret( new ContaminatedWater() );
  ret->drop();

  return ret;
}

void _decreaseHousesHealth( object::Type btype, PlayerCityPtr city, int value )
{
  TilePos offset( 2, 2 );

  const OverlayList& buildings = city->overlays();

  for( auto building : buildings )
  {
    if( building->type() != btype )
        continue;

    HouseList houses = city->statistic().objects.find<House>( object::house, building->pos() - offset, building->pos() + offset );

    for( auto house : houses )
    {
      house->updateState( pr::health, value );
    }
  }
}

void ContaminatedWater::_exec( Game& game, unsigned int time)
{
  if( game::Date::isWeekChanged() )
  {
    Logger::warning( "Execute contaminated water service" );
    _d->isDeleted = _d->endDate < game::Date::current();

    _decreaseHousesHealth( object::well, game.city(), -_d->value );
    _decreaseHousesHealth( object::fountain, game.city(), -_d->value );
  }
}

bool ContaminatedWater::_mayExec(Game&, unsigned int) const { return true; }
bool ContaminatedWater::isDeleted() const {  return _d->isDeleted; }

void ContaminatedWater::load(const VariantMap& stream)
{
  GameEvent::load( stream );
  VARIANT_LOAD_ANYDEF_D( _d, value, 10, stream )
  VARIANT_LOAD_TIME_D( _d, endDate, stream )
}

VariantMap ContaminatedWater::save() const
{
  VariantMap ret = GameEvent::save();
  VARIANT_SAVE_ANY_D( ret, _d, value )
  VARIANT_SAVE_ANY_D( ret, _d, endDate )

  return ret;
}

ContaminatedWater::ContaminatedWater() : _d( new Impl )
{
  _d->isDeleted = false;
}

}//end namespace events
