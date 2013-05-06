#ifndef __OPENCAESAR3_PREDEFINITIONS_H_INCLUDED__
#define __OPENCAESAR3_PREDEFINITIONS_H_INCLUDED__

#include "oc3_smartptr.hpp"
#include <list>

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

#endif