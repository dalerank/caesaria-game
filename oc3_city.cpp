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

#include <iostream>
#include <set>

#include "oc3_building_data.hpp"
#include "oc3_path_finding.hpp"
#include "oc3_exception.hpp"
#include "oc3_positioni.hpp"
#include "oc3_constructionmanager.hpp"
#include "oc3_astarpathfinding.hpp"
#include "oc3_safetycast.hpp"
#include "oc3_cityservice_emigrant.hpp"

typedef std::vector< CityServicePtr > CityServices;

class City::Impl
{
public:
    Signal1<int> onPopulationChangedSignal;
    Signal1<int> onFundsChangedSignal;
    Signal1<int> onMonthChangedSignal;

    int population;
    long funds;  // amount of money
    unsigned long month; // number of months since start

    LandOverlays overlayList;
    Walkers walkerList;
    TilePos roadEntry; //coordinates can't be negative!
    CityServices services;
};

City::City() : _d( new Impl )
{
   _time = 0;
   _d->month = 0;
   _d->roadEntry = TilePos( 0, 0 );
   _roadExitI = 0;
   _roadExitJ = 0;
   _boatEntryI = 0;
   _boatEntryJ = 0;
   _boatExitI = 0;
   _boatExitJ = 0;
   _d->funds = 1000;
   _d->population = 0;
   _taxRate = 700;
   _climate = C_CENTRAL;
   
   // DEBUG
   pGraphicGrid = (short int     *)malloc(52488);
   pEdgeGrid    = (unsigned char *)malloc(26244);
   pTerrainGrid = (short int     *)malloc(52488);
   pRndmTerGrid = (unsigned char *)malloc(26244);
   pRandomGrid  = (unsigned char *)malloc(26244);
   pZeroGrid    = (unsigned char *)malloc(26244);
   if ( pGraphicGrid == NULL || pEdgeGrid == NULL || pTerrainGrid == NULL ||
     pRndmTerGrid == NULL || pRandomGrid == NULL || pZeroGrid == NULL )
     THROW("NOT ENOUGH MEMORY!!!! FATAL");

   addService( CityServiceEmigrant::create( *this ) );
}

void City::timeStep()
{
  // CALLED 11 time/second
  _time += 1;

  if( _time % 110 == 1 )
  {
     // every X seconds
     _d->month++;
     monthStep();
  }

  Walkers::iterator walkerIt = _d->walkerList.begin();
  while (walkerIt != _d->walkerList.end())
  {
     Walker& walker = **walkerIt;
     walker.timeStep(_time);

     if( walker.isDeleted() )
     {
       // remove the walker from the walkers list  
       delete *walkerIt;
       walkerIt = _d->walkerList.erase(walkerIt);       
     }
     else
     {
        ++walkerIt;
     }
  }

  LandOverlays::iterator overlayIt = _d->overlayList.begin();
  while( overlayIt != _d->overlayList.end() )
  {
      (*overlayIt)->timeStep(_time);

      if( (*overlayIt)->isDeleted() )
      {
         // remove the overlay from the overlay list
          (*overlayIt)->destroy();
          delete (*overlayIt);

          overlayIt = _d->overlayList.erase(overlayIt);
      }
      else
      {
         ++overlayIt;
      }
  }

  CityServices::iterator serviceIt=_d->services.begin();
  while( serviceIt != _d->services.end() )
  {
    (*serviceIt)->update( _time );

    if( (*serviceIt)->isDeleted() )
    {
      (*overlayIt)->destroy();

      serviceIt = _d->services.erase(serviceIt);
    }
    else
      serviceIt++;
  }
}

void City::monthStep()
{
   collectTaxes();
   _calculatePopulation();
   _d->onMonthChangedSignal.emit( _d->month );
}

City::Walkers City::getWalkerList( const WalkerType type )
{
	Walkers res;

	Walker* walker = 0;
	for (Walkers::iterator itWalker = _d->walkerList.begin(); itWalker != _d->walkerList.end(); ++itWalker )
	{
		// for each walker
		walker = *itWalker;
		if( walker && (walker->getType() == type || WT_ALL == type ) )
		{
			res.push_back(walker);
		}
	}

	return res;
}

std::list<LandOverlay*>& City::getOverlayList()
{
   return _d->overlayList;
}

