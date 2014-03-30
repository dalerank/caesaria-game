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

// comparison (for sorting list of tiles by their coordinates)
bool
compare_tiles_(const Tile * first, const Tile * second)
{
  if (first->i() < second->i())
    return true;

  else if (first->i() == second->i() &&
           first->j() > second->j())
    return true;

  return false;
}

TilesArray RoadPropagator::createPath(Tilemap& tileMap, TilePos startPos, TilePos stopPos , bool roadAssignment)
{  
  int flags = Pathfinder::fourDirection | Pathfinder::terrainOnly;
  flags |= (roadAssignment ? 0 : Pathfinder::ignoreRoad );
  Pathway way = Pathfinder::getInstance().getPath( startPos, stopPos, flags );

  if( way.isValid() )
  {
    return way.allTiles();
  }

  TilesArray ret;

  //int mapSize = tileMap.getSize();;
  int iStep = (startPos.i() < stopPos.i()) ? 1 : -1;
  int jStep = (startPos.j() < stopPos.j()) ? 1 : -1;

  std::cout << "RoadPropagator::getPath" << std::endl;

  Logger::warning( "(%d, %d) to (%d, %d)", startPos.i(), startPos.j(), stopPos.i(), stopPos.j() );

  if( startPos == stopPos )
  {
    ret.push_back( &tileMap.at( startPos ) );
    return ret;
  }

  std::cout << "propagate by I axis" << std::endl;

  // propagate on I axis
  for( TilePos tmp( startPos.i(), stopPos.j() ); ; tmp+=TilePos( iStep, 0 ) )
  {
    Tile& curTile = tileMap.at( tmp );

    Logger::warning( "+ (%d, %d)", curTile.i(), curTile.j() );
    ret.push_back( &curTile );

    if (tmp.i() == stopPos.i())
      break;
  }

  std::cout << "propagate by J axis" << std::endl;

  // propagate on J axis
  for( int j = startPos.j();; j+=jStep )
  {
    Tile& curTile = tileMap.at( startPos.i(), j );

    std::cout << "+ (" << curTile.i() << " " << curTile.j() << ") ";
    ret.push_back( &curTile );

    if( j == stopPos.j() )
      break;
  }

  // sort tiles to be drawn in the rigth order on screen
  std::sort( ret.begin(), ret.end(), compare_tiles_ );

  return ret;
}
