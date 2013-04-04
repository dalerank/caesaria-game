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



#include <path_finding.hpp>

#include <tilemap.hpp>
#include <city.hpp>
#include <scenario.hpp>
#include <exception.hpp>

#include <iostream>

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
   _destinationI = 0;
   _destinationJ = 0;
   _isReverse = false;
}

PathWay::PathWay(const PathWay &copy)
{
   _tilemap = copy._tilemap;
   _origin = copy._origin;
   _destinationI = copy._destinationI;
   _destinationJ = copy._destinationJ;
   _directionList = copy._directionList;
   _directionIt = _directionList.begin();
   _directionIt_reverse = _directionList.rbegin();
   _tileList = copy._tileList;
}

void PathWay::init(Tilemap &tilemap, Tile &origin)
{
   _tilemap = &tilemap;
   _origin = &origin;
   _destinationI = origin.getI();
   _destinationJ = origin.getJ();
   _directionList.clear();
   _directionIt = _directionList.begin();
   _directionIt_reverse = _directionList.rbegin();
   _tileList.clear();
   _tileList.push_back(&origin);
}

int PathWay::getLength() const
{
   return _directionList.size();
}

Tile &PathWay::getOrigin() const
{
   return *_origin;
}

Tile &PathWay::getDestination() const
{
   Tile &res = _tilemap->at(_destinationI, _destinationJ);
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
      _destinationJ += 1;
      break;
   case D_NORTH_EAST:
      _destinationI += 1;
      _destinationJ += 1;
      break;
   case D_EAST:
      _destinationI += 1;
      break;
   case D_SOUTH_EAST:
      _destinationI += 1;
      _destinationJ -= 1;
      break;
   case D_SOUTH:
      _destinationJ -= 1;
      break;
   case D_SOUTH_WEST:
      _destinationI -= 1;
      _destinationJ -= 1;
      break;
   case D_WEST:
      _destinationI -= 1;
      break;
   case D_NORTH_WEST:
      _destinationI -= 1;
      _destinationJ += 1;
      break;
   default:
      THROW("Unexpected Direction:" << direction);
      break;
   }

   if (! _tilemap->is_inside(_destinationI, _destinationJ))
   {
      THROW("Destination is out of range");
   }

   _tileList.push_back(& _tilemap->at(_destinationI, _destinationJ));

   _directionList.push_back(direction);
}

void PathWay::setNextTile(Tile &tile)
{
   int dI = tile.getI() - _destinationI;
   int dJ = tile.getJ() - _destinationJ;

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
   for (std::list<Tile*>::reverse_iterator itTile = _tileList.rbegin(); itTile != _tileList.rend(); ++itTile)
   {
      if (*itTile == &tile)
      {
         res = true;
         break;
      }
   }

   return res;
}

std::list<Tile*>& PathWay::getAllTiles()
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
      std::cout << "pathWay from (" << _origin->getI() << ", " << _origin->getJ() << ") to (" << _destinationI << ", " << _destinationJ << "): ";
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

void PathWay::serialize(OutputSerialStream &stream)
{
   stream.write_int(_origin->getI(), 2, 0, 1000);
   stream.write_int(_origin->getJ(), 2, 0, 1000);
   stream.write_int(_destinationI, 2, 0, 1000);
   stream.write_int(_destinationJ, 2, 0, 1000);
   stream.write_int(_directionList.size(), 2, 0, 65535);
   for (std::vector<DirectionType>::iterator itDir = _directionList.begin(); itDir != _directionList.end(); ++itDir)
   {
      DirectionType dir = *itDir;
      stream.write_int((int) dir, 1, 0, D_MAX);
   }
   stream.write_int(_isReverse, 1, 0, 1);
   if (_isReverse)
   {
      stream.write_int(std::distance(_directionList.rbegin(), _directionIt_reverse), 2, 0, 65535);
   }
   else
   {
      stream.write_int(std::distance(_directionList.begin(), _directionIt), 2, 0, 65535);
   }
}

