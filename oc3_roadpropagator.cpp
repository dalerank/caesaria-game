#include "oc3_roadpropagator.h"
#include <set>
#include <map>
#include "exception.hpp"
#include "tilemap.hpp"
#include "oc3_positioni.h"
#include <list>

class RoadPropagator::Impl
{
public:
    Tilemap* tilemap;
    Tile* startTile;

    int mapSize;
};

RoadPropagator::RoadPropagator( Tilemap& tileMap, Tile* startTile ) : _d( new Impl )
{
    _d->tilemap = &tileMap;
    _d->startTile = startTile;
    _d->mapSize = tileMap.getSize();
}

bool RoadPropagator::getPath( Tile* destination, std::list< Tile* >& oPathWay )
{
    if( !_d->startTile || !destination )    
        return false;

    TilePos startPos = _d->startTile->getIJ();
    TilePos stopPos = destination->getIJ();
    int iStep = startPos.getI() < stopPos.getI() ? 1 : -1;
    int jStep = startPos.getJ() < stopPos.getJ() ? 1 : -1;

    if( startPos == stopPos )
    {
        oPathWay.push_back( _d->startTile );
        return true;
    }
    
    for( int i=startPos.getI(); i != stopPos.getI(); i+=iStep )
    {
        Tile* curTile = &_d->tilemap->at( i, stopPos.getJ() );
         
        if( curTile->get_terrain().isConstructible() || curTile->get_terrain().isRoad() )
            oPathWay.push_back( curTile );
        else
            return false;
    }

    for( int j=startPos.getJ(); j != stopPos.getJ(); j+=jStep )
    {
        Tile* curTile = &_d->tilemap->at( startPos.getI(), j );

        if( curTile->get_terrain().isConstructible() || curTile->get_terrain().isRoad() )
            oPathWay.push_back( curTile );
        else
            return false;
    }

    return true;
}

RoadPropagator::~RoadPropagator()
{

}