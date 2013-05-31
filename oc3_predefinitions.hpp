#ifndef __OPENCAESAR3_PREDEFINITIONS_H_INCLUDED__
#define __OPENCAESAR3_PREDEFINITIONS_H_INCLUDED__

#include "oc3_smartptr.hpp"

#include <list>
#include <vector>

class Walker;
typedef SmartPtr<Walker> WalkerPtr;
typedef std::list< WalkerPtr > Walkers;

class CartPusher;
typedef SmartPtr< CartPusher > CartPusherPtr;

class LandOverlay;
typedef SmartPtr< LandOverlay > LandOverlayPtr;
typedef std::list< LandOverlayPtr > LandOverlays;

class Building;
typedef SmartPtr< Building > BuildingPtr;
typedef std::list< BuildingPtr > Buildings;

class House;
typedef SmartPtr< House > HousePtr;

class Construction;
typedef SmartPtr< Construction > ConstructionPtr;

class Warehouse;
typedef SmartPtr< Warehouse > WarehousePtr;

class WorkingBuilding;
typedef SmartPtr< WorkingBuilding > WorkingBuildingPtr;
typedef std::list< WorkingBuildingPtr > WorkingBuildings;

class ServiceBuilding;
typedef SmartPtr< ServiceBuilding > ServiceBuildingPtr;

class ServiceWalker;
typedef SmartPtr< ServiceWalker > ServiceWalkerPtr;

class TraineeWalker;
typedef SmartPtr< TraineeWalker > TraineeWalkerPtr;

class Road;
typedef SmartPtr< Road > RoadPtr;

class Market;
typedef SmartPtr< Market > MarketPtr;

class MarketBuyer;
typedef SmartPtr< MarketBuyer > MarketBuyerPtr;

class Granary;
typedef SmartPtr< Granary > GranaryPtr;

class Factory;
typedef SmartPtr< Factory > FactoryPtr;

class RomeDivinity;
typedef SmartPtr< RomeDivinity > RomeDivinityPtr;

class Temple;
typedef SmartPtr< Temple > TemplePtr;

class Tile;
typedef std::list< const Tile* > ConstWayOnTiles;
typedef std::list< Tile* > WayOnTiles;
typedef std::list< Tile* > PtrTilesArea;
typedef std::list< Tile* > PtrTilesList;

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