unsigned long City::getTime()
{
   return _time;
}

std::list<LandOverlay*> City::getBuildingList(const BuildingType buildingType)
{
   LandOverlays res;

   for (LandOverlays::iterator itOverlay = _d->overlayList.begin(); itOverlay!=_d->overlayList.end(); ++itOverlay)
   {
      // for each overlay
      LandOverlay *overlay = *itOverlay;
      Construction *construction = safety_cast<Construction*>(overlay);
      if (construction != NULL && construction->getType() == buildingType)
      {
         // overlay matches the filter
         res.push_back(overlay);
      }
   }

   return res;
}

void City::recomputeRoadsForAll()
{
   for (LandOverlays::iterator itOverlay = _d->overlayList.begin(); itOverlay!=_d->overlayList.end(); ++itOverlay)
   {
      // for each overlay
      LandOverlay *overlay = *itOverlay;
      Construction *construction = safety_cast<Construction*>(overlay);
      if( construction != NULL )
      {
        // overlay matches the filter
        construction->computeAccessRoads();
	      // for some constructions we need to update picture
	      if (construction->getType() == B_ROAD) 
        {
          construction->setPicture(safety_cast<Road*>(construction)->computePicture());
        }
      }
   }   
}

Tilemap& City::getTilemap()
{
   return _tilemap;
}

unsigned int City::getRoadExitI() const  { return _roadExitI;  }
unsigned int City::getRoadExitJ() const  { return _roadExitJ;  }
unsigned int City::getBoatEntryI() const { return _boatEntryI; }
unsigned int City::getBoatEntryJ() const { return _boatEntryJ; }
unsigned int City::getBoatExitI() const  { return _boatExitI;  }
unsigned int City::getBoatExitJ() const  { return _boatExitJ;  }

ClimateType City::getClimate() const     { return _climate;    }

void City::setClimate(const ClimateType climate) { _climate = climate; }

// paste here protection from bad values
void City::setRoadEntry( const TilePos& pos )
{
  int size = getTilemap().getSize();
  _d->roadEntry = TilePos( math::clamp<unsigned int>( pos.getI(), 0, size - 1 ),
                           math::clamp<unsigned int>( pos.getJ(), 0, size - 1 ) );
}

void City::setRoadExitIJ(unsigned int i, unsigned int j)
{
  int size = getTilemap().getSize();
  i = math::clamp<unsigned int>( i, 0, size - 1 );
  j = math::clamp<unsigned int>( j, 0, size - 1 );
  _roadExitI  = i;
  _roadExitJ  = j;
}

void City::setBoatEntryIJ(unsigned int i, unsigned int j)
{
  int size = getTilemap().getSize();
  i = math::clamp<unsigned int>( i, 0, size - 1 );
  j = math::clamp<unsigned int>( j, 0, size - 1 );
  _boatEntryI = i;
  _boatEntryJ = j;
}

void City::setBoatExitIJ(unsigned int i, unsigned int j)
{
  int size = getTilemap().getSize();
  i = math::clamp<unsigned int>( i, 0, size - 1 );
  j = math::clamp<unsigned int>( j, 0, size - 1 );
  _boatExitI  = i;
  _boatExitJ = j;
}

int City::getTaxRate() const                 {  return _taxRate;    }
void City::setTaxRate(const int taxRate)     {  _taxRate = taxRate; }
long City::getFunds() const                  {  return _d->funds;   }
void City::setFunds(const long funds)        {  _d->funds = funds;  }

long City::getPopulation() const
{
   /* here we need to calculate population ??? */
   
   return _d->population;
}

void City::build( Construction& buildInstance, const TilePos& pos )
{
   BuildingData& buildingData = BuildingDataHolder::instance().getData(buildInstance.getType());
   // make new building
   Construction* building = (Construction*)buildInstance.clone();
   building->build( pos );

   _d->overlayList.push_back(building);
   _d->funds -= buildingData.getCost();
   _d->onFundsChangedSignal.emit( _d->funds );

   Pathfinder::getInstance().update( _tilemap );
}

