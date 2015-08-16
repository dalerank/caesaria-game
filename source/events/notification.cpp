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

#include "notification.hpp"
#include "city/cityservice_military.hpp"
#include "world/object.hpp"
#include "game/game.hpp"
#include "city/city.hpp"
#include "objects/construction.hpp"
#include "world/empire.hpp"
#include "world/barbarian.hpp"
#include "world/romechastenerarmy.hpp"
#include "city/statistic.hpp"

using namespace city;

namespace events
{

class Notify::Impl
{
public:
  std::string message;
  std::string cityname;
  std::string object;
  int type;
};

GameEventPtr Notify::attack(const std::string& cityname, const std::string& message, world::ObjectPtr object )
{
  Notify* ev = new Notify();

  ev->_d->cityname = cityname;
  ev->_d->message = message;
  ev->_d->object = object->name();

  if( is_kind_of<world::Barbarian>( object ) )  {  ev->_d->type = Notification::barbarian;  }
  else if( is_kind_of<world::RomeChastenerArmy>( object ) ) { ev->_d->type = Notification::chastener; }
  else { ev->_d->type = Notification::unknown; }

  GameEventPtr evPtr( ev );
  evPtr->drop();

  return evPtr;
}

void Notify::_exec(Game& game, unsigned int)
{
  world::CityPtr pCity = game.empire()->findCity( _d->cityname );

  PlayerCityPtr plrCity = ptr_cast<PlayerCity>( pCity );
  if( plrCity.isValid() )
  {    
    MilitaryPtr mil = plrCity->statistic().services.find<Military>();

    if( mil.isValid() )
    {
      mil->addNotification( _d->message, _d->object, (Notification::Type)_d->type );
    }
  }
}

bool Notify::_mayExec(Game& game, unsigned int time) const { return true; }

Notify::Notify() : _d( new Impl )
{

}

}//end namespace events
