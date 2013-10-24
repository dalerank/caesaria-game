#include "oc3_astarpathfinding.hpp"
#include "oc3_tilemap.hpp"
#include "core/position.hpp"
#include "oc3_astarpoint.hpp"
#include "oc3_path_finding.hpp"
#include "oc3_stringhelper.hpp"
#include "core/foreach.hpp"

using namespace std;

class Pathfinder::Impl
{
public:
  typedef map<AStarPoint*,AStarPoint::WalkableType> AspSavemap;
  typedef vector< AStarPoint* > AspRow;

  class Grid : public vector< AspRow >
  {
  public:
    AStarPoint* operator[](const TilePos& pos )
    {
      return at( pos.getI() ).at( pos.getJ() );
    }

    AspRow& operator[]( unsigned int row )
    {
      return at( row );
    }
  };

  Grid grid;
  Tilemap* tilemap;

  bool getTraversingPoints( const TilePos& start, const TilePos& stop, PathWay& oPathWay );

  AStarPoint* at( const TilePos& pos )
  {
    if( isValid( pos ) )
    {
      return grid[ pos ];
    }
    else
    {
      StringHelper::debug( 0xff, "ERROR: failed to gather point (%d,%d) on grid", pos.getI(), pos.getJ() );
      return 0;
    }
  }

  bool isValid( const TilePos& pos )
  {
    return ( pos.getI() >= 0 && pos.getJ() >= 0 && pos.getI() < (int)grid.size() && pos.getJ() < (int)grid[pos.getI()].size() );
  }

  bool isWalkable( const TilePos& pos, int flags )
  {
    return ( isValid( pos ) && at( pos )->isWalkable( flags ) );
  }

  void tunePoints( int flags );
};

Pathfinder::Pathfinder() : _d( new Impl )
{
}

void Pathfinder::update( const Tilemap& tilemap )
{
  _d->tilemap = const_cast< Tilemap* >( &tilemap );

  _d->grid.resize( tilemap.getSize() );
  for( int k=0; k < tilemap.getSize(); k++)
  {
    _d->grid[ k ].resize( tilemap.getSize());
  }

  TilemapTiles tiles = _d->tilemap->getArea( TilePos( 0, 0 ), Size( tilemap.getSize() ) );
  foreach( Tile* tile, tiles )
  {
    _d->grid[ tile->getI() ][ tile->getJ() ] = new AStarPoint( tile );
  }
}

bool Pathfinder::getPath( const Tile& start, const Tile& stop, PathWay& oPathWay,
                          int flags, const Size& arrivedArea )
{
  return getPath( start.getIJ(), stop.getIJ(), oPathWay, flags, arrivedArea );
}

bool Pathfinder::getPath( const TilePos& start, const TilePos& stop,  
                          PathWay& oPathWay, int flags,
                          const Size& arrivedArea )
{
  if( (flags & checkStart) && !_d->isWalkable( start, AStarPoint::wtAll ) )
      return false;

  if( flags & traversePath )
  {
    return _d->getTraversingPoints( start, stop, oPathWay );
  }

  return aStar( start, stop, arrivedArea, oPathWay, flags );
}

bool Pathfinder::Impl::getTraversingPoints( const TilePos& start, const TilePos& stop, PathWay& oPathway )
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
  return 600;
}

void Pathfinder::Impl::tunePoints( int flags )
{

}

bool Pathfinder::aStar( const TilePos& startPos, const TilePos& stopPos, 
                        const Size& arrivedArea, PathWay& oPathWay,
                        int flags )
{
  oPathWay.init( *_d->tilemap, _d->tilemap->at( startPos ) );

  int pointFlags = AStarPoint::wtAll;
  if( (flags & roadOnly) > 0 ) { pointFlags = AStarPoint::road; }
  else if( (flags & terrainOnly) > 0 ) { pointFlags = AStarPoint::road | AStarPoint::land; }
  else if( (flags & waterOnly) > 0 ) { pointFlags = AStarPoint::water; }

  // Define points to work with
  AStarPoint* start = _d->at( startPos );
  AStarPoint* end = _d->at( stopPos );
  AStarPoint* current = NULL;
  AStarPoint* child = NULL;

  // Define the open and the close list
  list<AStarPoint*> openList;
  list<AStarPoint*> closedList;
  list<AStarPoint*>::iterator i;

  int tSize = _d->tilemap->getSize();
  Impl::AspSavemap saveArrivedArea;

  TilePos arrivedAreaStart( math::clamp( stopPos.getI()-arrivedArea.getWidth(), 0, tSize ),
                            math::clamp( stopPos.getJ()-arrivedArea.getHeight(), 0, tSize) );

  TilePos arrivedAreaStop(  math::clamp( stopPos.getI()+arrivedArea.getWidth(), 0, tSize ),
                            math::clamp( stopPos.getJ()+arrivedArea.getHeight(), 0, tSize) );
  
  for( int i=arrivedAreaStart.getI(); i <= arrivedAreaStop.getI(); i++ )
  {
    for( int j=arrivedAreaStart.getJ(); j <= arrivedAreaStop.getJ(); j++ )
    {
      AStarPoint* ap = _d->at( TilePos( i, j) );
      if( ap )
      {
        saveArrivedArea[ ap ] = ap->priorWalkable;
        ap->priorWalkable = AStarPoint::alwaysWalkable;
      }
    }
  }

  unsigned int n = 0;

  // Add the start point to the openList
  openList.push_back( start );
  start->opened = true;

  while( n == 0 || (current != end && n < getMaxLoopCount() ))
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
    if( current == end ) 
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
        child = _d->at( current->getPos() + TilePos( x, y ) );

        if( !child )
        {
          StringHelper::debug( 0xff, "No child for parent is (%d,%d)", current->getPos().getI() + x, current->getPos().getJ() + y );
          continue;
        }
        // If it's closed or not walkable then pass
        if( child->closed || !child->isWalkable( pointFlags ) )
        {
          continue;
        }

        // If we are at a corner
        if (x != 0 && y != 0)
        {
          // if the next horizontal point is not walkable or in the closed list then pass
          //AStarPoint* tmpPoint = getPoint( current->pos + TilePos( 0, y ) );
          TilePos tmp = current->getPos() + TilePos( 0, y );
          if( !_d->isWalkable( tmp, pointFlags ) || _d->at( tmp )->closed)
          {
            continue;
          }

          tmp = current->getPos() + TilePos( x, 0 );
          // if the next vertical point is not walkable or in the closed list then pass
          if( !_d->isWalkable( tmp, pointFlags ) || _d->at( tmp )->closed)
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
            child->computeScores(end);
          }
        }
        else
        {
          // Add it to the openList with current point as parent
          openList.push_back(child);
          child->opened = true;

          // Compute it's g, h and f score
          child->setParent(current);
          child->computeScores(end);
        }
      }
    }

    n++;
  }

  // Reset
  foreach( AStarPoint* point, openList) { point->opened = false; }

  foreach( AStarPoint* point, closedList) { point->closed = false; }

  foreach( Impl::AspSavemap::value_type point, saveArrivedArea )
  {
    point.first->priorWalkable = point.second;
  }

  if( n == getMaxLoopCount() )
  {
    return false;
  }
  // Resolve the path starting from the end point
  list<AStarPoint*> lPath;
  while( current->hasParent() && current != start )
  {
    lPath.push_front( current );
    current = current->getParent();
    n++;
  }

  foreach( AStarPoint* pathPoint, lPath )
  {
    oPathWay.setNextTile( _d->tilemap->at( pathPoint->getPos() ) );
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