void City::disaster( const TilePos& pos, DisasterType type )
{
    TerrainTile& terrain = _tilemap.at( pos ).get_terrain();

    if( terrain.isDestructible() )
    {
        int size = 1;
 
        LandOverlay* overlay = terrain.getOverlay();

        bool deleteRoad = false;

        std::list<Tile*> clearedTiles = _tilemap.getFilledRectangle( pos, Size( size ) );
        for (std::list<Tile*>::iterator itTile = clearedTiles.begin(); itTile!=clearedTiles.end(); ++itTile)
        {
            BuildingType dstr2constr[] = { B_BURNING_RUINS, B_COLLAPSED_RUINS };
            Construction* ruins = dynamic_cast<Construction*>( ConstructionManager::getInstance().create( dstr2constr[type] ) );
            if( ruins )
                build( *ruins, (*itTile)->getIJ() );
       }
    }
}

void City::clearLand(const TilePos& pos  )
{
  Tile& cursorTile = _tilemap.at( pos );
  TerrainTile& terrain = cursorTile.get_terrain();

  if( terrain.isDestructible() )
  {
    int size = 1;
    TilePos rPos = pos;

    LandOverlay* overlay = terrain.getOverlay();
      
    bool deleteRoad = false;

    if (terrain.isRoad()) deleteRoad = true;
      
    if (overlay != NULL)	
    {
      size = overlay->getSize();
      rPos = overlay->getTile().getIJ();
      overlay->deleteLater();
    }

    std::list<Tile*> clearedTiles = _tilemap.getFilledRectangle( rPos, Size( size - 1 ) );
    for (std::list<Tile*>::iterator itTile = clearedTiles.begin(); itTile!=clearedTiles.end(); ++itTile)
    {
      (*itTile)->set_master_tile(NULL);
      TerrainTile &terrain = (*itTile)->get_terrain();
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
        Picture& pic = PicLoader::instance().get_picture( TerrainTileHelper::convId2PicName( originId ) );

        (*itTile)->set_picture( &pic );
      }
      else
      {
        // choose a random background image, green_something 62-119 or green_flat 232-240
         // 30% => choose green_sth 62-119
        // 70% => choose green_flat 232-289
        int startOffset  = ( (rand() % 10 > 6) ? 62 : 232 ); 
        int imgId = rand() % 58;

        (*itTile)->set_picture(&PicLoader::instance().get_picture("land1a", startOffset + imgId));
      }      
    }
      
    // recompute roads;
    // there is problem that we NEED to recompute all roads map for all buildings
    // because MaxDistance2Road can be any number
    
    if( deleteRoad )
    {
      recomputeRoadsForAll();     
    }
  }
}

void City::collectTaxes()
{
   long taxes = 0;
   std::list<LandOverlay*> houseList = getBuildingList(B_HOUSE);
   for (std::list<LandOverlay*>::iterator itHouse = houseList.begin(); itHouse != houseList.end(); ++itHouse)
   {
      House &house = dynamic_cast<House&>(**itHouse);
      taxes += house.collectTaxes();
   }

   _d->funds += taxes;
   _d->onFundsChangedSignal.emit( _d->funds );

   std::cout << "Monthly Taxes=" << taxes << std::endl;
}

void City::_calculatePopulation()
{
  long pop = 0; /* population can't be negative - should be unsigned long long*/
  
  std::list<LandOverlay*> houseList = getBuildingList(B_HOUSE);
  for (std::list<LandOverlay*>::iterator itHouse = houseList.begin(); itHouse != houseList.end(); ++itHouse)
  {
    //check for error on dyncast
    if( House* house = dynamic_cast<House*>(*itHouse) )
    {
        pop += house->getNbHabitants();
    }
  }
  
  _d->population = pop;
  _d->onPopulationChangedSignal.emit( pop );
}

