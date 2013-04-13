#ifndef __OPENCAESAR3_ROADPROPAGATOR_H_INCLUDE_
#define __OPENCAESAR3_ROADPROPAGATOR_H_INCLUDE_

#include "oc3_scopedptr.h"
#include <list>

class Tile;
class Tilemap;

class RoadPropagator
{
public:
    RoadPropagator( Tilemap& tileMap, Tile* startTile );
    ~RoadPropagator();

    /** finds the shortest path between origin and destination
    * returns True if a path exists
    * the path is returned in oPathWay
    */
    bool getPath( Tile* destination, std::list<Tile*>& oPathWay );
private:
    class Impl;
    ScopedPtr< Impl > _d;
};

#endif //__OPENCAESAR3_ROADPROPAGATOR_H_INCLUDE_