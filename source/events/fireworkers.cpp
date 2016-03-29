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

#include "fireworkers.hpp"
#include "game/game.hpp"
#include "city/statistic.hpp"
#include "gfx/tilemap.hpp"
#include "objects/working.hpp"
#include "gfx/tile.hpp"
#include "city/city.hpp"

using namespace gfx;

namespace events
{
static const int defaultReturnWorkersDistance = 40;

GameEventPtr FireWorkers::create(const TilePos& center, unsigned int workers)
{
  FireWorkers* e = new FireWorkers();
  e->_center  = center;
  e->_workers = workers;
  GameEventPtr ret( e );
  ret->drop();
  return ret;
}

bool FireWorkers::_mayExec(Game& game, unsigned int time) const
{
  return true;
}

FireWorkers::FireWorkers() : _workers( 0 ) {}

void FireWorkers::_exec(Game& game, unsigned int)
{
  Tilemap& tilemap = game.city()->tilemap();

  for( int curRange=1; curRange < defaultReturnWorkersDistance; curRange++ )
  {
    TilePos range( curRange, curRange );
    TilesArray perimetr = tilemap.rect( _center - range,
                                                _center + range );
    for( auto& tile : perimetr )
    {
      WorkingBuildingPtr wrkBuilding = tile->overlay<WorkingBuilding>();
      if( wrkBuilding.isValid() )
      {
        int removedFromWb = wrkBuilding->removeWorkers( _workers );
        _workers -= removedFromWb;
      }

      if( !_workers )
        return;
    }
  }

  if( _workers > 0 )
  {
    WorkingBuildingList buildings = game.city()->statistic().objects.find<WorkingBuilding>();
    for( auto building : buildings )
    {
      int removedFromWb = building->removeWorkers( _workers );
      _workers -= removedFromWb;

      if( !_workers )
        return;
    }
  }
}

}// end namesopace events
