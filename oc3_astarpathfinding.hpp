#ifndef __OPENCAESAR3_ASTARPATHFINDING_H_INCLUDED__
#define __OPENCAESAR3_ASTARPATHFINDING_H_INCLUDED__

#include <iostream>
#include <vector>
#include <list>
#include <map>

class Tilemap;
class TilePos;
class PathWay;
class AStarPoint;
class Size;

class Pathfinder
{
public:
  static const AStarPoint invalidPoint;
  static Pathfinder& getInstance();

  void update( const Tilemap& tmap );

  bool getPath( const TilePos& start, const TilePos& stop, 
                PathWay& oPathWay, bool checkLast, bool tryTraverse,
                const Size& arrivedArea );

  int getMaxLoopCount() const;

private:
  Pathfinder();

  bool aStar( const TilePos& start, const TilePos& stop, const Size& arrivedArea, PathWay& oPathWay );
  std::list<AStarPoint*> getTraversingPoints( const TilePos& start, const TilePos& stop );

  AStarPoint* getPoint( const TilePos& pos );

  bool pointExists( const TilePos& pos );
  bool pointIsWalkable( const TilePos& pos );

  std::map< int, std::map< int, AStarPoint* > > grid;
  Tilemap* _tilemap;
};


#endif //__OPENCAESAR3_ASTARPATHFINDING_H_INCLUDED__