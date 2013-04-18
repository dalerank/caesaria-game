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

#include "tilemap.hpp"
#include "walker.hpp"
#include "building.hpp"
#include "service_building.hpp"
#include "factory_building.hpp"
#include "house.hpp"
#include "enums.hpp"
#include "serializer.hpp"
#include "oc3_signals.h"

#include <list>

class TilePos;

class City : public Serializable
{
public:
   City();
   ~City();

   void timeStep();  // performs one simulation step
   void monthStep();

   std::list<Walker*>& getWalkerList();
   std::list<Walker*> getWalkerList( const WalkerType type );
   std::list<LandOverlay*>& getOverlayList();
   std::list<LandOverlay*> getBuildingList( const BuildingType buildingType );

   void setRoadEntryIJ(const int i, const int j);
   void setRoadExitIJ(const int i, const int j);
   void setBoatEntryIJ(const int i, const int j);
   void setBoatExitIJ(const int i, const int j);

   int getRoadEntryI() const;
   int getRoadEntryJ() const;
   TilePos getRoadEntryIJ() const;
   
   int getRoadExitI() const;
   int getRoadExitJ() const;
   TilePos getRoadExitIJ() const;
   
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
   unsigned long getMonth() const;

   Tilemap& getTilemap();

   void serialize(OutputSerialStream &stream);
   void unserialize(InputSerialStream &stream);

   // add construction
   void build(Construction &buildInstance, const TilePos& pos );

   //
   void disaster( const TilePos& pos, DisasterType type );
   // remove construction
   void clearLand( const TilePos& pos );
   // collect taxes from all houses
   void collectTaxes();

   unsigned long getTime();

oc3_signals public:
   Signal1<int>& onPopulationChanged();
   Signal1<int>& onFundsChanged();
   Signal1<int>& onMonthChanged();

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
   int _taxRate;
   
   void _calculatePopulation();
   void _createImigrants();
   void recomputeRoadsForAll();

   class Impl;
   ScopedPtr< Impl > _d;
};


#endif
