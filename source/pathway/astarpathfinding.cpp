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

#include "objects/construction.hpp"
#include "astarpathfinding.hpp"
#include "gfx/tilemap.hpp"
#include "core/position.hpp"
#include "astarpoint.hpp"
#include "path_finding.hpp"
#include "core/utils.hpp"
#include "core/foreach.hpp"
#include "core/logger.hpp"
#include "core/stacktrace.hpp"
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

  class Grid : std::vector< AStarPoint* >
  {
  public:
    inline size_type hash( const TilePos& pos ) const { return pos.j() * _size.width() + pos.i(); }
    
    AStarPoint* operator[](const TilePos& pos ) const
    {
      return *(begin() + hash( pos ));
    }

    void reset( int width, int height )
    {
      foreach( it, *this )
      {
        delete *it;
        *it = 0;
      }

    	_size = Size( width, height );
    	resize( _size.area() );
    }
    
    void init( Tile* tile )
    {
      *(begin() + hash( tile->pos() ) ) = new AStarPoint( tile );	
    }
    
    Size _size;
  };

  Grid grid;
  unsigned int maxLoopCount;
  int verbose;

  bool getTraversingPoints(TilePos start, TilePos stop, Pathway& oPathWay );

  AStarPoint* at( const TilePos& pos ) const
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

  const Tile* tile( const TilePos& pos ) const
  {
    AStarPoint* ap = at( pos );
    return ap ? ap->tile : 0;
  }

  bool isValid( const TilePos& pos ) const
  {
    return ( pos.i() >= 0 && pos.j() >= 0 && pos.i() < grid._size.width() && pos.j() < grid._size.height() );
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

  bool aStar(const TilePos& start, TilesArray arrivedArea, Pathway& oPathWay, int flags );
  void isRoad( const Tile* tile, bool& possible );
  void isDeepWater( const Tile* tile, bool& possible );
  void isWater( const Tile* tile, bool& possible );
  void isTerrain( const Tile* tile, bool& possible );
};

Pathfinder::Pathfinder() : _d( new Impl )
{
  _d->maxLoopCount = 4800;
  _d->verbose = 0;
}

void Pathfinder::update( const Tilemap& tilemap )
{
  Logger::warning( "Pathfinder: start updating" );

  Logger::warning( "Pathfinder: resizing grid to %d", tilemap.size() );
  int size = tilemap.size();
  _d->grid.reset( size, size );

  Logger::warning( "Pathfinder: allocation AStarPoints" );
  const TilesArray& tiles = tilemap.allTiles();
  foreach( tile, tiles )
  {
    _d->grid.init( *tile );
  }
}

Pathway Pathfinder::getPath(TilePos start, TilesArray arrivedArea, int flags)
{
  if( _d->grid._size.area() == 0 )
    return Pathway();

  Pathway oPathway;
  if( (flags & checkStart) )
  {
    AStarPoint* ap = _d->at( start );
    if( !ap || !(ap->tile) || !(ap->tile->isWalkable( true ) ) )
      return Pathway();
  }

  if( flags & traversePath )
  {
    bool found = _d->getTraversingPoints( start, arrivedArea.front()->pos(), oPathway );
    return found ? oPathway : Pathway();
  }

  bool found = _d->aStar( start, arrivedArea, oPathway, flags );
  return found ? oPathway : Pathway();
}

Pathway Pathfinder::getPath( const Tile& start, const Tile& stop, int flags )
{
  return getPath( start.pos(), stop.pos(), flags );
}

void Pathfinder::setCondition(const TilePossibleCondition& condition)
{
  _d->condition = condition;
}

Pathway Pathfinder::getPath(TilePos start, TilePos stop,  int flags)
{
  if( start == stop )
  {
    Logger::warning( "WARNING!!! Pathfinder::getPath start==stop" );
    return Pathway();
  }

  if( _d->grid._size.area() == 0 )
  {
    return Pathway();
  }

  TilesArray area;
  area.push_back( const_cast<Tile*>( _d->tile( stop ) ) );
  return getPath( start, area, flags );
}

bool Pathfinder::Impl::getTraversingPoints( TilePos start, TilePos stop, Pathway& oPathway )
{
  oPathway.init( *tile( start ) );

  if( start == stop )
    return false;

  TilePos cPos = start;
  while( cPos != stop )
  {
    TilePos move( math::clamp( stop.i() - cPos.i(), -1, 1 ), math::clamp( stop.j() - cPos.j(), -1, 1 ) );
    oPathway.setNextTile( *(at( cPos + move )->tile) );
    cPos += move;
  }

  return true;
}

