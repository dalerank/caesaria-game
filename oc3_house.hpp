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


#ifndef HOUSE_HPP
#define HOUSE_HPP

#include "oc3_building.hpp"

class HouseLevelSpec;

class House : public Building
{
public:
   enum { smallHovel=1 } Level;

   House( const int houseId );
   House* clone() const;

   void timeStep(const unsigned long time);

   virtual GuiInfoBox* makeInfoBox( Widget* parent );

   SimpleGoodStore& getGoodStore();

   // return the current house level
   HouseLevelSpec& getLevelSpec();

   virtual void applyService(ServiceWalker &walker);
   virtual float evaluateService(ServiceWalker &walker);
   virtual void buyMarket(ServiceWalker &walker);

   bool hasServiceAccess(const ServiceType service);
   int getServiceAccess(const ServiceType service);
   void setServiceAccess(const ServiceType service, const int access);

   void levelUp();
   void levelDown();

   void destroy();

   int getNbHabitants();
   int getMaxHabitants();
   void addHabitants( const Uint8 newHabitCount );
   int collectTaxes();  // compute the tax level for this house, called once per month

   Uint8 getMaxDistance2Road() const; 

   bool isWalkable() const;

private:
   SimpleGoodStore _goodStore;
   std::map<ServiceType, int> _serviceAccessMap;  // value=access to the service (0=no access, 100=good access)
   int _currentHabitants;
   int _maxHabitants;
   int _freeWorkersCount;
   int _houseId;  // pictureId
   int _houseLevel;
   HouseLevelSpec* _houseLevelSpec;  // characteristics of the current house level
   HouseLevelSpec* _nextHouseLevelSpec;  // characteristics of the house level+1
   int _picIdOffset;

   void _update();
};


#endif
