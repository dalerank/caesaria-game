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

#include "showtileinfo.hpp"
#include "game/game.hpp"
#include "city/statistic.hpp"
#include "script_event.hpp"
#include "objects/construction.hpp"
#include "movecamera.hpp"

namespace events
{

GameEventPtr ShowTileInfo::create(TilePos pos, Mode mode)
{
  ShowTileInfo* e = new ShowTileInfo();
  e->_pos = pos;
  e->_mode = mode;

  GameEventPtr ret( e );
  ret->drop();

  return ret;
}

void ShowTileInfo::_exec(Game& game, unsigned int time)
{
  switch( _mode )
  {
  case current:
  break;

  case next:
  case prew:
  {
    ConstructionPtr c =  game.city()->getOverlay( _pos ).as<Construction>();
    c = (_mode == next
                ? game.city()->statistic().objects.next( c )
                : game.city()->statistic().objects.prew( c ));

    if( c.isValid() )
    {
      _pos = c->pos();

      events::dispatch<MoveCamera>( _pos );
    }
  }
  break;

  default:
  break;
  }

  VariantList vl; vl << _pos;
  events::dispatch<events::ScriptFunc>("OnShowOverlayInfobox", vl);
}

bool ShowTileInfo::_mayExec(Game&, unsigned int ) const { return true; }

ShowTileInfo::ShowTileInfo() : _mode( count )
{

}

}
