#ifndef __OPENCAESAR3_ASTARPATHFINDING_H_INCLUDED__
#define __OPENCAESAR3_ASTARPATHFINDING_H_INCLUDED__

#include <iostream>
#include <vector>
#include <list>
#include <map>

#include "oc3_scopedptr.hpp"

class Tilemap;
class TilePos;
class PathWay;
class AStarPoint;
class Size;
class Tile;

class Pathfinder
{
public:
  typedef enum { checkStart=0x1, checkStop=0x2, noFlags=0x0 } Flags;
  static const AStarPoint invalidPoint;
  static Pathfinder& getInstance();

  void update( const Tilemap& tmap );

  bool getPath( const TilePos& start, const TilePos& stop, 
                PathWay& oPathWay, int flags,
                const Size& arrivedArea );

  bool getPath( const Tile& start, const Tile& stop, 
                PathWay& oPathWay, int flags, 
                const Size& arrivedArea );
  
  unsigned int getMaxLoopCount() const;

  ~Pathfinder();
private:
  Pathfinder();

  bool aStar( const TilePos& start, const TilePos& stop, const Size& arrivedArea, PathWay& oPathWay, int flags );
  std::list<AStarPoint*> getTraversingPoints( const TilePos& start, const TilePos& stop );

  AStarPoint* getPoint( const TilePos& pos );

  bool pointExists( const TilePos& pos );
  bool pointIsWalkable( const TilePos& pos );

  std::map< int, std::map< int, AStarPoint* > > grid;
  Tilemap* _tilemap;

  class Impl;
  ScopedPtr< Impl > _d;
};


#endif //__OPENCAESAR3_ASTARPATHFINDING_H_INCLUDED__