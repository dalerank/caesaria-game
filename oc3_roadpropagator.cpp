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

#include "oc3_roadpropagator.hpp"

#include <set>
#include <map>

#include "oc3_stringhelper.hpp"
#include "oc3_exception.hpp"
#include "oc3_tilemap.hpp"
#include "oc3_positioni.hpp"
#include "oc3_tile.hpp"

class RoadPropagator::Impl
{
public:
    const Tilemap& tilemap;
    const Tile& startTile;

    int mapSize;

    Impl( const Tilemap& tmap, const Tile& start ) : tilemap(tmap), startTile(start)
    {

    }
};

RoadPropagator::RoadPropagator( const Tilemap& tileMap, const Tile& startTile )
    : _d( new Impl( tileMap, startTile ) )
{
    _d->mapSize = tileMap.getSize();
}

bool RoadPropagator::getPath( const Tile& destination, ConstWayOnTiles& oPathWay ) const
{
  TilePos startPos = _d->startTile.getIJ();
  TilePos stopPos  = destination.getIJ();
  int iStep = (startPos.getI() < stopPos.getI()) ? 1 : -1;
  int jStep = (startPos.getJ() < stopPos.getJ()) ? 1 : -1;

  std::cout << "RoadPropagator::getPath" << std::endl;

  StringHelper::debug( 0xff, "(%d, %d) to (%d, %d)", startPos.getI(), startPos.getJ(), stopPos.getI(), stopPos.getJ() );

  if( startPos == stopPos )
  {
    oPathWay.push_back( &_d->startTile );
    return true;
  }

  std::cout << "propagate by I axis" << std::endl;

  // propagate on I axis
  for( TilePos tmp( startPos.getI(), stopPos.getJ() ); ; tmp+=TilePos( iStep, 0 ) )
  {
    const Tile& curTile = _d->tilemap.at( tmp );

    // if (curTile.getTerrain().isConstructible() ||
    //     curTile.getTerrain().isRoad()//           ||
    //     // curTile.getTerrain().isAqueduct()
    //     )
    // {
      StringHelper::debug( 0xff, "+ (%d, %d)", curTile.getI(), curTile.getJ() );
      oPathWay.push_back( &curTile );
      //    }
    // else
    //   return false;

    if (tmp.getI() == stopPos.getI())
      break;
  }

  std::cout << "propagate by J axis" << std::endl;

  // propagate on J axis
  for( int j = startPos.getJ();; j+=jStep )
  {
    const Tile& curTile = _d->tilemap.at( startPos.getI(), j );

    // if (curTile.getTerrain().isConstructible() ||
    //     curTile.getTerrain().isRoad()//           ||
    //     // curTile.getTerrain().isAqueduct()
    //     )
    // {
      std::cout << "+ (" << curTile.getI() << " " << curTile.getJ() << ") ";
      oPathWay.push_back( &curTile );
      //    }
    // else
    //   return false;

    if( j == stopPos.getJ() )
      break;
  }

  return true;
}

RoadPropagator::~RoadPropagator()
{

}