unsigned int Pathfinder::maxLoopCount() const {  return _d->maxLoopCount; }
void Pathfinder::setMaxLoopCount(unsigned int count){ _d->maxLoopCount = count; }
void Pathfinder::setVerboseMode(int level) {  _d->verbose = level;}

bool _inArea( APoints& area, AStarPoint* end )
{
  foreach( p, area )
  {
    if( *p == end )
      return true;
  }

  return false;
}

void Pathfinder::Impl::isRoad( const Tile* tile, bool& possible ) {  possible = tile ? tile->isWalkable( false ) : false; }
void Pathfinder::Impl::isTerrain( const Tile* tile, bool& possible ) { possible = tile ? tile->isWalkable( true ) : false; }
void Pathfinder::Impl::isDeepWater( const Tile* tile, bool& possible ) { possible = tile ? tile->getFlag( Tile::tlDeepWater ) : false; }
void Pathfinder::Impl::isWater( const Tile* tile, bool& possible ) { possible = tile ? tile->getFlag( Tile::tlWater ) : false; }

bool Pathfinder::Impl::aStar( const TilePos& startPos, TilesArray arrivedArea, Pathway& oPathWay, int flags )
{
  if( arrivedArea.empty() )
  {
    Logger::warning( "AStarPathfinder: no arrived area" );
    return false;
  }  

  AStarPoint* ap = at( startPos );
  if( !ap || !ap->tile )
  {
    Logger::warning( "AStarPathfinder: wrong start pos at %d,%d", startPos.i(), startPos.j()  );
    return false;
  }

  oPathWay.init( *(ap->tile) );

  foreach( tile, arrivedArea )
  {
    if( (*tile)->pos() == startPos )
    {
      oPathWay.setNextTile( *(*tile) );
      return true;
    }
  }

  bool useRoad = (( flags & ignoreRoad ) == 0 );

  if( (flags & customCondition)) {}
  else if( (flags & roadOnly) > 0 ) { condition = makeDelegate( this, &Impl::isRoad); }
  else if( (flags & terrainOnly) > 0 ) { condition = makeDelegate( this, &Impl::isTerrain ); }
  else if( (flags & deepWaterOnly) > 0 ) { condition = makeDelegate( this, &Impl::isDeepWater ); }
  else if( (flags & waterOnly) > 0 ) { condition = makeDelegate( this, &Impl::isWater ); }
  else
  {
    return false;
  }

  // Define points to work with
  AStarPoint* start = at( startPos );
  if( !start )
    return false;
  APoints endPoints;

  foreach( tile, arrivedArea ) { endPoints.push_back( at( (*tile)->pos() ) ); }

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
    n++;
    // Look for the smallest F value in the openList and make it the current point
    foreach( point, openList)
    {
      if( *point == openList.front() || (*point)->getFScore() <= current->getFScore() )
      {
        current = *point;
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
          if (child->getGScore() > child->getGScore(current, useRoad ))
          {
            // Change its parent and g score
            child->setParent(current);
            child->computeScores( endPoints.front(), useRoad );
          }
        }
        else
        {
          // Add it to the openList with current point as parent
          openList.push_back(child);
          child->opened = true;

          // Compute it's g, h and f score
          child->setParent(current);
          child->computeScores( endPoints.front(), useRoad );
        }
      }
    }
  }

  // Reset
  foreach( point, openList) { (*point)->opened = false; }

  foreach( point, closedList) { (*point)->closed = false; }

  if( n == maxLoopCount )
  {
    if( verbose > 0 )
    {
      Logger::warning( "AStarPathfinder: maxLoopCount reached from [%d,%d] to [%d,%d]",
                     startPos.i(), startPos.j(), endPoints.front()->getPos().i(), endPoints.front()->getPos().j() );
      Stacktrace::print();
    }
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

  foreach( pathPoint, lPath )
  {
    oPathWay.setNextTile( *((*pathPoint)->tile) );
  }

  return oPathWay.length() > 0;
}

Pathfinder& Pathfinder::instance()
{
  static Pathfinder inst;
  return inst;
}

Pathfinder::~Pathfinder() {}