void PathWay::unserialize(InputSerialStream &stream)
{
   int originI = stream.read_int(2, 0, 1000);
   int originJ = stream.read_int(2, 0, 1000);
   _tilemap = &Scenario::instance().getCity().getTilemap();
   _origin = &_tilemap->at(originI, originJ);
   _destinationI = stream.read_int(2, 0, 1000);
   _destinationJ = stream.read_int(2, 0, 1000);
   int size = stream.read_int(2, 0, 65535);
   for (int i = 0; i<size; ++i)
   {
      DirectionType dir = (DirectionType) stream.read_int(1, 0, D_MAX);
      _directionList.push_back(dir);
   }
   _isReverse = stream.read_int(1, 0, 1) > 0;
   int off = stream.read_int(2, 0, 65535);
   _directionIt = _directionList.begin();
   _directionIt_reverse = _directionList.rbegin();
   std::advance(_directionIt_reverse, off);
   std::advance(_directionIt, off);
}

Propagator::Propagator()
{
   _city = &Scenario::instance().getCity();
   _tilemap = &_city->getTilemap();
}


void Propagator::init(Construction &origin)
{
   std::list<Road*> accessRoads;

   // init propagation on access roads
   const std::list<Tile*>& accessTiles = origin.getAccessRoads();
   for (std::list<Tile*>::const_iterator itTile = accessTiles.begin(); itTile!=accessTiles.end(); ++itTile)
   {
      // for every access road
      Tile &tile = **itTile;
      //std::cout << "road access " << tile.getI() << "," << tile.getJ() << std::endl;
      Road &road = dynamic_cast<Road&> (*tile.get_terrain().getOverlay());

      accessRoads.push_back(&road);
   }

   init(accessRoads);
}


void Propagator::init(Road &origin)
{
   std::list<Road*> roadList;
   roadList.push_back(&origin);

   init(roadList);
}


void Propagator::init(std::list<Road*> &origin)
{
   _activeBranches.clear();
   _completedBranches.clear();

   // init propagation on access roads
   for (std::list<Road*>::const_iterator itRoad = origin.begin(); itRoad!=origin.end(); ++itRoad)
   {
      // for every access road
      Road &road = **itRoad;
      //std::cout << "road access " << tile.getI() << "," << tile.getJ() << std::endl;

      PathWay pathWay;
      pathWay.init(*_tilemap, road.getTile());

      // init active branches
      _activeBranches.insert(pathWay);

      // init trivial completed branches
      _completedBranches.insert(std::pair<Road*, PathWay>(&road, pathWay));
   }
}


void Propagator::propagate(const int maxDistance)
{
   int nbLoops = 0;  // to detect infinite loops

   std::set<PathWay>::iterator firstBranch;

   // propagate on all roads
   while (!_activeBranches.empty())
   {
      // while there are active roads
      if ((nbLoops++)>100000) THROW("Infinite loop detected during propagation");

      // get the lowest_distance active road
      firstBranch = _activeBranches.begin();

      const PathWay &pathWay = *firstBranch;
      Tile& tile = pathWay.getDestination();
      Road& road = dynamic_cast<Road&> (*tile.get_terrain().getOverlay());
      //std::cout << "Propagation from tile " << tile.getI() << ", " << tile.getJ() << std::endl;

      int roadLength = 1;
      if (pathWay.getLength() + roadLength > maxDistance)
      {
         // we processed all roads within range. stop the propagation
         //std::cout << "MaxDistance reached. stop propagation" << std::endl;
         break;
      }

      // propagate to neighbour roads
      const std::list<Tile*>& accessTiles = road.getAccessRoads();
      for (std::list<Tile*>::const_iterator itTile = accessTiles.begin(); itTile!=accessTiles.end(); ++itTile)
      {
         // for every access road
         Tile &tile2 = **itTile;
         // std::cout << "Next tile: " << tile2.getI() << ", " << tile2.getJ() << std::endl;

         Road &road2 = dynamic_cast<Road&> (*tile2.get_terrain().getOverlay());
         if (_completedBranches.find(&road2)==_completedBranches.end())
         {
            // the road has not been processed yet
            PathWay pathWay2(pathWay);
            pathWay2.setNextTile(tile2);
            _activeBranches.insert(pathWay2);
            _completedBranches.insert(std::pair<Road*, PathWay>(&road2, pathWay2));

            // pathWay2.prettyPrint();
            // std::cout << "distance at:" << tile2.getI() << "," << tile2.getJ() << " is:" << pathWay2.getLength() << std::endl;
         }
      }

      // this road is no longer active
      _activeBranches.erase(firstBranch);
   }
}

