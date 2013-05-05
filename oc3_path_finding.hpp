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


#ifndef PATH_FINDING_HPP
#define PATH_FINDING_HPP


#include "oc3_building.hpp"
#include "oc3_enums.hpp"
#include "oc3_serializer.hpp"
#include "oc3_predefinitions.hpp"
#include <list>

class PathWay : public Serializable
{
public:
   PathWay();
   PathWay(const PathWay &copy);
   void init(Tilemap &tilemap, Tile &origin);

   int getLength() const;
   Tile &getOrigin() const;
   Tile &getDestination() const;
   bool isReverse() const;

   void begin();
   void rbegin();
   virtual DirectionType getNextDirection();
   bool isDestination() const;

   void setNextDirection(const DirectionType direction);
   void setNextTile( const Tile& tile);
   bool contains(Tile &tile);
   std::list<Tile*>& getAllTiles();

   void prettyPrint() const;
   void serialize(OutputSerialStream &stream);
   void unserialize(InputSerialStream &stream);
   void toggleDirection();

   PathWay& operator=(const PathWay& other );

private:
   Tilemap *_tilemap;
   Tile *_origin;
   int _destinationI;
   int _destinationJ;

   typedef std::vector<DirectionType> Directions;
   Directions _directionList;
   Directions::iterator _directionIt;
   Directions::reverse_iterator _directionIt_reverse;
   std::list<Tile*> _tileList;
   bool _isReverse;
};
bool operator<(const PathWay &v1, const PathWay &v2);

class Tilemap;
class City;
class Propagator
{
public:
  typedef std::map<BuildingPtr, PathWay> ReachedBuldings;
  
  Propagator();
  void setAllLands(const bool value);
  void setAllDirections(const bool value);

  /** propagate some data in the road network
  * param origin : propagation origin
  * param oCompletedBranches: result of the propagation: road=destination, pathWay=path
  */
  void init(Tile& origin);
  void init(const std::list<Tile*>& origin);
  void init(const Construction& origin);
  void propagate(const int maxDistance);

  void getReachedBuildings(const BuildingType buildingType, ReachedBuldings& oPathWayList);

  /** finds the shortest path between origin and destination
   * returns True if a path exists
   * the path is returned in oPathWay
   */
  bool getPath(Road &destination, PathWay &oPathWay);
  bool getPath(BuildingPtr destination, PathWay &oPathWay);

  /** returns all paths starting at origin */
  void getAllPaths(const int maxDistance, std::list<PathWay> &oPathWayList);

private:
  std::set<PathWay> _activeBranches;
  std::map<Tile*, PathWay> _completedBranches;

  bool _allLands;  // true if can walk in all lands, false if limited to roads
  bool _allDirections;  // true if can walk in all directions, false if limited to North/South/East/West

  City* _city;
  Tile* _origin;
  Tilemap* _tilemap;
};


#endif
