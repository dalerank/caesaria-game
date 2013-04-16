// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.


#include "oc3_roadpropagator.hpp"

#include <set>
#include <map>

#include "oc3_exception.hpp"
#include "oc3_tilemap.hpp"
#include "oc3_positioni.hpp"

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
    
    // propagate on I axis
    for( int i=startPos.getI();; i+=iStep )
    {
        Tile* curTile = &_d->tilemap->at( i, stopPos.getJ() );
         
        if( curTile->get_terrain().isConstructible() || curTile->get_terrain().isRoad() )
            oPathWay.push_back( curTile );
        else
            return false;

        if( i == stopPos.getI() )
           break;
    }

    // propagate on J axis
    for( int j=startPos.getJ();; j+=jStep )
    {
        Tile* curTile = &_d->tilemap->at( startPos.getI(), j );

        if( curTile->get_terrain().isConstructible() || curTile->get_terrain().isRoad() )
            oPathWay.push_back( curTile );
        else
            return false;

        if( j == stopPos.getJ() )
           break;
    }

    return true;
}

RoadPropagator::~RoadPropagator()
{

}
