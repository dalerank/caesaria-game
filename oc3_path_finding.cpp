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
//
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com

#include "oc3_path_finding.hpp"

#include "oc3_tilemap.hpp"
#include "oc3_city.hpp"
#include "oc3_exception.hpp"
#include "oc3_positioni.hpp"
#include "oc3_road.hpp"
#include "oc3_tile.hpp"
#include "oc3_variant.hpp"

#include <iterator>

class Propagator::Impl
{
public:
  CityPtr city;
};


Propagator::Propagator( CityPtr city ) : _d( new Impl )
{
   _d->city = city;
   _tilemap = &city->getTilemap();
   _allLands = false;
   _allDirections = true;
}

void Propagator::setAllLands(const bool value)
{
   _allLands = value;
}

void Propagator::setAllDirections(const bool value)
{
   _allDirections = value;
}


void Propagator::init( ConstructionPtr origin)
{
   // init propagation on access roads
  _origin = &origin->getTile();
  init( origin->getAccessRoads() );
}

void Propagator::init(Tile& origin)
{
  _origin = &origin;
   std::list<Tile*> tileList;
   tileList.push_back(&origin);

   init( tileList );
}

void Propagator::init( const PtrTilesList& origin)
{
  _activeBranches.clear();
  _completedBranches.clear();

  // init propagation
  for( PtrTilesList::const_iterator it=origin.begin(); it != origin.end(); it++ )
  {
    // std::cout << "Tile access " << tile.getI() << "," << tile.getJ() << std::endl;
    Tile* tile = *it;
    PathWay pathWay;
    pathWay.init(*_tilemap, *tile);
    // init active branches
    _activeBranches.insert(pathWay);
    // init trivial completed branches
    _completedBranches.insert( std::pair<Tile*, PathWay>( tile, pathWay ) );
  }
}

void Propagator::propagate(const int maxDistance)
{
   int nbLoops = 0;  // to detect infinite loops

   std::set<PathWay>::iterator firstBranch;

   // propagate on all tiles
   while (!_activeBranches.empty())
   {
      // while there are active paths
      if ((nbLoops++)>10000) THROW("Infinite loop detected during propagation");

      // get the shortest active path
      firstBranch = _activeBranches.begin();

      const PathWay &pathWay = *firstBranch;
      const Tile& tile = pathWay.getDestination();
      //std::cout << "Propagation from tile " << tile.getI() << ", " << tile.getJ() << std::endl;

      int tileLength = 1;
      if (pathWay.getLength() + tileLength > maxDistance)
      {
         // we processed all paths within range. stop the propagation
         //std::cout << "MaxDistance reached. stop propagation" << std::endl;
         break;
      }

      // propagate to neighbour tiles
      PtrTilesList accessTiles = _tilemap->getRectangle( tile.getIJ() + TilePos( -1,-1 ),
                                                         tile.getIJ() + TilePos( 1, 1 ), _allDirections);
      foreach( Tile* tile2, accessTiles )
      {
         // for every neighbor tile
         if( tile2->getTerrain().isWalkable(_allLands))
         {
            // std::cout << "Next tile: " << tile2.getI() << ", " << tile2.getJ() << std::endl;

            if (_completedBranches.find( tile2 )==_completedBranches.end())
            {
               // the tile has not been processed yet
               PathWay pathWay2(pathWay);
               pathWay2.setNextTile( *tile2 );
               _activeBranches.insert(pathWay2);
               _completedBranches.insert(std::pair<Tile*, PathWay>( tile2, pathWay2));

               // pathWay2.prettyPrint();
               // std::cout << "distance at:" << tile2.getI() << "," << tile2.getJ() << " is:" << pathWay2.getLength() << std::endl;
            }
         }
      }

      // this path is no longer active
      _activeBranches.erase(firstBranch);
   }
}

bool Propagator::getPath( RoadPtr destination, PathWay &oPathWay)
{
   std::map<Tile*, PathWay>::iterator mapIt;
   int distance = 30;
   while (true)
   {
      propagate(distance);

      mapIt = _completedBranches.find( &destination->getTile() );
      if (mapIt != _completedBranches.end())
      {
         // found pathWay!
         oPathWay = (*mapIt).second;
         return true;
      }

      // not found: try again
      if (_activeBranches.empty())
      {
         // no need to continue, no more active branches!
         return false;
      }

      distance = distance * 2;
   }
}


