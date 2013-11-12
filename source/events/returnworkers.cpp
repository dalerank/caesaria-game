// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.

#include "returnworkers.hpp"
#include "game/game.hpp"
#include "game/city.hpp"
#include "building/house.hpp"
#include "game/tilemap.hpp"

namespace events
{

GameEventPtr ReturnWorkers::create(TilePos center, unsigned int workers)
{
  ReturnWorkers* e = new ReturnWorkers();
  e->_center  = center;
  e->_workers = workers;
  GameEventPtr ret( e );
  ret->drop();
  return ret;
}

void ReturnWorkers::exec(Game& game)
{
  Tilemap& tilemap = game.getCity()->getTilemap();
  const int defaultFireWorkersDistance = 40;
  for( int curRange=1; curRange < defaultFireWorkersDistance; curRange++ )
  {
    TilemapArea perimetr = tilemap.getRectangle( _center - TilePos( curRange, curRange ),
                                                 _center + TilePos( curRange, curRange ) );
    foreach( Tile* tile, perimetr )
    {
      HousePtr house = tile->getOverlay().as<House>();
      if( house.isValid() )
      {
        int lastWorkersCount = house->getServiceValue( Service::workersRecruter );
        house->appendServiceValue( Service::workersRecruter, _workers );
        int currentWorkers = house->getServiceValue( Service::workersRecruter );

        int mayAppend = math::clamp<int>( _workers, 0, currentWorkers - lastWorkersCount );
        _workers -= mayAppend;
      }

      if( !_workers )
        return;
    }
  }
}

}
