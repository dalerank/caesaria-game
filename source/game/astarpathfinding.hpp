#ifndef __OPENCAESAR3_ASTARPATHFINDING_H_INCLUDED__
#define __OPENCAESAR3_ASTARPATHFINDING_H_INCLUDED__

#include <iostream>
#include <vector>
#include <list>

#include "core/scopedptr.hpp"

class Tilemap;
class TilePos;
class Pathway;
class AStarPoint;
class Size;
class Tile;

class Pathfinder
{
public:
  typedef enum { noFlags=0x0, checkStart=0x1, checkStop=0x2, roadOnly=0x4, waterOnly=0x8,
                 terrainOnly=0x10, traversePath=0x20, everyWhere=0x80 } Flags;
  static const AStarPoint invalidPoint;
  static Pathfinder& getInstance();

  void update( const Tilemap& tmap );

  bool getPath( const TilePos& start, const TilePos& stop, 
                Pathway& oPathWay, int flags,
                const Size& arrivedArea );

  bool getPath( const Tile& start, const Tile& stop, 
                Pathway& oPathWay, int flags, 
                const Size& arrivedArea );
  
  unsigned int getMaxLoopCount() const;

  ~Pathfinder();
private:
  Pathfinder();

  bool aStar( const TilePos& start, const TilePos& stop, const Size& arrivedArea, Pathway& oPathWay, int flags );

  class Impl;
  ScopedPtr< Impl > _d;
};


#endif //__OPENCAESAR3_ASTARPATHFINDING_H_INCLUDED__
