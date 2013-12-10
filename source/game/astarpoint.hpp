#ifndef __OPENCAESAR3_ASTARPOINT_H_INCLUDED__
#define __OPENCAESAR3_ASTARPOINT_H_INCLUDED__

#include "core/position.hpp"
#include "gfx/tile.hpp"

class AStarPoint
{

public:
  typedef enum { autoWalkable, alwaysWalkable, alwaysImpassable } WalkableType;
  typedef enum { land=1, road=2, water=4, wtAll=0xf } WayType;

  AStarPoint* parent;
  bool closed;
  bool opened;
  int f, g, h;
  const Tile* tile;
  WalkableType priorWalkable;

  AStarPoint()
  {
    parent = NULL;
    closed = false;
    opened = false;
    priorWalkable = alwaysImpassable;
    tile = 0;

    f = g = h = 0;
  }

  TilePos getPos()
  {
    return tile ? tile->getIJ() : TilePos( 0, 0 );
  }

  bool isWalkable( int wtype )
  {
    switch( priorWalkable )
    {
    case autoWalkable:
    {
      if( (wtype & land)!=0 || (wtype & road) != 0 )
      {
        return tile ? tile->isWalkable( (wtype & land) != 0 ) : false;
      }
      else if( (wtype & water) != 0 )
      {
        return tile ? tile->getFlag( Tile::tlWater ) : false;
      }
    }
    break;

    case alwaysWalkable: return true;
    case alwaysImpassable: return false;   
    }

    return false;
  }

  AStarPoint( const Tile* t ) : tile( t )
  {    
    parent = NULL;
    closed = false;
    opened = false;
    priorWalkable = autoWalkable;

    f = g = h = 0;
  }

  AStarPoint* getParent()
  {
    return parent;
  }

  void setParent(AStarPoint* p)
  {
    parent = p;
  }

  int getGScore(AStarPoint* p)
  { 
    int offset = p->tile
                  ? (p->tile->getFlag( Tile::tlRoad ) ? 0 : +50)
                  : (+100);
    TilePos pos = tile ? tile->getIJ() : TilePos( 0, 0 ); 
    TilePos otherPos = p->tile ? p->tile->getIJ() : getPos();
    return p->g + ((pos.getI() == otherPos.getI() || pos.getJ() == otherPos.getJ()) ? 10 : 14) + offset;
  }

  int getHScore(AStarPoint* p)
  {
    TilePos pos = tile ? tile->getIJ() : TilePos( 0, 0 ); 
    TilePos otherPos = p ? p->tile->getIJ() : TilePos( 0, 0 );
    return (abs(otherPos.getI() - pos.getI()) + abs(otherPos.getJ() - pos.getJ())) * 10;
  }

  int getGScore()
  {
    return g;
  }

  int getHScore()
  {
    return h;
  }

  int getFScore()
  {
    return f;
  }

  void computeScores(AStarPoint* end)
  {
    g = getGScore(parent);
    h = getHScore(end);
    f = g + h;
  }

  bool hasParent()
  {
    return parent != NULL;
  }
};

#endif //__OPENCAESAR3_ASTARPOINT_H_INCLUDED__