bool Propagator::getPath( BuildingPtr destination, PathWay &oPathWay)
{
   const PtrTilesList& destTiles = destination->getAccessRoads();
   std::map<Tile*, PathWay>::iterator mapIt;
   std::set<PathWay> destPath;  // paths to the destination building, ordered by distance
   int distance = 30;
   while (true)
   {
      propagate(distance);

      // searches reached destTiles
      for (PtrTilesList::const_iterator itTile= destTiles.begin(); itTile != destTiles.end(); ++itTile)
      {
         // for each destination tile
         Tile &tile= **itTile;

         // searches path to that given tile
         mapIt = _completedBranches.find(&tile);

         if (mapIt != _completedBranches.end())
         {
            // we found a path!
            PathWay &path= (*mapIt).second;
            destPath.insert(path);
         }
      }

      if (! destPath.empty())
      {
         // there is a path to that building
         oPathWay = *destPath.begin();
         return true;
      }

      // not found: try again
      if (_activeBranches.empty())
      {
         // no need to continue, no more active branches!
         return false;
      }

      distance = distance * 2;
   }
}

void Propagator::getRoutes( const BuildingType buildingType, Routes& oPathWayList)
{
  // init the building list
  CityHelper helper( _d->city );
  Buildings buildingList = helper.getBuildings<Building>( buildingType );

  for( Buildings::iterator itDest = buildingList.begin(); itDest != buildingList.end(); ++itDest )
  {
     // for each destination building
     BuildingPtr destination = *itDest;

     std::set<PathWay> destPath;  // paths to the current building, ordered by distance

     PtrTilesList destTiles= destination->getAccessRoads();
     for( PtrTilesList::iterator itTile= destTiles.begin(); itTile != destTiles.end(); ++itTile)
     {
        // for each destination tile
        Tile &tile= **itTile;

        // searches path to that given tile
        std::map<Tile*, PathWay>::iterator pathWayIt= _completedBranches.find(&tile);

        if (pathWayIt != _completedBranches.end())
        {
           // we found a path!
           PathWay &path= (*pathWayIt).second;
           destPath.insert(path);
        }
     }

     if (!destPath.empty())
     {
        // there is a path to that destination
        oPathWayList[ destination ] = *destPath.begin();
     }
  }
}

void Propagator::getWays(const int maxDistance, std::list<PathWay> &oPathWayList)
{
   // cannot use propagate function
   oPathWayList.clear();

   int nbLoops = 0;  // to detect infinite loops

   std::set<PathWay>::iterator firstBranch;

   std::set< Tile* > markTiles;

   // propagate all branches
   while (!_activeBranches.empty())
   {
      // while there are active branches

      // get an active branch
      firstBranch = _activeBranches.begin();

      PathWay pathWay = *firstBranch;

      while (pathWay.getLength() < maxDistance)
      {
         // propagate branch until maxDistance is reached
         if ((nbLoops++)>100000) THROW("Infinite loop detected during propagation");

         const Tile& tile = pathWay.getDestination();
         // std::cout << "Propagation from tile " << tile.getI() << ", " << tile.getJ() << std::endl;

         // propagate to neighbour tiles
         PtrTilesList accessTiles = _tilemap->getRectangle( tile.getIJ() + TilePos( -1, -1 ),
                                                            tile.getIJ() + TilePos( 1, 1 ), _allDirections);

         // nextTiles = accessTiles - alreadyProcessedTiles
         PtrTilesList nextTiles;
         foreach( Tile* tile2, accessTiles )
         {
            // for every neighbour tile
            bool notResolved = (markTiles.find( tile2 ) == markTiles.end());
            if (tile2->getTerrain().isWalkable(_allLands) && !pathWay.contains( *tile2 ) && notResolved)
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

         for (PtrTilesList::const_iterator itTile = nextTiles.begin(); itTile!=nextTiles.end(); ++itTile)
         {
            // for every neighbor tile
            Tile &tile2 = **itTile;
            // std::cout << "Next tile: " << tile2.getI() << ", " << tile2.getJ() << std::endl;

            if (*itTile == nextTiles.back())
            {
               // update the current branch
               // std::cout << "updated!" << std::endl;
               pathWay.setNextTile(tile2);
            }
            else
            {
               // copy the current branch
               // std::cout << "cloned!" << std::endl;
               PathWay pathWay2(pathWay);
               pathWay2.setNextTile(tile2);
               _activeBranches.insert(pathWay2);
            }
         }

      }

      // the current branch has been fully maximized
      oPathWayList.push_back(pathWay);
      // pathWay.prettyPrint();

      // this branch is no longer active
      _activeBranches.erase(firstBranch);
   }
}

Propagator::~Propagator()
{

}

Propagator::DirectRoute Propagator::getShortestRoute( const Routes& routes )
{
  DirectRoute ret;
  int minLength = 999;

  for( Propagator::Routes::const_iterator pathWayIt= routes.begin(); 
       pathWayIt != routes.end(); ++pathWayIt)
  {
    // for every warehouse within range
    const PathWay& pathWay= pathWayIt->second;

    if( pathWay.getLength() < minLength )
    {
      minLength = pathWay.getLength();
      ret.first = pathWayIt->first;
      ret.second = pathWay;
    }
  }

  return ret;
}

Propagator::DirectRoute Propagator::getShortestRoute( const BuildingType buildingType )
{
  Routes routes;
  getRoutes( buildingType, routes );

  return getShortestRoute( routes );
}
