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

#include "removecitizen.hpp"
#include "game/game.hpp"
#include "city/city.hpp"
#include "objects/house.hpp"
#include "gfx/tilemap.hpp"
#include "gfx/tile.hpp"
#include "core/foreach.hpp"

using namespace gfx;

namespace events
{

GameEventPtr RemoveCitizens::create(TilePos center, unsigned int count)
{
  RemoveCitizens* e = new RemoveCitizens();
  e->_center  = center;
  e->_count = count;

  GameEventPtr ret( e );
  ret->drop();
  return ret;
}

void RemoveCitizens::_exec(Game& game, unsigned int time)
{
  Tilemap& tilemap = game.city()->tilemap();
  const int defaultFireWorkersDistance = 40;
  for( int curRange=1; curRange < defaultFireWorkersDistance; curRange++ )
  {
    HouseList hList;
    hList << tilemap.getRectangle( curRange, _center ).overlays();

    foreach( it, hList )
    {
      HousePtr house = *it;
      if( house.isValid() )
      {
        if( house->habitants().count() >= _count )
        {
          house->remHabitants( _count );
          break;
        }
      }
    }
  }
}

bool RemoveCitizens::_mayExec(Game&, unsigned int) const { return true; }

RemoveCitizens::RemoveCitizens() : _count( 0 )
{
}

}
