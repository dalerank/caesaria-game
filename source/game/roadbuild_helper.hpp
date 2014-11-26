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


#ifndef __CAESARIA_ROADPROPAGATOR_H_INCLUDE_
#define __CAESARIA_ROADPROPAGATOR_H_INCLUDE_

#include "core/scopedptr.hpp"
#include "gfx/tilemap.hpp"

class RoadPropagator
{
public:
  static RoadPropagator& instance();

  /** finds the shortest path between origin and destination
  * returns True if a path exists
  * the path is returned in oPathWay
  */
  static gfx::TilesArray createPath(gfx::Tilemap& tileMap,
                                TilePos startTile, TilePos destination,
                                bool roadAssignment=false, bool returnRect=false);

  void canBuildRoad(const gfx::Tile* tile, bool& ret);

private:
  RoadPropagator();
};

#endif //__CAESARIA_ROADPROPAGATOR_H_INCLUDE_
