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


#include "oc3_city.hpp"

#include "oc3_tile.hpp"
#include "oc3_building_data.hpp"
#include "oc3_path_finding.hpp"
#include "oc3_exception.hpp"
#include "oc3_positioni.hpp"
#include "oc3_constructionmanager.hpp"
#include "oc3_astarpathfinding.hpp"
#include "oc3_safetycast.hpp"
#include "oc3_cityservice_emigrant.hpp"
#include "oc3_cityservice_workershire.hpp"
#include "oc3_cityservice_timers.hpp"
#include "oc3_tilemap.hpp"
#include "oc3_road.hpp"
#include "oc3_variant.hpp"
#include "oc3_stringhelper.hpp"
#include "oc3_walkermanager.hpp"

#include <set>

typedef std::vector< CityServicePtr > CityServices;

class City::Impl
{
public:
  int population;
  long funds;  // amount of money
  unsigned long month; // number of months since start

  LandOverlays overlayList;
  Walkers walkerList;
  TilePos roadEntry; //coordinates can't be negative!
  CityServices services;
  bool needRecomputeAllRoads;
  int taxRate;
  unsigned long time;  // number of timesteps since start
  TilePos roadExit;
  Tilemap tilemap;
  TilePos boatEntry;
  TilePos boatExit;
  TilePos cameraStart;

  ClimateType climate;   
  UniqueId walkerIdCount;

oc3_signals public:
  Signal1<int> onPopulationChangedSignal;
  Signal1<int> onFundsChangedSignal;
  Signal1<int> onMonthChangedSignal;
  Signal1<std::string> onWarningMessageSignal;
};

City::City() : _d( new Impl )
{
  _d->time = 0;
  _d->month = 0;
  _d->roadEntry = TilePos( 0, 0 );
  _d->roadExit = TilePos( 0, 0 );
  _d->boatEntry = TilePos( 0, 0 );
  _d->boatExit = TilePos( 0, 0 );
  _d->funds = 1000;
  _d->population = 0;
  _d->needRecomputeAllRoads = false;
  _d->taxRate = 700;
  _d->climate = C_CENTRAL;

  addService( CityServiceEmigrant::create( *this ) );
  addService( CityServiceWorkersHire::create( *this ) );
  addService( CityServicePtr( &CityServiceTimers::getInstance() ) );
}

void City::timeStep()
{
  // CALLED 11 time/second
  _d->time += 1;

  if( _d->time % 110 == 1 )
  {
     // every X seconds
     _d->month++;
     monthStep();
  }

  Walkers::iterator walkerIt = _d->walkerList.begin();
  while (walkerIt != _d->walkerList.end())
  {
    try
    {
      WalkerPtr walker = *walkerIt;
      walker->timeStep( _d->time );

      if( walker->isDeleted() )
      {
        // remove the walker from the walkers list  
        walkerIt = _d->walkerList.erase(walkerIt);       
      }
      else
      {
         ++walkerIt;
      }
    }
    catch(...)
    {
      int o=0;
      //volatile error here... WTF
    }
  }

  LandOverlays::iterator overlayIt = _d->overlayList.begin();
  while( overlayIt != _d->overlayList.end() )
  {
    try
    {   
      (*overlayIt)->timeStep(_d->time);

      if( (*overlayIt)->isDeleted() )
      {
         // remove the overlay from the overlay list
          (*overlayIt)->destroy();
          //delete (*overlayIt);

          overlayIt = _d->overlayList.erase(overlayIt);
      }
      else
      {
         ++overlayIt;
      }
    }
    catch(...)
    {
      int i=0;
    }
  }

  CityServices::iterator serviceIt=_d->services.begin();
  while( serviceIt != _d->services.end() )
  {
    (*serviceIt)->update( _d->time );

    if( (*serviceIt)->isDeleted() )
    {
      (*serviceIt)->destroy();

      serviceIt = _d->services.erase(serviceIt);
    }
    else
      serviceIt++;
  }

  if( _d->needRecomputeAllRoads )
  {
    _d->needRecomputeAllRoads = false;
    for (LandOverlays::iterator itOverlay = _d->overlayList.begin(); itOverlay!=_d->overlayList.end(); ++itOverlay)
    {
      // for each overlay
      LandOverlayPtr overlay = *itOverlay;
      ConstructionPtr construction = overlay.as<Construction>();
      if( construction != NULL )
      {
        // overlay matches the filter
        construction->computeAccessRoads();
        // for some constructions we need to update picture
        if( construction->getType() == B_ROAD ) 
        {
          RoadPtr road = construction.as<Road>();
          road->updatePicture();
        }
      }
    }   
  }
}

