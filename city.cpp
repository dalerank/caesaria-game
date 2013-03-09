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



#include <city.hpp>

#include <building_data.hpp>
#include <path_finding.hpp>
#include <exception.hpp>
#include <iostream>
#include <set>


City::City()
{
   _time = 0;
   _month = 0;
   _roadEntryI = 0;
   _roadEntryJ = 0;
   _roadExitI = 0;
   _roadExitJ = 0;
   _boatEntryI = 0;
   _boatEntryJ = 0;
   _boatExitI = 0;
   _boatExitJ = 0;
   _funds = 1000;
   _population = 0;
   _taxRate = 700;
   _climate = C_CENTRAL;
}


void City::timeStep()
{
   // CALLED 11 time/second
   _time += 1;

   if (_time % 110 == 1)
   {
      // every X seconds
      _month++;
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
   while (overlayIt != _overlayList.end())
   {
      LandOverlay &overlay = **overlayIt;
      overlay.timeStep(_time);

      if (overlay.isDeleted())
      {
         // remove the overlay from the overlay list
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
}


std::list<Walker*>& City::getWalkerList()
{
   return _walkerList;
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


Tilemap& City::getTilemap()
{
   return _tilemap;
}

int City::getRoadEntryI() const
{
   return _roadEntryI;
}
int City::getRoadEntryJ() const
{
   return _roadEntryJ;
}
int City::getRoadExitI() const
{
   return _roadExitI;
}
int City::getRoadExitJ() const
{
   return _roadExitJ;
}
int City::getBoatEntryI() const
{
   return _boatEntryI;
}
int City::getBoatEntryJ() const
{
   return _boatEntryJ;
}
int City::getBoatExitI() const
{
   return _boatExitI;
}
int City::getBoatExitJ() const
{
   return _boatExitJ;
}

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
   return _funds;
}

void City::setFunds(const long funds)
{
   _funds = funds;
}

long City::getPopulation() const
{
   return _population;
}

void City::setPopulation(const long population)
{
   _population = population;
}


void City::build(Construction &buildInstance, const int i, const int j)
{
   BuildingData &buildingData = BuildingDataHolder::instance().getData(buildInstance.getType());
   // make new building
   Construction *building = (Construction *) buildInstance.clone();
   building->build(i, j);
   _overlayList.push_back(building);
   _funds -= buildingData.getCost();
}

void City::clearLand(const int i, const int j)
{
   Tile& cursorTile = _tilemap.at(i, j);
   TerrainTile& terrain = cursorTile.get_terrain();

   if (terrain.isDestructible())
   {
      int size = 1;
      int i1 = i;
      int j1 = j;

      LandOverlay* overlay = terrain.getOverlay();
      if (overlay != NULL)
      {
         size = overlay->getSize();
         i1 = overlay->getTile().getI();
         j1 = overlay->getTile().getJ();
         overlay->destroy();
         _overlayList.remove(overlay);
         delete overlay;
      }

      std::list<Tile*> clearedTiles = _tilemap.getFilledRectangle(i1, j1, i1+size-1, j1+size-1);
      for (std::list<Tile*>::iterator itTile = clearedTiles.begin(); itTile!=clearedTiles.end(); ++itTile)
      {
         Tile &tile = **itTile;
         tile.set_master_tile(NULL);
         TerrainTile &terrain = tile.get_terrain();
         terrain.setTree(false);
         terrain.setBuilding(false);
         terrain.setRoad(false);
         terrain.setOverlay(NULL);

         // choose a random landscape picture:
         // flat land1a 2-9;
         // wheat: land1a 18-29;
         // green_something: land1a 62-119;  => 58
         // green_flat: land1a 232-289; => 58

         // choose a random background image, green_something 62-119 or green_flat 232-240
         std::string res_pfx = "land1a";
         int res_id = 0;
         if (rand()%10 > 6)
         {
            // 30% => choose green_sth 62-119
            res_id = 62+rand()%(119-62+1);
         }
         else
         {
            // 70% => choose green_flat 232-289
            res_id = 232+rand()%(289-232+1);
         }
         tile.set_picture(&PicLoader::instance().get_picture(res_pfx, res_id));
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

   _funds += taxes;

   std::cout << "Monthly Taxes=" << taxes << std::endl;
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
   stream.write_int(_funds, 4, 0, 1000000);
   stream.write_int(_population, 4, 0, 1000000);

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
   _funds = stream.read_int(4, 0, 1000000);
   _population = stream.read_int(4, 0, 1000000);

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
      LandOverlay &overlay = **itLLO;
      int i = overlay.getTile().getI();
      int j = overlay.getTile().getJ();

      overlay.build(i, j);
   }

}

