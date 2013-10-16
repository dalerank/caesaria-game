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
#include "oc3_positioni.hpp"
#include "oc3_variant.hpp"
#include "oc3_pathway.hpp"
#include <list>

class Propagator
{
public:
  typedef std::pair< BuildingPtr,  PathWay > DirectRoute;
  typedef std::map< BuildingPtr, PathWay > Routes;
  //typedef std::list<PathWay> Ways;
  
  Propagator( CityPtr city );
  ~Propagator();

  void setAllLands(const bool value);
  void setAllDirections(const bool value);

  /** propagate some data in the road network
  * param origin : propagation origin
  * param oCompletedBranches: result of the propagation: road=destination, pathWay=path
  */
  void init(Tile& origin);
  void init(const TilemapTiles& origin);
  void init(const ConstructionPtr origin);
  void propagate(const int maxDistance);

  /** returns all paths starting at origin */
  void getWays(const int maxDistance, std::list<PathWay> &oPathWayList);
  void getRoutes(const LandOverlayType buildingType, Routes& oPathWayList);

  /** finds the shortest path between origin and destination
   * returns True if a path exists
   * the path is returned in oPathWay
   */
  bool getPath( RoadPtr destination, PathWay& oPathWay );
  bool getPath( BuildingPtr destination, PathWay& oPathWay );

  DirectRoute getShortestRoute( const Routes& routes );
  DirectRoute getShortestRoute( const LandOverlayType buildingType );

private:
  std::set<PathWay> _activeBranches;
  std::map<Tile*, PathWay> _completedBranches;

  bool _allLands;  // true if can walk in all lands, false if limited to roads
  bool _allDirections;  // true if can walk in all directions, false if limited to North/South/East/West

  Tile* _origin;
  Tilemap* _tilemap;

  class Impl;
  ScopedPtr< Impl > _d;
};


#endif