void City::monthStep()
{
  collectTaxes();
  _calculatePopulation();
  _d->onMonthChangedSignal.emit( _d->month );
}

Walkers City::getWalkerList( const WalkerType type )
{
  Walkers res;

  WalkerPtr walker;
  for (Walkers::iterator itWalker = _d->walkerList.begin(); itWalker != _d->walkerList.end(); ++itWalker )
  {
    if( (*itWalker)->getType() == type || WT_ALL == type )
    {
      res.push_back( *itWalker );
    }
  }

  return res;
}

LandOverlays& City::getOverlayList()
{
  return _d->overlayList;
}

unsigned long City::getTime()
{
  return _d->time;
}

LandOverlays City::getBuildingList(const BuildingType buildingType)
{
   LandOverlays res;

   for( LandOverlays::iterator itOverlay = _d->overlayList.begin(); 
        itOverlay!=_d->overlayList.end(); ++itOverlay)
   {
      // for each overlay
      LandOverlayPtr overlay = *itOverlay;
      ConstructionPtr construction = overlay.as<Construction>();
      if( construction.isValid() && construction->getType() == buildingType)
      {
         // overlay matches the filter
         res.push_back( overlay );
      }
   }

   return res;
}

Tilemap& City::getTilemap()
{
   return _d->tilemap;
}

TilePos City::getBoatEntry() const { return _d->boatEntry; }
TilePos City::getBoatExit() const  { return _d->boatExit;  }

ClimateType City::getClimate() const     { return _d->climate;    }

void City::setClimate(const ClimateType climate) { _d->climate = climate; }

// paste here protection from bad values
void City::setRoadEntry( const TilePos& pos )
{
  int size = getTilemap().getSize();
  _d->roadEntry = TilePos( math::clamp<unsigned int>( pos.getI(), 0, size - 1 ),
                           math::clamp<unsigned int>( pos.getJ(), 0, size - 1 ) );
}

void City::setRoadExit( const TilePos& pos )
{
  int size = getTilemap().getSize();
  _d->roadExit.setI( math::clamp<unsigned int>( pos.getI(), 0, size - 1 ) );
  _d->roadExit.setJ( math::clamp<unsigned int>( pos.getJ(), 0, size - 1 ) );
}

void City::setBoatEntry(const TilePos& pos )
{
  int size = getTilemap().getSize();
  _d->boatEntry.setI( math::clamp<unsigned int>( pos.getI(), 0, size - 1 ) );
  _d->boatEntry.setJ( math::clamp<unsigned int>( pos.getJ(), 0, size - 1 ) );
}

void City::setBoatExit( const TilePos& pos )
{
  int size = getTilemap().getSize();
  _d->boatExit.setI( math::clamp<unsigned int>( pos.getI(), 0, size - 1 ) );
  _d->boatExit.setJ( math::clamp<unsigned int>( pos.getJ(), 0, size - 1 ) );
}

int City::getTaxRate() const                 {  return _d->taxRate;    }
void City::setTaxRate(const int taxRate)     {  _d->taxRate = taxRate; }
long City::getFunds() const                  {  return _d->funds;   }
void City::setFunds(const long funds)        {  _d->funds = funds;  }

long City::getPopulation() const
{
   /* here we need to calculate population ??? */
   
   return _d->population;
}

void City::build( const BuildingType type, const TilePos& pos )
{
   BuildingData& buildingData = BuildingDataHolder::instance().getData( type );
   // make new building
   ConstructionPtr building = ConstructionManager::getInstance().create( type );
   if( building.isValid() )
   {
     building->build( pos );

     _d->overlayList.push_back( building.as<LandOverlay>() );
     _d->funds -= buildingData.getCost();
     _d->onFundsChangedSignal.emit( _d->funds );

     if( building->isNeedRoadAccess() && building->getAccessRoads().empty() )
     {
       _d->onWarningMessageSignal.emit( "##building_need_road_access##" );
     }
   }
}

void City::disaster( const TilePos& pos, DisasterType type )
{
    TerrainTile& terrain = _d->tilemap.at( pos ).getTerrain();
    TilePos rPos = pos;

    if( terrain.isDestructible() )
    {
        int size = 1;

        LandOverlayPtr overlay = terrain.getOverlay();
        if( overlay.isValid() )
        {
          overlay->deleteLater();
          size = overlay->getSize();
          rPos = overlay->getTile().getIJ();
        }

        bool deleteRoad = false;

        PtrTilesArea clearedTiles = _d->tilemap.getFilledRectangle( rPos, Size( size ) );
        for( PtrTilesArea::iterator itTile = clearedTiles.begin(); itTile!=clearedTiles.end(); ++itTile)
        {
          BuildingType dstr2constr[] = { B_BURNING_RUINS, B_COLLAPSED_RUINS };
          bool canCreate = ConstructionManager::getInstance().canCreate( dstr2constr[type] );
          if( canCreate )
            build( dstr2constr[type], (*itTile)->getIJ() );
       }
    }
}

