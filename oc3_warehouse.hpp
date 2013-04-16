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


#ifndef WAREHOUSE_HPP
#define WAREHOUSE_HPP

#include "oc3_building.hpp"
#include "oc3_enums.hpp"
#include "oc3_good.hpp"

#include <string>
#include <list>



class WarehouseTile
{
public:
   WarehouseTile(const int i, const int j);
   void computePicture();

   int _i;
   int _j;
   GoodStock _stock;
   Picture _picture;
};


// implementation of the GoodStore for the Warehouse
class Warehouse;
class WarehouseStore: public GoodStore
{
public:
   using GoodStore::applyStorageReservation;
   using GoodStore::applyRetrieveReservation;

   WarehouseStore();

   void init(Warehouse &_warehouse);

   int getCurrentQty(const GoodType &goodType);

   // returns the max quantity that can be stored now
   int getMaxStore(const GoodType goodType);

   // store/retrieve
   void applyStorageReservation(GoodStock &stock, const long reservationID);
   void applyRetrieveReservation(GoodStock &stock, const long reservationID);

private:
   Warehouse *_warehouse;
};


class Warehouse: public WorkingBuilding
{
   friend class WarehouseStore;

public:
   Warehouse();
   Warehouse* clone() const;
   void init();

   void timeStep(const unsigned long time);
   void computePictures();
   WarehouseStore& getGoodStore();

private:
   Animation _animFlag;  // the flag above the warehouse
   std::vector<WarehouseTile> _subTiles;
   WarehouseStore _goodStore;
};



#endif
