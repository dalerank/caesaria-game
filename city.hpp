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


#ifndef CITY_HPP
#define CITY_HPP


#include <tilemap.hpp>
#include <walker.hpp>
#include <building.hpp>
#include <service_building.hpp>
#include <factory_building.hpp>
#include <house.hpp>
#include <enums.hpp>
#include <serializer.hpp>

#include <list>



class City : public Serializable
{
public:
   City();

   void timeStep();  // performs one simulation step
   void monthStep();

   std::list<Walker*>& getWalkerList();
   std::list<LandOverlay*>& getOverlayList();
   std::list<LandOverlay*> getBuildingList(const BuildingType buildingType);

   void setRoadEntryIJ(const int i, const int j);
   void setRoadExitIJ(const int i, const int j);
   void setBoatEntryIJ(const int i, const int j);
   void setBoatExitIJ(const int i, const int j);

   int getRoadEntryI() const;
   int getRoadEntryJ() const;
   int getRoadExitI() const;
   int getRoadExitJ() const;
   int getBoatEntryI() const;
   int getBoatEntryJ() const;
   int getBoatExitI() const;
   int getBoatExitJ() const;

   ClimateType getClimate() const;
   void setClimate(const ClimateType);

   int getTaxRate() const;
   void setTaxRate(const int taxRate);
   long getFunds() const;
   void setFunds(const long funds);
   long getPopulation() const;
   void setPopulation(const long population);
   unsigned long getMonth() const {return _month;}

   Tilemap& getTilemap();

   void serialize(OutputSerialStream &stream);
   void unserialize(InputSerialStream &stream);

   // add construction
   void build(Construction &buildInstance, const int i, const int j);
   // remove construction
   void clearLand(const int i, const int j);
   // collect taxes from all houses
   void collectTaxes();

   unsigned long getTime();

private:
   int _roadEntryI, _roadEntryJ;
   int _roadExitI, _roadExitJ;
   int _boatEntryI, _boatEntryJ;
   int _boatExitI, _boatExitJ;

   ClimateType _climate;
   Tilemap _tilemap;
   std::list<Walker*> _walkerList;
   std::list<LandOverlay*> _overlayList;
   unsigned long _time;  // number of timesteps since start
   unsigned long _month; // number of months since start
   long _funds;  // amount of money
   long _population;  // number of inhabitants
   int _taxRate;
   
   void calculatePopulation();
};


#endif
