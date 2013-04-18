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
#include "oc3_emigrant.hpp"
#include "oc3_positioni.hpp"
#include "oc3_constructionmanager.hpp"

class City::Impl
{
public:
    Signal1<int> onPopulationChangedSignal;
    Signal1<int> onFundsChangedSignal;
    Signal1<int> onMonthChangedSignal;

    int population;
    long funds;  // amount of money
    unsigned long month; // number of months since start
};

City::City() : _d( new Impl )
{
   _time = 0;
   _d->month = 0;
   _roadEntryI = 0;
   _roadEntryJ = 0;
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
}

void City::timeStep()
{
   // CALLED 11 time/second
   _time += 1;

   if( _time % 22 == 1 )
   {
	   _createImigrants();
   }

   if( _time % 110 == 1 )
   {
      // every X seconds
      _d->month++;
      monthStep();
   }

   std::list<Walker*>::iterator walkerIt = _walkerList.begin();
   while (walkerIt != _walkerList.end())
   {
      Walker &walker = **walkerIt;
      walker.timeStep(_time);

      if (walker.isDeleted())
      {
         // remove the walker from the walkers list
         walkerIt = _walkerList.erase(walkerIt);
      }
      else
      {
         ++walkerIt;
      }
   }

   std::list<LandOverlay*>::iterator overlayIt = _overlayList.begin();
   while( overlayIt != _overlayList.end() )
   {
       (*overlayIt)->timeStep(_time);

       if( (*overlayIt)->isDeleted() )
       {
          // remove the overlay from the overlay list
           (*overlayIt)->destroy();
           delete (*overlayIt);

           overlayIt = _overlayList.erase(overlayIt);
       }
       else
       {
          ++overlayIt;
       }
   }
}

void City::monthStep()
{
   collectTaxes();
   _calculatePopulation();
   _d->onMonthChangedSignal.emit( _d->month );
}

void City::_createImigrants()
{
	Uint32 vacantPop=0;

	std::list<LandOverlay*> houses = getBuildingList(B_HOUSE);
    for( std::list<LandOverlay*>::iterator itHouse = houses.begin(); itHouse != houses.end(); ++itHouse )
    {
		House* house = dynamic_cast<House*>(*itHouse);
        if( house && house->getAccessRoads().size() > 0 )
        {
            vacantPop += house->getMaxHabitants() - house->getNbHabitants();
        }
    }

	if( vacantPop == 0 )
	{
		return;
	}

	std::list<Walker*> walkers = getWalkerList( WT_EMIGRANT );

	if( vacantPop <= walkers.size() )
	{
		return;
	}

    Tile& roadTile = _tilemap.at( _roadEntryI, _roadEntryJ );
    Road* roadEntry = dynamic_cast< Road* >( roadTile.get_terrain().getOverlay() );

    if( roadEntry )
    {
		vacantPop = std::max<Uint32>( 1, rand() % std::max<Uint32>( 1, vacantPop / 2 ) );
        Emigrant* ni = Emigrant::create( *roadEntry );
        _walkerList.push_back( ni );
    }    
}

std::list<Walker*>& City::getWalkerList()
{
   return _walkerList;
}

std::list<Walker*> City::getWalkerList( const WalkerType type )
{
	std::list<Walker*> res;

	Walker* walker = 0;
	for (std::list<Walker*>::iterator itWalker = _walkerList.begin(); itWalker != _walkerList.end(); ++itWalker )
	{
		// for each walker
		walker = *itWalker;
		if( walker != NULL && walker->getType() == type )
		{
			res.push_back(walker);
		}
	}

	return res;
}

std::list<LandOverlay*>& City::getOverlayList()
{
   return _overlayList;
}

unsigned long City::getTime()
{
   return _time;
}