void City::clearLand(const TilePos& pos  )
{
  Tile& cursorTile = _d->tilemap.at( pos );
  TerrainTile& terrain = cursorTile.getTerrain();

  if( terrain.isDestructible() )
  {
    int size = 1;
    TilePos rPos = pos;

    LandOverlayPtr overlay = terrain.getOverlay();
      
    bool deleteRoad = false;

    if (terrain.isRoad()) deleteRoad = true;
      
    if ( overlay.isValid() )	
    {
      size = overlay->getSize();
      rPos = overlay->getTile().getIJ();
      overlay->deleteLater();
    }

    PtrTilesArea clearedTiles = _d->tilemap.getFilledRectangle( rPos, Size( size ) );
    for (PtrTilesArea::iterator itTile = clearedTiles.begin(); itTile!=clearedTiles.end(); ++itTile)
    {
      (*itTile)->setMasterTile(NULL);
      TerrainTile &terrain = (*itTile)->getTerrain();
      terrain.setTree(false);
      terrain.setBuilding(false);
      terrain.setRoad(false);
      terrain.setGarden(false);
      terrain.setOverlay(NULL);

      // choose a random landscape picture:
      // flat land1a 2-9;
      // wheat: land1a 18-29;
      // green_something: land1a 62-119;  => 58
      // green_flat: land1a 232-289; => 58

      // FIX: when delete building on meadow, meadow is replaced by common land tile
      if( terrain.isMeadow() )
      {
        unsigned int originId = terrain.getOriginalImgId();
        (*itTile)->setPicture( &Picture::load( TerrainTileHelper::convId2PicName( originId ) ) );
      }
      else
      {
        // choose a random background image, green_something 62-119 or green_flat 232-240
         // 30% => choose green_sth 62-119
        // 70% => choose green_flat 232-289
        int startOffset  = ( (rand() % 10 > 6) ? 62 : 232 ); 
        int imgId = rand() % 58;

        (*itTile)->setPicture( &Picture::load( "land1a", startOffset + imgId));
      }      
    }
      
    // recompute roads;
    // there is problem that we NEED to recompute all roads map for all buildings
    // because MaxDistance2Road can be any number    
    if( deleteRoad )
    {
      _d->needRecomputeAllRoads = true;     
    }
  }
}

void City::collectTaxes()
{
  CityHelper hlp( *this );
  long taxes = 0;
  
  std::list<HousePtr> houseList = hlp.getBuildings< House >(B_HOUSE);
  for( std::list<HousePtr>::iterator itHouse = houseList.begin(); itHouse != houseList.end(); ++itHouse)
  {
    taxes += (*itHouse)->collectTaxes();
  }

  _d->funds += taxes;
  _d->onFundsChangedSignal.emit( _d->funds );

  std::cout << "Monthly Taxes=" << taxes << std::endl;
}

void City::_calculatePopulation()
{
  long pop = 0; /* population can't be negative - should be unsigned long long*/
  
  LandOverlays houseList = getBuildingList(B_HOUSE);
  for( LandOverlays::iterator itHouse = houseList.begin(); 
       itHouse != houseList.end(); ++itHouse)
  {
    HousePtr house = (*itHouse).as<House>();
    if( house.isValid() )
    {
        pop += house->getNbHabitants();
    }
  }
  
  _d->population = pop;
  _d->onPopulationChangedSignal.emit( pop );
}

