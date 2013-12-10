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

#include "astarpathfinding.hpp"
#include "gfx/tilemap.hpp"
#include "core/position.hpp"
#include "astarpoint.hpp"
#include "path_finding.hpp"
#include "core/stringhelper.hpp"
#include "core/foreach.hpp"
#include "core/logger.hpp"
#include <set>

using namespace std;

namespace {
static const AStarPoint invalidPoint;
typedef std::vector< AStarPoint* > APoints;
}

class Pathfinder::Impl
{
public:  
  TilePossibleCondition condition;

  class Grid : public std::vector< APoints >
  {
  public:
    AStarPoint* operator[](const TilePos& pos )
    {
      return at( pos.getI() ).at( pos.getJ() );
    }

    APoints& operator[]( unsigned int row )
    {
      return at( row );
    }
  };

  Grid grid;
  Tilemap* tilemap;
  unsigned int maxLoopCount;

  bool getTraversingPoints(TilePos start, TilePos stop, Pathway& oPathWay );

  AStarPoint* at( const TilePos& pos )
  {
    if( isValid( pos ) )
    {
      return grid[ pos ];
    }
    else
    {
      //Logger::warning( "ERROR: failed to gather point (%d,%d) on grid", pos.getI(), pos.getJ() );
      return 0;
    }
  }

  bool isValid( const TilePos& pos )
  {
    return ( pos.getI() >= 0 && pos.getJ() >= 0 && pos.getI() < (int)grid.size() && pos.getJ() < (int)grid[pos.getI()].size() );
  }

  bool isWalkable( const TilePos& pos )
  {
    if( isValid( pos ) )
    {
       bool ret;
       condition( at( pos )->tile, ret );
       return ret;
    }
    return false;
  }

  bool aStar(TilePos start, TilesArray arrivedArea, Pathway& oPathWay, int flags );

  void isRoad( const Tile* tile, bool& possible );
  void isWater( const Tile* tile, bool& possible );
  void isTerrain( const Tile* tile, bool& possible );
};

Pathfinder::Pathfinder() : _d( new Impl )
{
  _d->maxLoopCount = 1200;
}

void Pathfinder::update( const Tilemap& tilemap )
{
  _d->tilemap = const_cast< Tilemap* >( &tilemap );

  _d->grid.resize( tilemap.getSize() );
  for( int k=0; k < tilemap.getSize(); k++)
  {
    _d->grid[ k ].resize( tilemap.getSize());
  }

  TilesArray tiles = _d->tilemap->getArea( TilePos( 0, 0 ), Size( tilemap.getSize() ) );
  foreach( Tile* tile, tiles )
  {
    _d->grid[ tile->getI() ][ tile->getJ() ] = new AStarPoint( tile );
  }
}

bool Pathfinder::getPath(TilePos start, TilesArray arrivedArea, Pathway& oPathway, int flags)
{
  if( (flags & checkStart) )
  {
    AStarPoint* ap = _d->at( start );
    if( !ap || !(ap->tile) || !(ap->tile->isWalkable( true ) ) )
      return false;
  }

  if( flags & traversePath )
  {
    return _d->getTraversingPoints( start, arrivedArea.front()->getIJ(), oPathway );
  }

  return _d->aStar( start, arrivedArea, oPathway, flags );
}

bool Pathfinder::getPath( const Tile& start, const Tile& stop, Pathway& oPathWay, int flags )
{
  return getPath( start.getIJ(), stop.getIJ(), oPathWay, flags );
}

void Pathfinder::setCondition(const TilePossibleCondition& condition)
{
  _d->condition = condition;
}

bool Pathfinder::getPath( TilePos start, TilePos stop, Pathway& oPathway, int flags)
{
  TilesArray area;
  area.push_back( &_d->tilemap->at(stop) );
  return getPath( start, area, oPathway, flags );
}

bool Pathfinder::Impl::getTraversingPoints( TilePos start, TilePos stop, Pathway& oPathway )
{
  oPathway.init( *tilemap, tilemap->at( start ) );

  if( start == stop )
    return false;

  TilePos cPos = start;
  while( cPos != stop )
  {
    TilePos move( math::clamp( stop.getI() - cPos.getI(), -1, 1 ), math::clamp( stop.getJ() - cPos.getJ(), -1, 1 ) );
    oPathway.setNextTile( *(at( cPos + move )->tile) );
    cPos += move;
  }

  return true;
}

unsigned int Pathfinder::getMaxLoopCount() const
{
  return _d->maxLoopCount;
}

bool _inArea( APoints& area, AStarPoint* end )
{
  foreach( AStarPoint* p, area )
  {
    if( p == end )
      return true;
  }

  return false;
}

