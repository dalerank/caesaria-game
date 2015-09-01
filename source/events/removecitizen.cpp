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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#include "removecitizen.hpp"
#include "game/game.hpp"
#include "city/statistic.hpp"
#include "objects/house.hpp"
#include "gfx/tilemap.hpp"
#include "gfx/tile.hpp"

using namespace gfx;

namespace events
{
const int defaultReturnWorkersDistance = 40;

GameEventPtr RemoveCitizens::create(TilePos center, const CitizenGroup& group)
{
  RemoveCitizens* e = new RemoveCitizens();
  e->_center  = center;
  e->_group = group;

  GameEventPtr ret( e );
  ret->drop();
  return ret;
}

void RemoveCitizens::_exec(Game& game, unsigned int time)
{
  Tilemap& tilemap = game.city()->tilemap();
  for( int curRange=1; curRange < defaultReturnWorkersDistance; curRange++ )
  {
    HouseList hList = tilemap.getRectangle( curRange, _center ).overlays().select<House>();

    for( auto house : hList )
    {
      if( house.isValid() )
      {
        house->removeHabitants( _group );
        if( _group.empty() )
          break;
      }
    }
  }
}

bool RemoveCitizens::_mayExec(Game&, unsigned int) const { return true; }

RemoveCitizens::RemoveCitizens() {}

}