bool Propagator::getPath(Road &destination, PathWay &oPathWay)
{
   std::map<Road*, PathWay>::iterator mapIt;
   int distance = 30;
   while (true)
   {
      propagate(distance);

      mapIt = _completedBranches.find(&destination);
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


bool Propagator::getPath(Building &destination, PathWay &oPathWay)
{
   const std::list<Tile*>& destTiles = destination.getAccessRoads();
   std::map<Road*, PathWay>::iterator mapIt;
   std::set<PathWay> destPath;  // paths to the destination building, ordered by distance
   int distance = 30;
   while (true)
   {
      propagate(distance);

      // searches reached destTiles
      for (std::list<Tile*>::const_iterator itTile= destTiles.begin(); itTile != destTiles.end(); ++itTile)
      {
         // for each destination tile
         Tile &tile= **itTile;
         Road &road = dynamic_cast<Road&>( *tile.get_terrain().getOverlay() );

         // searches path to that given tile
         mapIt = _completedBranches.find(&road);

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


void Propagator::getReachedRoads(std::map<Road*, PathWay> &oPathWayList)
{
   oPathWayList = _completedBranches;
}


void Propagator::getReachedBuildings(const BuildingType buildingType, std::map<Building*, PathWay> &oPathWayList)
{
   // init the building list
   std::list<Building*> buildingList;
   std::list<LandOverlay*> overlayList = _city->getBuildingList(buildingType);
   for (std::list<LandOverlay*>::iterator overlayIt= overlayList.begin(); overlayIt!= overlayList.end(); ++overlayIt)
   {
      LandOverlay* overlay= *overlayIt;
      Building* building= dynamic_cast<Building*>(overlay);
      buildingList.push_back(building);
   }

   for (std::list<Building*>::iterator itDest = buildingList.begin(); itDest != buildingList.end(); ++itDest)
   {
      // for each destination building
      Building &destination = **itDest;

      std::set<PathWay> destPath;  // paths to the current building, ordered by distance

      std::list<Tile*> destTiles= destination.getAccessRoads();
      for (std::list<Tile*>::iterator itTile= destTiles.begin(); itTile != destTiles.end(); ++itTile)
      {
         // for each destination tile
         Tile &tile= **itTile;
         Road &road = dynamic_cast<Road&> (*tile.get_terrain().getOverlay());

         // searches path to that given tile
         std::map<Road*, PathWay>::iterator pathWayIt= _completedBranches.find(&road);

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
         oPathWayList.insert(std::make_pair(&destination, bestPath));
      }
   }

}


void Propagator::getAllPaths(const int maxDistance, std::list<PathWay> &oPathWayList)
{
   // cannot use propagate function
   oPathWayList.clear();

   int nbLoops = 0;  // to detect infinite loops

   std::set<PathWay>::iterator firstBranch;

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
         Road& road = dynamic_cast<Road&> (*tile.get_terrain().getOverlay());
         // std::cout << "Propagation from tile " << tile.getI() << ", " << tile.getJ() << std::endl;

         // propagate to neighbour roads
         const std::list<Tile*>& accessTiles = road.getAccessRoads();

         // nextTiles = accessTiles - alreadyProcessedTiles
         std::list<Tile*> nextTiles;
         for (std::list<Tile*>::const_iterator itTile = accessTiles.begin(); itTile!=accessTiles.end(); ++itTile)
         {
            // for every access road
            Tile &tile2 = **itTile;

            if (! pathWay.contains(tile2) )
            {
               nextTiles.push_back(&tile2);
            }
         }

         if (nextTiles.size() == 0)
         {
            // the current branch has been fully maximized
            break;
         }

         for (std::list<Tile*>::const_iterator itTile = nextTiles.begin(); itTile!=nextTiles.end(); ++itTile)
         {
            // for every next road
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

