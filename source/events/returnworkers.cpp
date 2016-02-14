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

#include "returnworkers.hpp"
#include "game/game.hpp"
#include "city/city.hpp"
#include "objects/house.hpp"
#include "gfx/tilemap.hpp"
#include "gfx/tile.hpp"

using namespace gfx;

namespace events
{
const int defaultReturnWorkersDistance = 40;

GameEventPtr ReturnWorkers::create(const TilePos& center, unsigned int workers)
{
  ReturnWorkers* e = new ReturnWorkers();
  e->_center  = center;
  e->_workers = workers;
  GameEventPtr ret( e );
  ret->drop();
  return ret;
}

void ReturnWorkers::_exec(Game& game, unsigned int time)
{
  Tilemap& tilemap = game.city()->tilemap();
  for( int curRange=1; curRange < defaultReturnWorkersDistance; curRange++ )
  {
    HouseList hList = tilemap.rect( curRange, _center ).overlays().select<House>();

    for( auto house : hList )
    {
      int lastWorkersCount = (int)house->unemployed(); //save value, forexample 5 (max 8)
      house->appendServiceValue( Service::recruter, _workers );                //add some people, current value 8
      int delta = (int)house->unemployed();   //check delta 8 - 5 == 3

      int mayAppend = math::clamp<int>( _workers, 0, delta - lastWorkersCount );
      _workers -= mayAppend;

      if( !_workers )
        return;
    }
  }
}

bool ReturnWorkers::_mayExec(Game&, unsigned int) const { return true; }
ReturnWorkers::ReturnWorkers() : _workers( 0 ){}

}
