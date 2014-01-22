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
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com

#include "path_finding.hpp"

#include "gfx/tilemap.hpp"
#include "city/city.hpp"
#include "core/exception.hpp"
#include "core/position.hpp"
#include "objects/road.hpp"
#include "gfx/tile.hpp"
#include "core/variant.hpp"
#include "city/helper.hpp"
#include "core/logger.hpp"
#include "objects/building.hpp"

class Propagator::Impl
{
public:
  std::set<PathwayPtr> activeBranches;

  typedef std::map<Tile*, PathwayPtr> RouteMap;

  RouteMap completedBranches;
  PlayerCityPtr city;
  Tile* origin;
  Tilemap* tilemap;
  bool allLands;  // true if can walk in all lands, false if limited to roads
  bool allDirections;  // true if can walk in all directions, false if limited to North/South/East/West
};

Propagator::Propagator(PlayerCityPtr city ) : _d( new Impl )
{
   _d->city = city;
   _d->tilemap = &city->getTilemap();
   _d->allLands = false;
   _d->allDirections = true;
}

void Propagator::setAllLands(const bool value)
{
   _d->allLands = value;
}

void Propagator::setAllDirections(const bool value)
{
  _d->allDirections = value;
}

void Propagator::init(TilePos origin)
{
  init( _d->tilemap->at( origin ) );
}


void Propagator::init( ConstructionPtr origin)
{
   // init propagation on access roads
  _d->origin = &origin->getTile();
  init( origin->getAccessRoads() );
}

void Propagator::init( Tile& origin )
{
  _d->origin = &origin;
  TilesArray tileList;
  tileList.push_back(&origin);

  init( tileList );
}

void Propagator::init(const TilesArray& origin)
{
  _d->activeBranches.clear();
  _d->completedBranches.clear();

  // init propagation
  for( TilesArray::const_iterator it=origin.begin(); it != origin.end(); it++ )
  {
    // std::cout << "Tile access " << tile.getI() << "," << tile.getJ() << std::endl;
    Tile* tile = *it;
    PathwayPtr pathWay( new Pathway() );
    pathWay->drop();
    pathWay->init(*_d->tilemap, *tile);
    // init active branches
    _d->activeBranches.insert( pathWay );
    // init trivial completed branches
    _d->completedBranches.insert( std::pair<Tile*, PathwayPtr>( tile, pathWay ) );
  }
}

void Propagator::propagate(const int maxDistance)
{
   int nbLoops = 0;  // to detect infinite loops

   std::set<PathwayPtr>::iterator firstBranch;

   // propagate on all tiles
   while (!_d->activeBranches.empty())
   {
      // while there are active paths
      if ((nbLoops++)>10000) THROW("Infinite loop detected during propagation");

      // get the shortest active path
      firstBranch = _d->activeBranches.begin();

      PathwayPtr pathWay = *firstBranch;
      const Tile& tile = pathWay->getDestination();
      //std::cout << "Propagation from tile " << tile.getI() << ", " << tile.getJ() << std::endl;

      int tileLength = 1;
      if (pathWay->getLength() + tileLength > maxDistance)
      {
         // we processed all paths within range. stop the propagation
         //std::cout << "MaxDistance reached. stop propagation" << std::endl;
         break;
      }

      // propagate to neighbour tiles
      TilesArray accessTiles = _d->tilemap->getRectangle( tile.getIJ() + TilePos( -1,-1 ),
                                                          tile.getIJ() + TilePos( 1, 1 ), _d->allDirections);
      foreach( itr, accessTiles )
      {
        Tile* tile2 = *itr;
         // for every neighbor tile
         if( tile2->isWalkable(_d->allLands))
         {
            // std::cout << "Next tile: " << tile2.getI() << ", " << tile2.getJ() << std::endl;

            if (_d->completedBranches.find( tile2 )==_d->completedBranches.end())
            {
               // the tile has not been processed yet
               PathwayPtr pathWay2( new Pathway( *pathWay.object() ) );
               pathWay2->drop();
               pathWay2->setNextTile( *tile2 );

               _d->activeBranches.insert(pathWay2);
               _d->completedBranches.insert(std::pair<Tile*, PathwayPtr>( tile2, pathWay2));

               // pathWay2.prettyPrint();
               // std::cout << "distance at:" << tile2.getI() << "," << tile2.getJ() << " is:" << pathWay2.getLength() << std::endl;
            }
         }
      }

      // this path is no longer active
      _d->activeBranches.erase(firstBranch);
   }
}

/*bool Propagator::getPath( RoadPtr destination, Pathway &oPathWay)
{
   std::map<Tile*, Pathway>::iterator mapIt;
   int distance = 30;
   while (true)
   {
      propagate(distance);

      mapIt = _d->completedBranches.find( &destination->getTile() );
      if (mapIt != _d->completedBranches.end())
      {
         // found pathWay!
         oPathWay = (*mapIt).second;
         return true;
      }

      // not found: try again
      if (_d->activeBranches.empty())
      {
         // no need to continue, no more active branches!
         return false;
      }

      distance = distance * 2;
   }
}*/

