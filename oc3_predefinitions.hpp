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

#ifndef __OPENCAESAR3_PREDEFINITIONS_H_INCLUDED__
#define __OPENCAESAR3_PREDEFINITIONS_H_INCLUDED__

#include "oc3_smartptr.hpp"

#include <list>
#include <vector>

#define PREFEDINE_CLASS_SMARTPOINTER(a) class a; typedef SmartPtr<a> a##Ptr;

PREFEDINE_CLASS_SMARTPOINTER(Walker)
typedef std::list< WalkerPtr > Walkers;

PREFEDINE_CLASS_SMARTPOINTER(CartPusher)
PREFEDINE_CLASS_SMARTPOINTER(LandOverlay)
typedef std::list< LandOverlayPtr > LandOverlays;

PREFEDINE_CLASS_SMARTPOINTER(Building)
typedef std::list< BuildingPtr > Buildings;

PREFEDINE_CLASS_SMARTPOINTER(House);
PREFEDINE_CLASS_SMARTPOINTER(Construction)
PREFEDINE_CLASS_SMARTPOINTER(Warehouse)
PREFEDINE_CLASS_SMARTPOINTER(WorkingBuilding)
typedef std::list< WorkingBuildingPtr > WorkingBuildings;

PREFEDINE_CLASS_SMARTPOINTER(Garden)
PREFEDINE_CLASS_SMARTPOINTER(ServiceBuilding)
PREFEDINE_CLASS_SMARTPOINTER(ServiceWalker)
PREFEDINE_CLASS_SMARTPOINTER(TraineeWalker)
PREFEDINE_CLASS_SMARTPOINTER(Road)
PREFEDINE_CLASS_SMARTPOINTER(Market)
PREFEDINE_CLASS_SMARTPOINTER(MarketBuyer)
PREFEDINE_CLASS_SMARTPOINTER(Granary)
PREFEDINE_CLASS_SMARTPOINTER(Factory)
PREFEDINE_CLASS_SMARTPOINTER(RomeDivinity)
PREFEDINE_CLASS_SMARTPOINTER(Temple)
PREFEDINE_CLASS_SMARTPOINTER(City)
PREFEDINE_CLASS_SMARTPOINTER(Senate)

class Tile;
typedef std::list< const Tile* > ConstWayOnTiles;
typedef std::list< Tile* > WayOnTiles;
typedef std::list< Tile* > PtrTilesArea;
typedef std::list< Tile* > PtrTilesList;
typedef std::list< const Tile* > ConstPtrTilesList;

typedef std::vector< Tile* > PtrTilesVector;
typedef std::vector< Tile  > TilesVector;
typedef std::vector< TilesVector > TileGrid;

class VariantMap;
class Picture;
class TilePos;
class Size;
class TerrainTile;
class VariantMap;

class Widget;
class GuiEnv;

#endif