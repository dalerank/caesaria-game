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

#include "roadbuild_helper.hpp"

#include <set>
#include <map>

#include "core/stringhelper.hpp"
#include "core/exception.hpp"
#include "core/position.hpp"
#include "pathway/astarpathfinding.hpp"
#include "gfx/tile.hpp"
#include "objects/construction.hpp"
#include "pathway/pathway.hpp"
#include "pathway/pathway_helper.hpp"
#include "core/logger.hpp"

using namespace gfx;

TilesArray RoadPropagator::createPath(Tilemap& tileMap, TilePos startPos, TilePos stopPos,
                                      bool roadAssignment, bool returnRect )
{  
  Logger::warning( "RoadPropagator::getPath from (%d, %d) to (%d, %d)",
                    startPos.i(), startPos.j(), stopPos.i(), stopPos.j() );

  TilesArray ret;
  if( startPos == stopPos )
  {
    ret.push_back( &tileMap.at( startPos ) );
    return ret;
  }

  if( returnRect )
  {
    ret = tileMap.getRectangle( startPos, stopPos );
  }
  else
  {
    bool yMoveFirst = stopPos.i() > startPos.i();

    //int mapSize = tileMap.getSize();;
    TilePos midlPos;
    midlPos = yMoveFirst
                ? TilePos( startPos.i(), stopPos.j() )
                : TilePos( startPos.i(), stopPos.j() );

    if( yMoveFirst )
    {
      ret.append( tileMap.getRectangle( startPos, midlPos ) );
      ret.append( tileMap.getRectangle( midlPos, stopPos ) );
    }
    else
    {
      ret.append( tileMap.getRectangle( stopPos, midlPos ) );
      ret.append( tileMap.getRectangle( midlPos, startPos ) );
    }

    foreach( it, ret )
    {
      if( !(*it)->isWalkable( true ) )
      {
        ret.clear();
        break;
      }
    }
  }

  if( ret.empty() )
  {
    int flags = Pathfinder::fourDirection | Pathfinder::terrainOnly;
    flags |= (roadAssignment ? 0 : Pathfinder::ignoreRoad );
    const Tile& stile = tileMap.at( startPos );
    const Tile& ftile = tileMap.at( stopPos );
    Pathway way = Pathfinder::instance().getPath( stile.pos(), ftile.pos(), flags );

    ret = way.allTiles();
  }

  return ret;
}