void Pathfinder::Impl::isRoad( const Tile* tile, bool& possible ) {  possible = tile ? tile->isWalkable( false ) : false; }
void Pathfinder::Impl::isTerrain( const Tile* tile, bool& possible ) {   possible = tile ? tile->isWalkable( true ) : false; }
void Pathfinder::Impl::isWater( const Tile* tile, bool& possible ) { possible = tile ? tile->getFlag( Tile::tlWater ) : false; }

bool Pathfinder::Impl::aStar(TilePos startPos, TilesArray arrivedArea, Pathway& oPathWay, int flags )
{
  if( arrivedArea.empty() )
    return false;

  oPathWay.init( *tilemap, tilemap->at( startPos ) );

  if( (flags & customCondition)) {}
  else if( (flags & roadOnly) > 0 ) { condition = makeDelegate( this, &Impl::isRoad); }
  else if( (flags & terrainOnly) > 0 ) { condition = makeDelegate( this, &Impl::isTerrain ); }
  else if( (flags & waterOnly) > 0 ) { condition = makeDelegate( this, &Impl::isWater ); }
  else
  {
    return false;
  }

  // Define points to work with
  AStarPoint* start = at( startPos );
  APoints endPoints;

  foreach( Tile* tile, arrivedArea )
  {
    endPoints.push_back( at( tile->getIJ() ) );
  }

  AStarPoint* current = NULL;
  AStarPoint* child = NULL;

  // Define the open and the close list
  list<AStarPoint*> openList;
  list<AStarPoint*> closedList;

  unsigned int n = 0;

  // Add the start point to the openList
  openList.push_back( start );
  start->opened = true;

  while( n == 0 || ( !_inArea( endPoints, current ) && n < maxLoopCount ))
  {
    // Look for the smallest F value in the openList and make it the current point
    foreach( AStarPoint* point, openList)
    {
      if( point == openList.front() || point->getFScore() <= current->getFScore() )
      {
        current = point;
      }
    }

    // Stop if we reached the end
    if( _inArea( endPoints, current ) )
    {
      break;
    }

    // Remove the current point from the openList
    openList.remove(current);
    current->opened = false;

    // Add the current point to the closedList
    closedList.push_back(current);
    current->closed = true;

    // Get all current's adjacent walkable points
    for (int x = -1; x < 2; x ++)
    {
      for (int y = -1; y < 2; y ++)
      {
        // If it's current point then pass
        if (x == 0 && y == 0)
        {
          continue;
        }

        if( (flags & fourDirection) && !(x==0 || y==0) )
          continue;
        // Get this point
        child = at( current->getPos() + TilePos( x, y ) );

        if( !child )
        {
          //Logger::warning( "No child for parent is (%d,%d)", current->getPos().getI() + x, current->getPos().getJ() + y );
          continue;
        }
        // If it's closed or not walkable then pass

        if( child->closed || !isWalkable( child->getPos() ) )
        {
          continue;
        }

        // If we are at a corner
        if (x != 0 && y != 0)
        {
          // if the next horizontal point is not walkable or in the closed list then pass
          //AStarPoint* tmpPoint = getPoint( current->pos + TilePos( 0, y ) );
          TilePos tmp = current->getPos() + TilePos( 0, y );
          if( !isWalkable( tmp ) || at( tmp )->closed)
          {
            continue;
          }

          tmp = current->getPos() + TilePos( x, 0 );
          // if the next vertical point is not walkable or in the closed list then pass
          if( !isWalkable( tmp ) || at( tmp )->closed)
          {
            continue;
          }
        }

        // If it's already in the openList
        if (child->opened)
        {
          // If it has a wroste g score than the one that pass through the current point
          // then its path is improved when it's parent is the current point
          if (child->getGScore() > child->getGScore(current))
          {
            // Change its parent and g score
            child->setParent(current);
            child->computeScores( endPoints.front() );
          }
        }
        else
        {
          // Add it to the openList with current point as parent
          openList.push_back(child);
          child->opened = true;

          // Compute it's g, h and f score
          child->setParent(current);
          child->computeScores( endPoints.front() );
        }
      }
    }

    n++;
  }

  // Reset
  foreach( AStarPoint* point, openList) { point->opened = false; }

  foreach( AStarPoint* point, closedList) { point->closed = false; }

  if( n == maxLoopCount )
  {
    return false;
  }
  // Resolve the path starting from the end point
  APoints lPath;
  while( current->hasParent() && current != start )
  {
    lPath.insert( lPath.begin(), 1, current );
    current = current->getParent();
    n++;
  }

  foreach( AStarPoint* pathPoint, lPath )
  {
    oPathWay.setNextTile( tilemap->at( pathPoint->getPos() ) );
  }

  return oPathWay.getLength() > 0;
}

Pathfinder& Pathfinder::getInstance()
{
  static Pathfinder inst;
  return inst;
}

Pathfinder::~Pathfinder()
{

}
