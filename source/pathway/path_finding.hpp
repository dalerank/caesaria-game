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

#ifndef _CAESARIA_PATHFINDING_HPP_INCLUDE_
#define _CAESARIA_PATHFINDING_HPP_INCLUDE_

#include "core/serializer.hpp"
#include "core/predefinitions.hpp"
#include "core/position.hpp"
#include "core/variant.hpp"
#include "pathway.hpp"
#include "gfx/tileoverlay.hpp"
#include "route.hpp"
#include "objects/constants.hpp"

class Propagator
{
public:
  typedef std::set<gfx::TileOverlay::Type> ObsoleteOverlays;

  Propagator( PlayerCityPtr city );
  ~Propagator();

  void setAllLands(const bool value);
  void setAllDirections(const bool value);
  void setObsoleteOverlay( gfx::TileOverlay::Type type );
  void setObsoleteOverlays( const ObsoleteOverlays& ovs );

  /** propagate some data in the road network
  * param origin : propagation origin
  * param oCompletedBranches: result of the propagation: road=destination, pathWay=path
  */
  void init(TilePos origin);
  void init(gfx::Tile& origin);
  void init(const gfx::TilesArray& origin);
  void init(const ConstructionPtr origin);
  void propagate(const unsigned int maxDistance);

  /** returns all paths starting at origin */
  PathwayList getWays(const unsigned int maxDistance);
  DirectPRoutes getRoutes(const gfx::TileOverlay::Type buildingType);

  DirectRoute getShortestRoute( const DirectPRoutes& routes );
  DirectRoute getShortestRoute( const gfx::TileOverlay::Type buildingType );

private:
  class Impl;
  ScopedPtr< Impl > _d;
};


#endif //_CAESARIA_PATHFINDING_HPP_INCLUDE_
