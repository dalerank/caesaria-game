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
//
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "routefinder.hpp"
#include "empiremap.hpp"
#include "core/delegate.hpp"
#include "core/foreach.hpp"

namespace world
{

class EmPoint
{

public:
  EmPoint* parent;
  bool closed;
  bool opened;
  int f, g, h;
  TilePos pos;
  unsigned int info;

  EmPoint()
  {
    parent = NULL;
    closed = false;
    opened = false;
    info = EmpireMap::unknown;

    f = g = h = 0;
  }

  EmPoint( TilePos p, EmpireMap::TerrainType i )
  {
    parent = NULL;
    closed = false;
    opened = false;
    pos = p;
    info = i;
    f = g = h = 0;
  }

  EmPoint* getParent()  {    return parent; }
  void setParent( EmPoint* p)  {    parent = p;  }

  int getGScore( const EmPoint* p)
  {
    TilePos otherPos = p->pos;
    return p->g + ((pos.i() == otherPos.i() || pos.j() == otherPos.j()) ? 10 : 14);
  }

  int getHScore( const EmPoint* p)
  {
    TilePos otherPos = p ? p->pos : TilePos( 0, 0 );
    return (abs(otherPos.i() - pos.i()) + abs(otherPos.j() - pos.j())) * 10;
  }

  void computeScores( const EmPoint* end )
  {
    g = getGScore(parent);
    h = getHScore(end);
    f = g + h;
  }

  int getGScore(){    return g;  }
  int getHScore(){    return h;  }
  int getFScore(){    return f;  }
  bool hasParent(){    return parent != NULL;  }
};

typedef std::vector< EmPoint* > EmPoints;
typedef Delegate2< const EmPoint*, bool& > IsWalkableFunctor;

class TraderouteFinder::Impl
{
public:
  class Grid : std::vector< EmPoint >
  {
  public:
    void resize( Size size )
    {
      _size = size;
      std::vector< EmPoint >::resize( _size.area() );
    }

    EmPoint* operator[]( const TilePos& pos )
    {
      unsigned int offset = pos.j() * _size.width() + pos.i();
      if( offset < size() )
      {
      	Grid::iterator it = begin();
      	std::advance( it, offset );
      	return &(*it)	;
      }
      
      return 0;
    }
  private:
    Size _size;
  };

  IsWalkableFunctor isWalkableCondition;
  Grid grid;
  unsigned int maxLoopCount;

  bool aStar(TilePos startPos, TilePos stopPos, TilePosArray& way, int flags);
  void update( const EmpireMap& emap );
  void isTerrain( const EmPoint* p, bool& ret ) { ret = p ? (p->info & EmpireMap::land) : false; }
  void isWater( const EmPoint* p, bool& ret ) { ret = p ? (p->info & EmpireMap::sea) : false; }
  bool isWalkable( const TilePos& pos )
  {
    bool ret;
    isWalkableCondition( grid[ pos ], ret );
    return ret;
  }
};

TraderouteFinder::TraderouteFinder(const EmpireMap& empiremap)
  : _d( new Impl )
{
  _d->update( empiremap );
  _d->maxLoopCount = 1200;
}

bool TraderouteFinder::findRoute(TilePos start, TilePos stop, TilePosArray& way, int flags)
{
  return _d->aStar( start, stop, way, flags );
}

void TraderouteFinder::setTerrainType(TilePos pos, unsigned int type)
{
  _d->grid[ pos ]->info = type;
}

TraderouteFinder::~TraderouteFinder(){}

void TraderouteFinder::Impl::update( const EmpireMap& emap )
{
  grid.resize( emap.getSize() );
  for( int k=0; k < emap.getSize().height(); k++)
  {
    for( int i=0; i < emap.getSize().width(); i++ )
    {
      TilePos p(i,k);

      EmPoint* et = grid[ p ];

      et->pos = p;
      et->info = emap.at( p );
    }
  }
}


bool TraderouteFinder::Impl::aStar(TilePos startPos, TilePos stopPos, TilePosArray& way, int flags )
{
  if( (flags & terrainOnly) > 0 ) { isWalkableCondition = makeDelegate( this, &Impl::isTerrain ); }
  else if( (flags & waterOnly) > 0 ) { isWalkableCondition = makeDelegate( this, &Impl::isWater ); }
  else
  {
    return false;
  }

  // Define points to work with
  EmPoint* start = grid[ startPos ];

  EmPoint* current = NULL;
  EmPoint* child = NULL;

  // Define the open and the close list
  std::list<EmPoint*> openList;
  std::list<EmPoint*> closedList;

  unsigned int n = 0;

  // Add the start point to the openList
  openList.push_back( start );
  start->opened = true;
  const EmPoint* stopPoint = grid[ stopPos ];

  while( n == 0 || ( !(stopPoint == current) && n < maxLoopCount ))
  {
    // Look for the smallest F value in the openList and make it the current point
    foreach( point, openList)
    {
      if( *point == openList.front() || (*point)->getFScore() <= current->getFScore() )
      {
        current = *point;
      }
    }

    // Stop if we reached the end
    if( stopPoint == current  )
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

        // Get this point
        child = grid[ current->pos + TilePos( x, y ) ];

        if( !child )
        {
          continue;
        }

        // If it's closed or not walkable then pass
        if( child->closed || !isWalkable( child->pos ) )
        {
          continue;
        }

        // If we are at a corner
        if (x != 0 && y != 0)
        {
          // if the next horizontal point is not walkable or in the closed list then pass
          TilePos tmp = current->pos + TilePos( 0, y );
          if( !isWalkable( tmp ) || grid[ tmp ]->closed )
          {
            continue;
          }

          tmp = current->pos + TilePos( x, 0 );
          // if the next vertical point is not walkable or in the closed list then pass
          if( !isWalkable( tmp ) || grid[ tmp ]->closed)
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
            child->computeScores( stopPoint );
          }
        }
        else
        {
          // Add it to the openList with current point as parent
          openList.push_back(child);
          child->opened = true;

          // Compute it's g, h and f score
          child->setParent(current);
          child->computeScores( stopPoint );
        }
      }
    }

    n++;
  }

  // Reset
  foreach( point, openList) { (*point)->opened = false; }

  foreach( point, closedList) { (*point)->closed = false; }

  if( n >= maxLoopCount )
  {
    return false;
  }
  // Resolve the path starting from the end point
  EmPoints lPath;
  while( current->hasParent() && current != start )
  {
    lPath.insert( lPath.begin(), 1, current );
    current = current->getParent();
    n++;
  }

  if( !lPath.empty() )
  {
    way.push_back( startPos );
    foreach( pathPoint, lPath ) { way.push_back( (*pathPoint)->pos ); }
  }

  return way.size() > 0;
}

}