std::list<LandOverlay*> City::getBuildingList(const BuildingType buildingType)
{
   std::list<LandOverlay*> res;

   for (std::list<LandOverlay*>::iterator itOverlay = _overlayList.begin(); itOverlay!=_overlayList.end(); ++itOverlay)
   {
      // for each overlay
      LandOverlay *overlay = *itOverlay;
      Construction *construction = dynamic_cast<Construction*>(overlay);
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
   for (std::list<LandOverlay*>::iterator itOverlay = _overlayList.begin(); itOverlay!=_overlayList.end(); ++itOverlay)
   {
      // for each overlay
      LandOverlay *overlay = *itOverlay;
      Construction *construction = dynamic_cast<Construction*>(overlay);
      if (construction != NULL)
      {
         // overlay matches the filter
         construction->computeAccessRoads();
	 // for some constructions we need to update picture
	 if (construction->getType() == B_ROAD) construction->setPicture(dynamic_cast<Road*>(construction)->computePicture());
      }
   }
}

Tilemap& City::getTilemap()
{
   return _tilemap;
}

int City::getRoadEntryI() const { return _roadEntryI; }
int City::getRoadEntryJ() const { return _roadEntryJ; }
int City::getRoadExitI() const  { return _roadExitI;  }
int City::getRoadExitJ() const  { return _roadExitJ;  }
int City::getBoatEntryI() const { return _boatEntryI; }
int City::getBoatEntryJ() const { return _boatEntryJ; }
int City::getBoatExitI() const  { return _boatExitI;  }
int City::getBoatExitJ() const  { return _boatExitJ;  }

ClimateType City::getClimate() const
{
   return _climate;
}
void City::setClimate(const ClimateType climate)
{
   _climate = climate;
}

void City::setRoadEntryIJ(const int i, const int j)
{
   _roadEntryI = i;
   _roadEntryJ = j;
}

void City::setRoadExitIJ(const int i, const int j)
{
   _roadExitI = i;
   _roadExitJ = j;
}

void City::setBoatEntryIJ(const int i, const int j)
{
   _boatEntryI = i;
   _boatEntryJ = j;
}

void City::setBoatExitIJ(const int i, const int j)
{
   _boatExitI = i;
   _boatExitJ = j;
}

int City::getTaxRate() const
{
   return _taxRate;
}

void City::setTaxRate(const int taxRate)
{
   _taxRate = taxRate;
}

long City::getFunds() const
{
   return _d->funds;
}

void City::setFunds(const long funds)
{
   _d->funds = funds;
}

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
   _overlayList.push_back(building);
   _d->funds -= buildingData.getCost();
   _d->onFundsChangedSignal.emit( _d->funds );
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
         overlay->destroy();
         _overlayList.remove(overlay);
         delete overlay;
      }

      std::list<Tile*> clearedTiles = _tilemap.getFilledRectangle( rPos, Size( size ) );
      for (std::list<Tile*>::iterator itTile = clearedTiles.begin(); itTile!=clearedTiles.end(); ++itTile)
      {
         Tile &tile = **itTile;
         tile.set_master_tile(NULL);
         TerrainTile &terrain = tile.get_terrain();
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

         // choose a random background image, green_something 62-119 or green_flat 232-240
         std::string res_pfx = "land1a";
         int res_id = 0;
         if (rand() % 10 > 6)
         {
            // 30% => choose green_sth 62-119
            res_id = 62 + rand() % (119 - 62 + 1);
         }
         else
         {
            // 70% => choose green_flat 232-289
            res_id = 232 + rand() % (289 - 232 + 1);
         }
         tile.set_picture(&PicLoader::instance().get_picture(res_pfx, res_id));
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
   stream.write_int(_roadEntryI, 2, 0, 1000);
   stream.write_int(_roadEntryJ, 2, 0, 1000);
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
   stream.write_int(_walkerList.size(), 2, 0, 65535);
   for (std::list<Walker*>::iterator itWalker = _walkerList.begin(); itWalker != _walkerList.end(); ++itWalker)
   {
      // std::cout << "WRITE WALKER @" << stream.tell() << std::endl;
      Walker &walker = **itWalker;
      walker.serialize(stream);
   }

   // overlays
   stream.write_int(_overlayList.size(), 2, 0, 65535);
   for (std::list<LandOverlay*>::iterator itOverlay = _overlayList.begin(); itOverlay != _overlayList.end(); ++itOverlay)
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
   _roadEntryI = stream.read_int(2, 0, 1000);
   _roadEntryJ = stream.read_int(2, 0, 1000);
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
      _walkerList.push_back(&walker);
   }

   // overlays
   nbItems = stream.read_int(2, 0, 65535);
   for (int i = 0; i < nbItems; ++i)
   {
      // std::cout << "READ OVERLAY @" << stream.tell() << std::endl;
      LandOverlay &overlay = LandOverlay::unserialize_all(stream);
      _overlayList.push_back(&overlay);
   }

   // set all pointers to overlays&walkers
   stream.set_dangling_pointers(false); // ignore missing pointers

   // finalize the buildings
   std::list<LandOverlay*> llo = _overlayList;
   for (std::list<LandOverlay*>::iterator itLLO = llo.begin(); itLLO!=llo.end(); ++itLLO)
   {
      (*itLLO)->build( (*itLLO)->getTile().getIJ());
   }
}

TilePos City::getRoadEntryIJ() const
{
    return TilePos( _roadEntryI, _roadEntryJ );
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