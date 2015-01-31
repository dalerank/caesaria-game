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

#include "start_work.hpp"
#include "game/game.hpp"
#include "city/helper.hpp"
#include "game/gamedate.hpp"
#include "scene/level.hpp"
#include "objects/colosseum.hpp"
#include "postpone.hpp"
#include "factory.hpp"

using namespace constants;

namespace events
{

REGISTER_EVENT_IN_FACTORY(StartWork,"start_work")

GameEventPtr StartWork::create()
{
  GameEventPtr ret( new StartWork() );
  ret->drop();

  return ret;
}

void StartWork::load(const VariantMap& stream)
{
  GameEvent::load( stream );
  _options = stream.get( "action" ).toMap();
  _noTroubles = stream.get( "no_troubles", false );
}

VariantMap StartWork::save() const
{
  VariantMap ret = GameEvent::save();

  ret[ "action" ] = _options;
  ret[ "no_troubles" ] = _noTroubles;

  return ret;
}

bool StartWork::isDeleted() const {  return _isDeleted; }

void StartWork::_exec(Game& game, unsigned int)
{
  foreach( i, _options )
  {
    GameEventPtr e = PostponeEvent::create( i->first, i->second.toMap() );
    e->dispatch();
  }
}

bool StartWork::_mayExec(Game& game, unsigned int ) const
{
  if( game::Date::isWeekChanged() )
  {
    city::Helper helper( game.city() );

    bool ret = false;

    foreach( i, _bldTypes )
    {
      WorkingBuildingList bld = helper.find<WorkingBuilding>( gfx::TileOverlay::Type(*i) );

      ret = !bld.empty();

      if( ret && _noTroubles )
      {
        ret &= bld.front()->troubleDesc().empty();
      }
    }

    return ret;
  }

  return false;
}

StartWork::StartWork()
{
  _isDeleted = false;
}

}
