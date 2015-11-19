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

#ifndef __CAESARIA_PATHWAYHELPER_H_INCLUDED__
#define __CAESARIA_PATHWAYHELPER_H_INCLUDED__

#include "pathway.hpp"
#include "city/predefinitions.hpp"
#include "objects/predefinitions.hpp"
#include "objects/constants.hpp"
#include "core/tilepos_array.hpp"
#include "gfx/tilesarray.hpp"
#include "route.hpp"

class PathwayCondition : std::set<const gfx::Tile*>
{
public:
  template<class Obj>
  bool append( SmartPtr<Obj> building )
  {
    return append( ptr_cast<Overlay>( building ) );
  }

  bool append(OverlayPtr overlay);
  void checkRoads(const gfx::Tile* tile, bool& ret);
  TilePossibleCondition byRoads();
};

class PathwayHelper
{
public:
  typedef enum { roadOnly=0, allTerrain, roadFirst, water, deepWater, deepWaterFirst } WayType;
  static Pathway create(TilePos startPos, TilePos stopPos,
                        WayType type=roadOnly );

  template<class T>
  static Pathway create( TilePos startPos, SmartPtr<T> building,
                         WayType type)
  {
    return create( startPos, ptr_cast<Construction>( building ), type );
  }

  static Pathway create(TilePos startPos, ConstructionPtr construction,
                        WayType type);

  static Pathway create(TilePos statrPos, TilePos stopPos,
                        const TilePossibleCondition& condition );

  static DirectRoute shortWay( PlayerCityPtr city, const TilePos& startPos, object::Type buildingType, WayType type );
  static DirectRoute shortWay( const TilePos& startPos, ConstructionList buildings, WayType type);
  static DirectRoute shortWay( PlayerCityPtr city, const Locations& area, object::Type buildingType, WayType type );

  static Pathway randomWay( PlayerCityPtr city, const TilePos& startPos, int walkRadius );
  static Pathway way2border( PlayerCityPtr city, const TilePos& startPos );
};

#endif