void City::save( VariantMap& stream) const
{
  VariantMap vm_tilemap;
  _d->tilemap.save( vm_tilemap );

  stream[ "tilemap" ] = vm_tilemap;
  stream[ "roadEntry" ] = _d->roadEntry;
  stream[ "roadExit" ]  = _d->roadExit;
  stream[ "cameraStart" ] = _d->cameraStart;
  stream[ "boatEntry" ] = _d->boatEntry;
  stream[ "boatExit" ] = _d->boatExit;
  stream[ "climate" ] = _d->climate;
  stream[ "time" ] = static_cast<unsigned long long>(_d->time);
  stream[ "funds" ] = static_cast<unsigned int>(_d->funds);
  stream[ "population" ] = _d->population;

  // walkers
  VariantMap vm_walkers;
  int walkedId = 0;
  for (Walkers::iterator itWalker = _d->walkerList.begin(); itWalker != _d->walkerList.end(); ++itWalker, walkedId++)
  {
    // std::cout << "WRITE WALKER @" << stream.tell() << std::endl;
     VariantMap vm_walker;
    (*itWalker)->save( vm_walker );
    vm_walkers[ StringHelper::format( 0xff, "%d", walkedId ) ] = vm_walker;
  }
  stream[ "walkers" ] = vm_walkers;

  // overlays
  VariantMap vm_overlays;
  for( LandOverlays::iterator itOverlay = _d->overlayList.begin(); 
       itOverlay != _d->overlayList.end(); ++itOverlay )
  {
    VariantMap vm_overlay;
    (*itOverlay)->save( vm_overlay );
    vm_overlays[ StringHelper::format( 0xff, "%03d%03d", (*itOverlay)->getTile().getI(),
                                                         (*itOverlay)->getTile().getJ() ) ] = vm_overlay;
  }

  stream[ "overlays" ] = vm_overlays;
}

void City::load( const VariantMap& stream )
{
  _d->tilemap.load( stream.get( "tilemap" ).toMap() );

  _d->roadEntry = TilePos( stream.get( "roadEntry" ).toTilePos() );
  _d->roadExit = TilePos( stream.get( "roadExit" ).toTilePos() );
  _d->boatEntry = TilePos( stream.get( "boatEntry" ).toTilePos() );
  _d->boatExit = TilePos( stream.get( "boatExit" ).toTilePos() );
  _d->climate = (ClimateType)stream.get( "climate" ).toInt(); 
  _d->time = (unsigned long)stream.get( "time" ).toULongLong();
  _d->funds = stream.get( "funds" ).toInt();
  _d->population = stream.get( "population" ).toInt();
  _d->cameraStart = TilePos( stream.get( "cameraStart" ).toTilePos() );

  VariantMap overlays = stream.get( "overlays" ).toMap();
  for( VariantMap::iterator it=overlays.begin(); it != overlays.end(); it++ )
  {
    VariantMap overlay = (*it).second.toMap();
    TilePos buildPos( overlay.get( "pos" ).toTilePos() );
    int buildingType = overlay.get( "buildingType" ).toInt();

    ConstructionPtr construction = ConstructionManager::getInstance().create( BuildingType( buildingType ) );
    if( construction.isValid() )
    {
      construction->build( buildPos );
      construction->load( overlay );
      _d->overlayList.push_back( construction.as<LandOverlay>() );
    }
  }

  VariantMap walkers = stream.get( "walkers" ).toMap();
  for( VariantMap::iterator it=walkers.begin(); it != walkers.end(); it++ )
  {
    VariantMap walkerInfo = (*it).second.toMap();
    int walkerType = walkerInfo.get( "type" ).toInt();

    WalkerPtr walker = WalkerManager::getInstance().create( WalkerType( walkerType ) );
    if( walker.isValid() )
    {
      walker->load( walkerInfo );
      _d->walkerList.push_back( walker );
    }
  }
}

TilePos City::getRoadEntry() const
{
  return _d->roadEntry;
}

TilePos City::getRoadExit() const
{
  return _d->roadExit;
}

City::~City()
{
}

Signal1<int>& City::onPopulationChanged()
{
  return _d->onPopulationChangedSignal;
}

Signal1<int>& City::onFundsChanged()
{
  return _d->onFundsChangedSignal;
}

unsigned long City::getMonth() const
{
  return _d->month;
}

Signal1<int>& City::onMonthChanged()
{
  return _d->onMonthChangedSignal;
}

void City::addWalker( WalkerPtr walker )
{
  walker->setUniqueId( ++_d->walkerIdCount );
  _d->walkerList.push_back( walker );
}

void City::removeWalker( WalkerPtr walker )
{
  _d->walkerList.remove( walker );
}

void City::setCameraPos(const TilePos pos) { _d->cameraStart = pos; }
TilePos City::getCameraPos() const {return _d->cameraStart; }

void City::addService( CityServicePtr service )
{
  _d->services.push_back( service );
}

CityServicePtr City::findService( const std::string& name )
{
  for( CityServices::iterator sIt=_d->services.begin(); sIt != _d->services.end(); sIt++ )
    if( name == (*sIt)->getName() )
      return *sIt;

  return CityServicePtr();
}

Signal1<std::string>& City::onWarningMessage()
{
  return _d->onWarningMessageSignal;
}