DirectRoutes Propagator::getRoutes(const TileOverlay::Type buildingType)
{
  DirectRoutes ret;
  // init the building list
  CityHelper helper( _d->city );
  ConstructionList constructionList = helper.find<Construction>( buildingType );

  // for each destination building
  foreach( it, constructionList )
  {
    ConstructionPtr destination = *it;
    std::set<PathwayPtr> destPath;  // paths to the current building, ordered by distance

    TilesArray destTiles = destination->getAccessRoads();
    foreach( tile, destTiles )
    {
      // searches path to that given tile
      Impl::RouteMap::iterator pathWayIt= _d->completedBranches.find( *tile );

      if( pathWayIt != _d->completedBranches.end() )
      {
        // we found a path!!!
        destPath.insert( pathWayIt->second );
      }
    }

    if (!destPath.empty())
    {
      // there is a path to that destination
      ret[ destination ] = *destPath.begin();
    }
  }

  return ret;
}

PathwayList Propagator::getWays(const int maxDistance)
{
  PathwayList oPathWayList;
  int nbLoops = 0;  // to detect infinite loops

  std::set<PathwayPtr>::iterator firstBranch;

  std::set< Tile* > markTiles;

  // propagate all branches
  while( !_d->activeBranches.empty() )
  {
    // while there are active branches

    // get an active branch
    firstBranch = _d->activeBranches.begin();

    PathwayPtr pathWay( new Pathway( *const_cast<PathwayPtr&>( *firstBranch ).object() ) );

    while( pathWay->getLength() < maxDistance)
    {
       // propagate branch until maxDistance is reached
       if ((nbLoops++)>100000)
       {
         Logger::warning("Infinite loop detected during propagation");
         return PathwayList();
       }

       const Tile& tile = pathWay->getDestination();
       // std::cout << "Propagation from tile " << tile.getI() << ", " << tile.getJ() << std::endl;

       // propagate to neighbour tiles
       TilesArray accessTiles = _d->tilemap->getRectangle( tile.getIJ() + TilePos( -1, -1 ),
                                                             tile.getIJ() + TilePos( 1, 1 ), _d->allDirections);

       // nextTiles = accessTiles - alreadyProcessedTiles
       TilesArray nextTiles;
       foreach( itr, accessTiles )
       {
         Tile* tile2 = *itr;
         // for every neighbour tile
         bool notResolved = (markTiles.find( tile2 ) == markTiles.end());
         if( tile2->isWalkable(_d->allLands) && !pathWay->contains( *tile2 ) && notResolved)
         {
           nextTiles.push_back( tile2 );
           markTiles.insert( tile2 );
         }
       }

       if (nextTiles.size() == 0)
       {
          // the current branch has been fully maximized
          break;
       }

       for( TilesArray::const_iterator itTile = nextTiles.begin(); itTile!=nextTiles.end(); ++itTile)
       {
          // for every neighbor tile
          Tile &tile2 = **itTile;
          // std::cout << "Next tile: " << tile2.getI() << ", " << tile2.getJ() << std::endl;

          if (*itTile == nextTiles.back())
          {
             // update the current branch
             // std::cout << "updated!" << std::endl;
             pathWay->setNextTile(tile2);
          }
          else
          {
             // copy the current branch
             // std::cout << "cloned!" << std::endl;
             PathwayPtr pathWay2( new Pathway( *pathWay.object() ) );
             pathWay2->drop();
             pathWay2->setNextTile(tile2);
             _d->activeBranches.insert( pathWay2 );
          }
       }

    }

    // the current branch has been fully maximized
    oPathWayList.push_back( pathWay );
    // pathWay.prettyPrint();

    // this branch is no longer active
    _d->activeBranches.erase( firstBranch );
  }

  return oPathWayList;
}

Propagator::~Propagator()
{

}

DirectRoute Propagator::getShortestRoute(const DirectRoutes& routes )
{
  DirectRoute ret;
  int minLength = 999;

  for( DirectRoutes::const_iterator pathWayIt= routes.begin();
       pathWayIt != routes.end(); ++pathWayIt)
  {
    // for every warehouse within range
    PathwayPtr pathWay = pathWayIt->second;

    if( pathWay->getLength() < minLength )
    {
      minLength = pathWay->getLength();
      ret = DirectRoute( pathWayIt->first, *pathWay.object() );
    }
  }

  return ret;
}

DirectRoute Propagator::getShortestRoute(const TileOverlay::Type buildingType )
{
  DirectRoutes routes = getRoutes( buildingType );

  return getShortestRoute( routes );
}
