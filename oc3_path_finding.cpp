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
#include "oc3_scenario.hpp"
#include "oc3_exception.hpp"
#include "oc3_positioni.hpp"
#include "oc3_road.hpp"
#include "oc3_tile.hpp"
#include "oc3_variant.hpp"

#include <iterator>

bool operator<(const PathWay &v1, const PathWay &v2)
{
   if (v1.getLength()!=v2.getLength())
   {
      return v1.getLength() < v2.getLength();
   }

   // compare memory address
   return (&v1 < &v2);
}


PathWay::PathWay()
{
   _origin = NULL;
   _destination = TilePos( 0, 0 );
   _isReverse = false;
}

PathWay::PathWay(const PathWay &copy)
{
   *this = copy;
}

PathWay::PathWay( Tilemap& tmap, const TilePos& startPos, const TilePos& stopPos, 
                  FindType type/*=roadOnly */ )
{
  _OC3_DEBUG_BREAK_IF( true && "broken constructor" );
}

void PathWay::init(Tilemap &tilemap, Tile &origin)
{
   _tilemap = &tilemap;
   _origin = &origin;
   _destination = origin.getIJ();
   _directionList.clear();
   _directionIt = _directionList.begin();
   _directionIt_reverse = _directionList.rbegin();
   _tileList.clear();
   _tileList.push_back(&origin);
}

int PathWay::getLength() const
{
   // TODO: various lands have various travel time (road easier to travel than open country)
   return _directionList.size();
}

Tile &PathWay::getOrigin() const
{
   return *_origin;
}

Tile &PathWay::getDestination() const
{
   Tile &res = _tilemap->at( _destination );
   return res;
}

bool PathWay::isReverse() const
{
   return _isReverse;
}

void PathWay::begin()
{
   _directionIt = _directionList.begin();
   _isReverse = false;
}

void PathWay::rbegin()
{
   _directionIt_reverse = _directionList.rbegin();
   _isReverse = true;
}

void PathWay::toggleDirection()
{
  if( _isReverse )
  {
    _isReverse = false;
    _directionIt = _directionIt_reverse.base();
  }
  else
  {
    _isReverse = true;
    _directionIt_reverse = Directions::reverse_iterator( _directionIt );
  }
}

DirectionType PathWay::getNextDirection()
{
   DirectionType res;
   if (_isReverse)
   {
      if (_directionIt_reverse == _directionList.rend())
      {
         // end of path!
         return D_NONE;
      }
      int direction = (int) *_directionIt_reverse;
      if (direction != (int) D_NONE)
      {
         if (direction + 4 < (int) D_MAX)
         {
            res = (DirectionType) (direction+4);
         }
         else
         {
            res = (DirectionType) (direction-4);
         }
      }
      _directionIt_reverse ++;
   }
   else
   {
      if (_directionIt == _directionList.end())
      {
         // end of path!
         return D_NONE;
      }
      res = *_directionIt;
      _directionIt ++;
   }


   return res;
}

bool PathWay::isDestination() const
{
   bool res;
   if (_isReverse)
   {
#if defined(_WIN32)
	  std::vector<DirectionType>::const_reverse_iterator convItReverse = _directionIt_reverse;
	  res = (convItReverse == _directionList.rend());
#else
      res = (_directionIt_reverse == _directionList.rend());
#endif
   }
   else
   {
      res = (_directionIt == _directionList.end());
   }

   return res;
}

void PathWay::setNextDirection(const DirectionType direction)
{
   switch (direction)
   {
   case D_NORTH:
      _destination += TilePos( 0, 1 );
      break;
   case D_NORTH_EAST:
      _destination += TilePos( 1, 1 );
      break;
   case D_EAST:
      _destination += TilePos( 1, 0 );
      break;
   case D_SOUTH_EAST:
      _destination += TilePos( 1, -1 );
      break;
   case D_SOUTH:
      _destination += TilePos( 0, -1 );
      break;
   case D_SOUTH_WEST:
      _destination += TilePos( -1, -1 );
      break;
   case D_WEST:
      _destination += TilePos( -1, 0 );
      break;
   case D_NORTH_WEST:
      _destination += TilePos( -1, 1 );
      break;
   default:
      THROW("Unexpected Direction:" << direction);
      break;
   }

   if (! _tilemap->isInside( TilePos( _destination ) ))
   {
      THROW("Destination is out of range");
   }

   _tileList.push_back( &_tilemap->at( _destination ));

   _directionList.push_back(direction);
}