void City::serialize(OutputSerialStream &stream)
{
   // std::cout << "WRITE TILEMAP @" << stream.tell() << std::endl;
   getTilemap().serialize(stream);
   // std::cout << "WRITE CITY @" << stream.tell() << std::endl;
   stream.write_int( _d->roadEntry.getI(), 2, 0, 1000);
   stream.write_int( _d->roadEntry.getJ(), 2, 0, 1000);
   stream.write_int(_roadExitI, 2, 0, 1000);
   stream.write_int(_roadExitJ, 2, 0, 1000);
   stream.write_int(_boatEntryI, 2, 0, 1000);
   stream.write_int(_boatEntryJ, 2, 0, 1000);
   stream.write_int(_boatExitI, 2, 0, 1000);
   stream.write_int(_boatExitJ, 2, 0, 1000);
   stream.write_int((int) _climate, 2, 0, C_MAX);
   stream.write_int(_time, 4, 0, 1000000);
   stream.write_int(_d->funds, 4, 0, 1000000);
   stream.write_int(_d->population, 4, 0, 1000000);

   // walkers
   stream.write_int(_d->walkerList.size(), 2, 0, 65535);
   for (Walkers::iterator itWalker = _d->walkerList.begin(); itWalker != _d->walkerList.end(); ++itWalker)
   {
      // std::cout << "WRITE WALKER @" << stream.tell() << std::endl;
      Walker &walker = **itWalker;
      walker.serialize(stream);
   }

   // overlays
   stream.write_int(_d->overlayList.size(), 2, 0, 65535);
   for (LandOverlays::iterator itOverlay = _d->overlayList.begin(); 
        itOverlay != _d->overlayList.end(); 
        ++itOverlay)
   {
      // std::cout << "WRITE OVERLAY @" << stream.tell() << std::endl;
      LandOverlay &overlay = **itOverlay;
      overlay.serialize(stream);
   }

}

void City::unserialize(InputSerialStream &stream)
{
   // std::cout << "READ TILEMAP @" << stream.tell() << std::endl;
   _tilemap.unserialize(stream);
   // std::cout << "READ CITY @" << stream.tell() << std::endl;
   _d->roadEntry.setI( stream.read_int(2, 0, 1000) );
   _d->roadEntry.setJ( stream.read_int(2, 0, 1000) );
   _roadExitI = stream.read_int(2, 0, 1000);
   _roadExitJ = stream.read_int(2, 0, 1000);
   _boatEntryI = stream.read_int(2, 0, 1000);
   _boatEntryJ = stream.read_int(2, 0, 1000);
   _boatExitI = stream.read_int(2, 0, 1000);
   _boatExitJ = stream.read_int(2, 0, 1000);
   _climate = (ClimateType) stream.read_int(2, 0, 1000);
   _time = stream.read_int(4, 0, 1000000);
   _d->funds = stream.read_int(4, 0, 1000000);
   _d->population = stream.read_int(4, 0, 1000000);

   // walkers
   int nbItems = stream.read_int(2, 0, 65535);
   for (int i = 0; i < nbItems; ++i)
   {
      // std::cout << "READ WALKER @" << stream.tell() << std::endl;
      Walker &walker = Walker::unserialize_all(stream);
      _d->walkerList.push_back(&walker);
   }

   // overlays
   nbItems = stream.read_int(2, 0, 65535);
   for (int i = 0; i < nbItems; ++i)
   {
      // std::cout << "READ OVERLAY @" << stream.tell() << std::endl;
      LandOverlay &overlay = LandOverlay::unserialize_all(stream);
      _d->overlayList.push_back(&overlay);
   }

   // set all pointers to overlays&walkers
   stream.set_dangling_pointers(false); // ignore missing pointers

   // finalize the buildings
   for( LandOverlays::iterator itLLO = _d->overlayList.begin(); 
        itLLO!=_d->overlayList.end(); ++itLLO)
   {
      (*itLLO)->build( (*itLLO)->getTile().getIJ());
   }
}

TilePos City::getRoadEntry() const
{
  return _d->roadEntry;
}

TilePos City::getRoadExitIJ() const
{
  return TilePos( _roadExitI, _roadExitJ );
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

void City::addWalker( Walker& walker )
{
  _d->walkerList.push_back( &walker );
}

void City::removeWalker( Walker& walker )
{
  _d->walkerList.remove( &walker );
}

void City::setCameraStartIJ(const unsigned int i, const unsigned int j) {_cameraStartI = i; _cameraStartJ = j;}
void City::setCameraStartIJ(const TilePos pos) {_cameraStartI = pos.getI(); _cameraStartJ = pos.getJ();}
unsigned int City::getCameraStartI() const {return _cameraStartI;}
unsigned int City::getCameraStartJ() const {return _cameraStartJ;}
TilePos City::getCameraStartIJ() const {return TilePos(_cameraStartI,_cameraStartJ);}

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