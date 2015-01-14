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

#include "event.hpp"
#include "dispatcher.hpp"
#include "core/utils.hpp"
#include "game/game.hpp"
#include "city/city.hpp"
#include "gfx/tilemap.hpp"
#include "warningmessage.hpp"
#include "core/gettext.hpp"
#include "game/resourcegroup.hpp"
#include "gui/environment.hpp"
#include "gui/label.hpp"
#include "core/variant_map.hpp"
#include "gfx/helper.hpp"
#include "requestdestroy.hpp"

using namespace constants;
using namespace gfx;

namespace events
{

namespace {
CAESARIA_LITERALCONST(type)
CAESARIA_LITERALCONST(name)
}

bool GameEvent::tryExec(Game& game, unsigned int time)
{
  if( _mayExec( game, time ) )
  {
    _exec( game, time );
    return true;
  }

  return false;
}

bool GameEvent::isDeleted() const { return true; }
void events::GameEvent::dispatch() { Dispatcher::instance().append( this );}

VariantMap GameEvent::save() const
{
  VariantMap ret;
  ret[ lc_type ] = Variant( _type );
  ret[ lc_name ] = Variant( _name );
  return ret;
}

void GameEvent::load(const VariantMap& stream)
{
  _type = stream.get( lc_type, Variant( _type ) ).toString();
  _name = stream.get( lc_name, Variant( _name ) ).toString();
}

} //end namespace events