void PathWay::setNextTile( const Tile& tile)
{
   int dI = tile.getI() - _destination.getI();
   int dJ = tile.getJ() - _destination.getJ();

   DirectionType direction;

   if (dI==0 && dJ==0)
   {
      direction = D_NONE;
   }
   else if (dI==0 && dJ==1)
   {
      direction = D_NORTH;
   }
   else if (dI==1 && dJ==1)
   {
      direction = D_NORTH_EAST;
   }
   else if (dI==1 && dJ==0)
   {
      direction = D_EAST;
   }
   else if (dI==1 && dJ==-1)
   {
      direction = D_SOUTH_EAST;
   }
   else if (dI==0 && dJ==-1)
   {
      direction = D_SOUTH;
   }
   else if (dI==-1 && dJ==-1)
   {
      direction = D_SOUTH_WEST;
   }
   else if (dI==-1 && dJ==0)
   {
      direction = D_WEST;
   }
   else if (dI==-1 && dJ==1)
   {
      direction = D_NORTH_WEST;
   }
   else
   {
      THROW("Unexpected tile, deltaI:" << dI << " deltaJ:" << dJ);
   }

   setNextDirection(direction);
}

bool PathWay::contains(Tile &tile)
{
   // search in reverse direction, because usually the last tile matches
   bool res = false;
   for (PtrTilesList::reverse_iterator itTile = _tileList.rbegin(); itTile != _tileList.rend(); ++itTile)
   {
      if (*itTile == &tile)
      {
         res = true;
         break;
      }
   }

   return res;
}

PtrTilesList& PathWay::getAllTiles()
{
   return _tileList;
}

void PathWay::prettyPrint() const
{
   if (_origin == NULL)
   {
      std::cout << "pathWay is NULL" << std::endl;
   }
   else
   {
      std::cout << "pathWay from (" << _origin->getI() << ", " << _origin->getJ() 
                 << ") to (" << _destination.getI() << ", " << _destination.getJ() << "): ";
      for (std::vector<DirectionType>::const_iterator itDir = _directionList.begin(); itDir != _directionList.end(); ++itDir)
      {
         DirectionType direction = *itDir;
         std::string strDir = "";
         switch (direction)
         {
         case D_NORTH:
            strDir = "N";
            break;
         case D_NORTH_EAST:
            strDir = "NE";
            break;
         case D_EAST:
            strDir = "E";
            break;
         case D_SOUTH_EAST:
            strDir = "SE";
            break;
         case D_SOUTH:
            strDir = "S";
            break;
         case D_SOUTH_WEST:
            strDir = "SW";
            break;
         case D_WEST:
            strDir = "W";
            break;
         case D_NORTH_WEST:
            strDir = "NW";
            break;
         default:
            THROW("Unexpected Direction:" << direction);
            break;
         }
         std::cout << strDir << " ";
      }
      std::cout << std::endl;
   }
}

void PathWay::save( VariantMap& stream) const
{
  stream[ "startPos" ] = _origin->getIJ();
  stream[ "stopPos" ] = _destination;

  VariantList directions;
  for( Directions::const_iterator itDir = _directionList.begin(); itDir != _directionList.end(); ++itDir)
  {
    directions.push_back( (int)*itDir);
  }
  stream[ "directions" ] = directions;
  stream[ "reverse" ] = _isReverse;
  stream[ "step" ] = getStep();
}

void PathWay::load( const VariantMap& stream )
{
  _tilemap = &Scenario::instance().getCity().getTilemap();
  _origin = &_tilemap->at( stream.get( "startPos" ).toTilePos() );
  _destination = _origin->getIJ();//stream.get( "stopPos" ).toTilePos();
  VariantList directions = stream.get( "directions" ).toList();
  for( VariantList::iterator it = directions.begin(); it != directions.end(); it++ )
  {
     DirectionType dir = (DirectionType)(*it).toInt();
     setNextDirection( dir );
  }
  _isReverse = stream.get( "reverse" ).toBool();
  int off = stream.get( "step" ).toInt();
  _directionIt = _directionList.begin();
  _directionIt_reverse = _directionList.rbegin();
  std::advance(_directionIt_reverse, off);
  std::advance(_directionIt, off);
}

PathWay& PathWay::operator=( const PathWay& other )
{
  _tilemap             = other._tilemap;
  _origin              = other._origin;
  _destination         = other._destination;
  _directionList       = other._directionList;
  _directionIt         = _directionList.begin();
  _directionIt_reverse = _directionList.rbegin();
  _tileList            = other._tileList;

  return *this;
}

