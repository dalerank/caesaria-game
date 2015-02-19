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

#include "spirit_of_mars.hpp"
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

class SpiritOfMars::Impl
{
public:
  DateTime endTime;
  bool isDeleted;
};

SrvcPtr SpiritOfMars::create( PlayerCityPtr city, int month )
{
  SpiritOfMars* ptr = new SpiritOfMars( city );
  ptr->_d->endTime = game::Date::current();
  ptr->_d->endTime.appendMonth( month );
  SrvcPtr ret( ptr );
  ret->drop();

  return ret;
}

void SpiritOfMars::timeStep( const unsigned int time)
{
  if( game::Date::isWeekChanged() )
  {
    _d->isDeleted = (_d->endTime < game::Date::current());

    Logger::warning( "SpiritOfMars: execute service" );

    EnemySoldierList enemies;
    enemies << city->walkers( walker::any );

    if( enemies.size() > 0 )
    {
      int step = std::min( enemies.size(), 32 );
      for( int k=0; k < step; k++ )
      {
        EnemySoldierPtr ptr = enemies.random();
        enemies.remove( ptr );
        ptr->die();
      }
      _d->isDeleted = true;
    }
  }
}

std::string SpiritOfMars::defaultName() { return "spirit_of_mars"; }
bool SpiritOfMars::isDeleted() const {  return _d->isDeleted; }

void SpiritOfMars::load(const VariantMap& stream)
{
  VARIANT_LOAD_TIME_D( _d, endTime, stream )
  VARIANT_LOAD_ANY_D( _d, isDeleted, strema )
}

VariantMap SpiritOfMars::save() const
{
  VariantMap ret;
  ret[ "endTime" ] = _d->endTime;
  ret[ "value" ] = _d->value;

  return ret;
}

SpiritOfMars::SpiritOfMars(PlayerCityPtr city )
  : Srvc( city, SpiritOfMars::defaultName() ), _d( new Impl )
{
  _d->isDeleted = false;
}

}//end namespace city
