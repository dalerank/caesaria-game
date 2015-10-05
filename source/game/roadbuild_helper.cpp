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

#include "core/utils.hpp"
#include "core/exception.hpp"
#include "core/position.hpp"
#include "pathway/astarpathfinding.hpp"
#include "gfx/tile.hpp"
#include "objects/construction.hpp"
#include "pathway/pathway.hpp"
#include "pathway/pathway_helper.hpp"
#include "objects/aqueduct.hpp"
#include "core/logger.hpp"
#include "city/city.hpp"
#include "gfx/tilemap.hpp"

using namespace gfx;

void RoadPropagator::canBuildRoad(const gfx::Tile* tile, bool& ret)
{
  ret = false;
  if( tile->getFlag( Tile::isConstructible ) || tile->getFlag( Tile::tlRoad ) )
  {
    ret = true;
  }
  else
  {
    AqueductPtr aq = tile->overlay<Aqueduct>();
    if( aq.isValid() )
    {     
      ret = aq->canAddRoad( PlayerCityPtr(), tile->pos() );
    }
  }
}

bool __checkWalkables( const TilesArray& tiles )
{
  for( auto tile : tiles )
  {
    if( !tile->isWalkable( true ) || tile->overlay().is<Building>() )
      return false;
  }

  return true;
}

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
    ret = tileMap.rect( startPos, stopPos );
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
      ret.append( tileMap.rect( startPos, midlPos ) );
      ret.append( tileMap.rect( midlPos, stopPos ) );
    }
    else
    {
      ret.append( tileMap.rect( stopPos, midlPos ) );
      ret.append( tileMap.rect( midlPos, startPos ) );
    }

    if( !__checkWalkables( ret ) )
      ret.clear();
  }

  if( ret.empty() )
  {
    Pathfinder& finder = Pathfinder::instance();
    finder.setCondition( makeDelegate( &RoadPropagator::instance(), &RoadPropagator::canBuildRoad ) );

    int flags = Pathfinder::fourDirection | Pathfinder::terrainOnly | Pathfinder::customCondition;

    flags |= (roadAssignment ? 0 : Pathfinder::ignoreRoad );
    const Tile& stile = tileMap.at( startPos );
    const Tile& ftile = tileMap.at( stopPos );
    Pathway way = finder.getPath( stile.pos(), ftile.pos(), flags );

    ret = way.allTiles();
  }

  return ret;
}
