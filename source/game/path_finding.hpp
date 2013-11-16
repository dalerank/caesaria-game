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

#include "enums.hpp"
#include "core/serializer.hpp"
#include "core/predefinitions.hpp"
#include "core/position.hpp"
#include "core/variant.hpp"
#include "pathway.hpp"
#include "gfx/tileoverlay.hpp"
#include <list>

class Propagator
{
public:
  typedef std::pair< ConstructionPtr, Pathway > DirectRoute;
  typedef std::map < ConstructionPtr, Pathway > Routes;
  typedef std::list< Pathway > PathWayList;
  //typedef std::list<PathWay> Ways;
  
  Propagator( PlayerCityPtr city );
  ~Propagator();

  void setAllLands(const bool value);
  void setAllDirections(const bool value);

  /** propagate some data in the road network
  * param origin : propagation origin
  * param oCompletedBranches: result of the propagation: road=destination, pathWay=path
  */
  void init(TilePos origin);
  void init(Tile& origin);
  void init(const TilesArray& origin);
  void init(const ConstructionPtr origin);
  void propagate(const int maxDistance);

  /** returns all paths starting at origin */
  PathWayList getWays(const int maxDistance);
  Routes getRoutes(const TileOverlay::Type buildingType);

  /** finds the shortest path between origin and destination
   * returns True if a path exists
   * the path is returned in oPathWay
   */
  bool getPath( RoadPtr destination, Pathway& oPathWay );
  bool getPath( ConstructionPtr destination, Pathway& oPathWay );

  DirectRoute getShortestRoute( const Routes& routes );
  DirectRoute getShortestRoute( const TileOverlay::Type buildingType );

private:
  class Impl;
  ScopedPtr< Impl > _d;
};


#endif
