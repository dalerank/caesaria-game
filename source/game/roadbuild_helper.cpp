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

#include "roadbuild_helper.hpp"

#include <set>
#include <map>

#include "core/stringhelper.hpp"
#include "core/exception.hpp"
#include "core/position.hpp"
#include "gfx/tile.hpp"
#include "core/logger.hpp"

// comparison (for sorting list of tiles by their coordinates)
bool
compare_tiles_(const Tile * first, const Tile * second)
{
  if (first->getI() < second->getI())
    return true;

  else if (first->getI() == second->getI() &&
           first->getJ() > second->getJ())
    return true;

  return false;
}

TilesArray RoadPropagator::createPath(Tilemap& tileMap, TilePos startPos, TilePos stopPos )
{
  TilesArray ret;

  //int mapSize = tileMap.getSize();;
  int iStep = (startPos.getI() < stopPos.getI()) ? 1 : -1;
  int jStep = (startPos.getJ() < stopPos.getJ()) ? 1 : -1;

  std::cout << "RoadPropagator::getPath" << std::endl;

  Logger::warning( "(%d, %d) to (%d, %d)", startPos.getI(), startPos.getJ(), stopPos.getI(), stopPos.getJ() );

  if( startPos == stopPos )
  {
    ret.push_back( &tileMap.at( startPos ) );
    return ret;
  }

  std::cout << "propagate by I axis" << std::endl;

  // propagate on I axis
  for( TilePos tmp( startPos.getI(), stopPos.getJ() ); ; tmp+=TilePos( iStep, 0 ) )
  {
    Tile& curTile = tileMap.at( tmp );

    Logger::warning( "+ (%d, %d)", curTile.getI(), curTile.getJ() );
    ret.push_back( &curTile );

    if (tmp.getI() == stopPos.getI())
      break;
  }

  std::cout << "propagate by J axis" << std::endl;

  // propagate on J axis
  for( int j = startPos.getJ();; j+=jStep )
  {
    Tile& curTile = tileMap.at( startPos.getI(), j );

    std::cout << "+ (" << curTile.getI() << " " << curTile.getJ() << ") ";
    ret.push_back( &curTile );

    if( j == stopPos.getJ() )
      break;
  }

  // sort tiles to be drawn in the rigth order on screen
  ret.sort(compare_tiles_);

  return ret;
}
