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
  ConstWayOnTiles tmpPathWay;

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

    StringHelper::debug( 0xff, "+ (%d, %d)", curTile.getI(), curTile.getJ() );
    tmpPathWay.push_back( &curTile );

    if (tmp.getI() == stopPos.getI())
      break;
  }

  std::cout << "propagate by J axis" << std::endl;

  // propagate on J axis
  for( int j = startPos.getJ();; j+=jStep )
  {
    const Tile& curTile = _d->tilemap.at( startPos.getI(), j );

    std::cout << "+ (" << curTile.getI() << " " << curTile.getJ() << ") ";
    tmpPathWay.push_back( &curTile );

    if( j == stopPos.getJ() )
      break;
  }

  // sort tiles to be drawn in the rigth order on screen
  while (!tmpPathWay.empty()) {
    ConstWayOnTiles::iterator it = tmpPathWay.begin();
    ConstWayOnTiles::iterator it_next = it;

    for (++it; it != tmpPathWay.end(); ++it) {
      // if got the lowest X, then take it
      if ((*it)->getI() < (*it_next)->getI())
        it_next = it;
      // if X is the same but Y is bigger, then take it
      else if ((*it)->getI() == (*it_next)->getI() &&
               (*it)->getJ() > (*it_next)->getJ())
        it_next = it;
    }

    tmpPathWay.erase(it_next);
    oPathWay.push_back(*it_next);
  }

  return true;
}

RoadPropagator::~RoadPropagator()
{

}
