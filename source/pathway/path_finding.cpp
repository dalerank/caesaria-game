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

#include "objects/construction.hpp"
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

using namespace gfx;
using namespace constants;

class Propagator::Impl
{
public:
  std::set<PathwayPtr> activeBranches;

  typedef std::map<Tile*, PathwayPtr> RouteMap;  

  RouteMap completedBranches;
  Propagator::ObsoleteOverlays obsoleteOvs;
  PlayerCityPtr city;

  Tile* origin;
  Tilemap* tilemap;
  bool allLands;  // true if can walk in all lands, false if limited to roads
  bool allDirections;  // true if can walk in all directions, false if limited to North/South/East/West
};

Propagator::Propagator(PlayerCityPtr city) : _d( new Impl )
{
  _d->city = city;
  _d->tilemap = &city->tilemap();
  _d->allLands = false;
  _d->allDirections = true;
}

void Propagator::setAllLands(const bool value) {   _d->allLands = value;}
void Propagator::setAllDirections(const bool value){  _d->allDirections = value;}
void Propagator::setObsoleteOverlay( TileOverlay::Type type){ _d->obsoleteOvs.insert( type ); }
void Propagator::init(TilePos origin){  init( _d->tilemap->at( origin ) );}

void Propagator::setObsoleteOverlays(const Propagator::ObsoleteOverlays& ovs)
{
  foreach( it, ovs ) { _d->obsoleteOvs.insert( *it ); }
}

void Propagator::init( ConstructionPtr origin)
{
   // init propagation on access roads
  _d->origin = &origin->tile();
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
  _d->obsoleteOvs.clear();

  // init propagation
  foreach( it, origin )
  {
    // std::cout << "Tile access " << tile.getI() << "," << tile.getJ() << std::endl;
    Tile* tile = *it;
    PathwayPtr pathWay( new Pathway() );
    pathWay->drop();
    pathWay->init( *tile );
    // init active branches
    _d->activeBranches.insert( pathWay );
    // init trivial completed branches
    _d->completedBranches.insert( std::pair<Tile*, PathwayPtr>( tile, pathWay ) );
  }
}

void Propagator::propagate(const unsigned int maxDistance)
{
   int nbLoops = 0;  // to detect infinite loops

   std::set<PathwayPtr>::iterator firstBranch;
   std::set<PathwayPtr>& activeBranches = _d->activeBranches;
   const ObsoleteOverlays& obsoleteOvs = _d->obsoleteOvs;

   // propagate on all tiles
   while (!activeBranches.empty())
   {
      // while there are active paths
      if ((nbLoops++)>10000)
      {
        Logger::warning( "WARNING!!!: Infinite loop detected during propagation");
        _d->completedBranches.clear();
        return;
      }

      // get the shortest active path
      firstBranch = activeBranches.begin();

      PathwayPtr pathWay = *firstBranch;
      const Tile& tile = pathWay->back();
      //std::cout << "Propagation from tile " << tile.getI() << ", " << tile.getJ() << std::endl;

      unsigned int tileLength = 1;
      if( pathWay->length() + tileLength > maxDistance)
      {
         // we processed all paths within range. stop the propagation
         //std::cout << "MaxDistance reached. stop propagation" << std::endl;
         break;
      }

      // propagate to neighbour tiles
      TilesArray accessTiles = _d->tilemap->getNeighbors(tile.pos(), _d->allDirections ? Tilemap::AllNeighbors : Tilemap::FourNeighbors);
      foreach( itr, accessTiles )
      {
        Tile* tile2 = *itr;
        // for every neighbor tile
        bool tileWalkable = tile2->isWalkable( _d->allLands );
        bool overlayWalkable = true;
        if( tile2->overlay().isValid() )
        {
          overlayWalkable = !obsoleteOvs.count( tile2->overlay()->type() );
        }

        if( tileWalkable && overlayWalkable )
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

DirectPRoutes Propagator::getRoutes(const TileOverlay::Type buildingType)
{
  DirectPRoutes ret;
  // init the building list
  city::Helper helper( _d->city );
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

PathwayList Propagator::getWays(const unsigned int maxDistance)
{
  PathwayList oPathWayList;
  int nbLoops = 0;  // to detect infinite loops

  std::set<PathwayPtr>::iterator firstBranch;

  std::set< Tile* > markTiles;
  const ObsoleteOverlays& obsoleteOvs = _d->obsoleteOvs;

  // propagate all branches
  while( !_d->activeBranches.empty() )
  {
    // while there are active branches

    // get an active branch
    firstBranch = _d->activeBranches.begin();

    PathwayPtr pathWay( new Pathway( *const_cast<PathwayPtr&>( *firstBranch ).object() ) );

    while( pathWay->length() < maxDistance)
    {
      // propagate branch until maxDistance is reached
      if ((nbLoops++)>100000)
      {
        Logger::warning("Infinite loop detected during propagation");
        return PathwayList();
      }

      const Tile& tile = pathWay->back();
      // std::cout << "Propagation from tile " << tile.getI() << ", " << tile.getJ() << std::endl;

      // propagate to neighbour tiles
      TilesArray accessTiles = _d->tilemap->getNeighbors(tile.pos(), _d->allDirections ? Tilemap::AllNeighbors : Tilemap::FourNeighbors);

      // nextTiles = accessTiles - alreadyProcessedTiles
      TilesArray nextTiles;
      foreach( itr, accessTiles )
      {
        Tile* tile2 = *itr;
        // for every neighbour tile
        bool notResolved = (markTiles.find( tile2 ) == markTiles.end());

        bool tileWalkable = tile2->isWalkable( _d->allLands );
        bool overlayWalkable = true;
        if( tile2->overlay().isValid() )
        {
          overlayWalkable = !obsoleteOvs.count( tile2->overlay()->type() );
        }

        if( tileWalkable && overlayWalkable && !pathWay->contains( *tile2 ) && notResolved)
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

      foreach( itTile, nextTiles )
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
    if( pathWay->length() )
    {
      oPathWayList.push_back( pathWay );
    }
    // pathWay.prettyPrint();

    // this branch is no longer active
    _d->activeBranches.erase( firstBranch );
  }

  return oPathWayList;
}

Propagator::~Propagator(){}

DirectRoute Propagator::getShortestRoute(const DirectPRoutes& routes )
{
  DirectRoute ret;
  unsigned int minLength = 999;

  foreach( pathWayIt, routes )
  {
    // for every warehouse within range
    PathwayPtr pathWay = pathWayIt->second;

    if( pathWay->length() < minLength )
    {
      minLength = pathWay->length();
      ret = DirectRoute( pathWayIt->first, *pathWay.object() );
    }
  }

  return ret;
}

DirectRoute Propagator::getShortestRoute(const TileOverlay::Type buildingType )
{
  DirectPRoutes routes = getRoutes( buildingType );

  return getShortestRoute( routes );
}