unsigned int PathWay::getStep() const
{
  if (_isReverse)
  {
    size_t pos = std::distance<Directions::const_reverse_iterator>(_directionList.rbegin(), _directionIt_reverse);
    return static_cast<unsigned int>(pos);
  }
  else
  {
    size_t pos = std::distance<Directions::const_iterator>(_directionList.begin(), _directionIt);
    return static_cast<unsigned int>(pos);
  }
}

Propagator::Propagator()
{
   _city = &Scenario::instance().getCity();
   _tilemap = &_city->getTilemap();
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


void Propagator::init(const Construction &origin)
{
   // init propagation on access roads
  _origin = &origin.getTile();
   init(origin.getAccessRoads());
}

void Propagator::init(Tile& origin)
{
  _origin = &origin;
   std::list<Tile*> tileList;
   tileList.push_back(&origin);

   init( tileList );
}

void Propagator::init(const std::list<Tile*>& origin)
{
   _activeBranches.clear();
   _completedBranches.clear();

   // init propagation
   for (std::list<Tile*>::const_iterator itTile = origin.begin(); itTile!=origin.end(); ++itTile)
   {
      // for every access tile
      Tile &tile = **itTile;
      // std::cout << "Tile access " << tile.getI() << "," << tile.getJ() << std::endl;

      PathWay pathWay;
      pathWay.init(*_tilemap, tile);

      // init active branches
      _activeBranches.insert(pathWay);

      // init trivial completed branches
      _completedBranches.insert(std::pair<Tile*, PathWay>(&tile, pathWay));
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
      Tile& tile = pathWay.getDestination();
      //std::cout << "Propagation from tile " << tile.getI() << ", " << tile.getJ() << std::endl;

      int tileLength = 1;
      if (pathWay.getLength() + tileLength > maxDistance)
      {
         // we processed all paths within range. stop the propagation
         //std::cout << "MaxDistance reached. stop propagation" << std::endl;
         break;
      }

      // propagate to neighbour tiles
      const std::list<Tile*> accessTiles = _tilemap->getRectangle( tile.getIJ() + TilePos( -1,-1 ),
                                                                   tile.getIJ() + TilePos( 1, 1 ), _allDirections);
      for (std::list<Tile*>::const_iterator itTile = accessTiles.begin(); itTile!=accessTiles.end(); ++itTile)
      {
         // for every neighbor tile
         Tile &tile2 = **itTile;
         if (tile2.getTerrain().isWalkable(_allLands))
         {
            // std::cout << "Next tile: " << tile2.getI() << ", " << tile2.getJ() << std::endl;

            if (_completedBranches.find(&tile2)==_completedBranches.end())
            {
               // the tile has not been processed yet
               PathWay pathWay2(pathWay);
               pathWay2.setNextTile(tile2);
               _activeBranches.insert(pathWay2);
               _completedBranches.insert(std::pair<Tile*, PathWay>(&tile2, pathWay2));

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

void Propagator::getReachedBuildings(const BuildingType buildingType, ReachedBuldings& oPathWayList)
{
   // init the building list
   Buildings buildingList;
   LandOverlays overlayList = _city->getBuildingList(buildingType);
   for ( LandOverlays::iterator overlayIt= overlayList.begin(); overlayIt!= overlayList.end(); ++overlayIt)
   {
      LandOverlayPtr overlay= *overlayIt;
      BuildingPtr building= overlay.as<Building>();
      if( building.isValid() )
        buildingList.push_back(building);
   }

   for ( Buildings::iterator itDest = buildingList.begin(); itDest != buildingList.end(); ++itDest)
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

      if (! destPath.empty())
      {
         // there is a path to that destination
         const PathWay& bestPath = *destPath.begin();
         oPathWayList.insert( std::make_pair( destination, bestPath) );
      }
   }
}

void Propagator::getAllPaths(const int maxDistance, std::list<PathWay> &oPathWayList)
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

         Tile& tile = pathWay.getDestination();
         // std::cout << "Propagation from tile " << tile.getI() << ", " << tile.getJ() << std::endl;

         // propagate to neighbour tiles
         const PtrTilesList accessTiles = _tilemap->getRectangle( tile.getIJ() + TilePos( -1, -1 ), 
                                                                      tile.getIJ() + TilePos( 1, 1 ), _allDirections);

         // nextTiles = accessTiles - alreadyProcessedTiles
         PtrTilesList nextTiles;
         for( PtrTilesList::const_iterator itTile = accessTiles.begin(); itTile!=accessTiles.end(); ++itTile)
         {
            // for every neighbour tile
            Tile* tile2 = *itTile;

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