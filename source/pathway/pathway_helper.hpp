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
#include "route.hpp"

class PathwayHelper
{
public:
  typedef enum { roadOnly=0, allTerrain, roadFirst, water, deepWater, deepWaterFirst } WayType;
  static Pathway create(TilePos startPos, TilePos stopPos,
                        WayType type=roadOnly );

  static Pathway create(TilePos startPos, ConstructionPtr construction,
                        WayType type);

  static Pathway create(TilePos statrPos, TilePos stopPos,
                        const TilePossibleCondition& condition );

  static DirectRoute shortWay( PlayerCityPtr city, TilePos startPos, constants::objects::Type buildingType, WayType type );

  static Pathway randomWay( PlayerCityPtr city, TilePos startPos, int walkRadius );

  static Pathway way2border( PlayerCityPtr city, TilePos startPos );
};

#endif
