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
#define PREFEDINE_CLASS_SMARTPOINTER_LIST(a,b) PREFEDINE_CLASS_SMARTPOINTER(a); typedef std::list< a##Ptr > a##b;

PREFEDINE_CLASS_SMARTPOINTER_LIST(Walker,List)
PREFEDINE_CLASS_SMARTPOINTER_LIST(Forum,List)
PREFEDINE_CLASS_SMARTPOINTER_LIST(House,List)
PREFEDINE_CLASS_SMARTPOINTER_LIST(Temple,List)
PREFEDINE_CLASS_SMARTPOINTER_LIST(Theater,List)
PREFEDINE_CLASS_SMARTPOINTER_LIST(Library,List)
PREFEDINE_CLASS_SMARTPOINTER_LIST(School,List)
PREFEDINE_CLASS_SMARTPOINTER_LIST(College,List)
PREFEDINE_CLASS_SMARTPOINTER_LIST(WorkingBuilding,List)
PREFEDINE_CLASS_SMARTPOINTER_LIST(EmpireTradeRoute,List)
PREFEDINE_CLASS_SMARTPOINTER_LIST(Warehouse,List)
PREFEDINE_CLASS_SMARTPOINTER_LIST(Farm,List)
PREFEDINE_CLASS_SMARTPOINTER_LIST(EmpireCity,List)
PREFEDINE_CLASS_SMARTPOINTER_LIST(TileOverlay,List)
PREFEDINE_CLASS_SMARTPOINTER_LIST(Building,List)
PREFEDINE_CLASS_SMARTPOINTER_LIST(Factory, List)
PREFEDINE_CLASS_SMARTPOINTER_LIST(ServiceBuilding,List)
PREFEDINE_CLASS_SMARTPOINTER_LIST(Aqueduct,List)
PREFEDINE_CLASS_SMARTPOINTER_LIST(ActorColony,List)
PREFEDINE_CLASS_SMARTPOINTER_LIST(GladiatorSchool,List)
PREFEDINE_CLASS_SMARTPOINTER_LIST(Road,List)
PREFEDINE_CLASS_SMARTPOINTER_LIST(Fountain,List)
PREFEDINE_CLASS_SMARTPOINTER_LIST(Construction,List)
PREFEDINE_CLASS_SMARTPOINTER_LIST(Immigrant,List)

PREFEDINE_CLASS_SMARTPOINTER(Collosseum)
PREFEDINE_CLASS_SMARTPOINTER(CartPusher)
PREFEDINE_CLASS_SMARTPOINTER(Garden)
PREFEDINE_CLASS_SMARTPOINTER(ServiceWalker)
PREFEDINE_CLASS_SMARTPOINTER(TraineeWalker)
PREFEDINE_CLASS_SMARTPOINTER(Market)
PREFEDINE_CLASS_SMARTPOINTER(MarketLady)
PREFEDINE_CLASS_SMARTPOINTER(Granary)
PREFEDINE_CLASS_SMARTPOINTER(RomeDivinity)
PREFEDINE_CLASS_SMARTPOINTER(City)
PREFEDINE_CLASS_SMARTPOINTER(Senate)
PREFEDINE_CLASS_SMARTPOINTER(TempleOracle)
PREFEDINE_CLASS_SMARTPOINTER(EmpireMerchant)
PREFEDINE_CLASS_SMARTPOINTER(Empire)
PREFEDINE_CLASS_SMARTPOINTER(MarketLadyHelper)
PREFEDINE_CLASS_SMARTPOINTER(Animal)
PREFEDINE_CLASS_SMARTPOINTER(CartSupplier)
PREFEDINE_CLASS_SMARTPOINTER(Prefecture)
PREFEDINE_CLASS_SMARTPOINTER(GameEvent)
PREFEDINE_CLASS_SMARTPOINTER(Player)

class Tile;
typedef std::list< const Tile* > ConstTilemapWay;
typedef std::list< Tile* > TilemapWay;
typedef std::list< Tile* > TilemapArea;
typedef std::list< Tile* > TilemapTiles;
typedef std::list< const Tile* > ConstTilemapTiles;
typedef std::list< const Tile* > ConstTilemapArea;

class VariantMap;
class Picture;
class TilePos;
class Size;
class TerrainTile;
class Tilemap;
class VariantMap;

class Widget;
class GuiEnv;
class DateTime;

#endif
