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

#include "house.hpp"

#include <iostream>

#include "house_level.hpp"
#include "gui_info_box.hpp"
#include "scenario.hpp"
#include "exception.hpp"

static const char* rcGrourName = "housng1a";

House::House(const int houseId) : Building()
{
   setType(B_HOUSE);
   _houseId = houseId;
   _houseLevel = HouseLevelSpec::getHouseLevel( houseId );
   _houseLevelSpec = &HouseLevelSpec::getHouseLevelSpec(_houseLevel);
   _nextHouseLevelSpec = &HouseLevelSpec::getHouseLevelSpec(_houseLevel+1);
   _name = _houseLevelSpec->getLevelName();
   _currentHabitants = 0;
   // _fireLevel = 98;

   _goodStore.setMaxQty(10000);  // no limit
   _goodStore.setMaxQty(G_WHEAT, 100);
   _goodStore.setMaxQty(G_FISH, 0);
   _goodStore.setMaxQty(G_MEAT, 0);
   _goodStore.setMaxQty(G_FRUIT, 0);
   _goodStore.setMaxQty(G_VEGETABLE, 0);
   _goodStore.setMaxQty(G_POTTERY, 0);
   _goodStore.setMaxQty(G_FURNITURE, 0);
   _goodStore.setMaxQty(G_OIL, 0);
   _goodStore.setMaxQty(G_WINE, 0);

   // init the service access
   for (int i = 0; i<S_MAX; ++i)
   {
      // for every service type
      ServiceType service = ServiceType(i);
      _serviceAccessMap[service] = 0;
   }

   _update();
}

House* House::clone() const
{
   House *res = new House(this->_houseId);
   return res;
}


void House::timeStep(const unsigned long time)
{
   // _goodStockList[G_WHEAT]._currentQty -= _currentHabitants;  // to do once every month!
   if (time % 16 == 0)
   {
      // consume services
      for (int i = 0; i < S_MAX; ++i)
      {
         ServiceType service = (ServiceType) i;
         _serviceAccessMap[service] = std::max(_serviceAccessMap[service] - 1, 0);
      }

      // consume goods
      for (int i = 0; i < G_MAX; ++i)
      {
         GoodType goodType = (GoodType) i;
         int qty = std::max(_goodStore.getCurrentQty(goodType) - 1, 0);
         _goodStore.setCurrentQty(goodType, qty);
      }
   }

   if (time % 64 == 0)
   {
      bool validate = _houseLevelSpec->checkHouse(*this);
      if (! validate)
      {
         levelDown();
      }
      else
      {
         validate = _nextHouseLevelSpec->checkHouse(*this);
         if( validate && _currentHabitants > 0 )
         {
            levelUp();
         }
      }
   }
   if( _currentHabitants > 0 )
       Building::timeStep( time );
}


GuiInfoBox* House::makeInfoBox( Widget* parent )
{
  return new InfoBoxHouse( parent, *this);
}


SimpleGoodStore& House::getGoodStore()
{
   return _goodStore;
}


HouseLevelSpec& House::getLevelSpec()
{
   return *_houseLevelSpec;
}


void House::levelUp()
{
   _houseLevel++;
   _houseLevelSpec = &HouseLevelSpec::getHouseLevelSpec(_houseLevel);
   _nextHouseLevelSpec = &HouseLevelSpec::getHouseLevelSpec(_houseLevel+1);

   switch (_houseLevel)
   {
   case 1:
      _houseId = 1;
      break;
   case 2:
      _houseId = 3;
      break;
   case 3:
      _houseId = 7;
      break;
   case 4:
      _houseId = 10;
      break;
   case 5:
      _houseId = 13;
      break;
   case 6:
      _houseId = 15;
      break;
   case 7:
      _houseId = 19;
      break;
   case 8:
      _houseId = 21;
      break;
   case 9:
      _houseId = 25;
      break;
   case 10:
      _houseId = 27;
      break;
   }
   _update();
}


void House::levelDown()
{
   _houseLevel--;
   _houseLevelSpec = &HouseLevelSpec::getHouseLevelSpec(_houseLevel);
   _houseLevelSpec = &HouseLevelSpec::getHouseLevelSpec(_houseLevel+1);

   switch (_houseLevel)
   {
   case 1:
      _houseId = 1;
      break;
   case 2:
      _houseId = 3;
      break;
   case 3:
      _houseId = 7;
      break;
   case 4:
      _houseId = 10;
      break;
   case 5:
      _houseId = 13;
      break;
   case 6:
      _houseId = 15;
      break;
   case 7:
      _houseId = 19;
      break;
   case 8:
      _houseId = 21;
      break;
   case 9:
      _houseId = 25;
      break;
   case 10:
      _houseId = 27;
      break;
   }
   _update();
}

void House::buyMarket(ServiceWalker &walker)
{
   // std::cout << "House buyMarket" << std::endl;
   Market &market = dynamic_cast<Market&> (walker.getServiceBuilding());
   GoodStore &marketStore = market.getGoodStore();
   SimpleGoodStore &houseStore = getGoodStore();
   for (int i = 0; i < G_MAX; ++i)
   {
      GoodType goodType = (GoodType) i;
      int houseQty = houseStore.getCurrentQty(goodType);
      int houseSafeQty = _houseLevelSpec->computeMonthlyConsumption(*this, goodType)
                         + _nextHouseLevelSpec->computeMonthlyConsumption(*this, goodType);
      int marketQty = marketStore.getCurrentQty(goodType);
      if (houseQty < houseSafeQty && marketQty > 0)
      {
         int qty = std::min(houseSafeQty - houseQty, marketQty);
         GoodStock stock(goodType, qty);
         marketStore.retrieve(stock, qty);
         houseStore.store(stock, qty);
      }
   }
}

void House::applyService(ServiceWalker &walker)
{
  Building::applyService(walker);  // handles basic services, and remove service reservation

  ServiceType service = walker.getService();
  switch (service)
  {
  case S_WELL:
  case S_FOUNTAIN:
  case S_TEMPLE_NEPTUNE:
  case S_TEMPLE_CERES:
  case S_TEMPLE_VENUS:
  case S_TEMPLE_MARS:
  case S_TEMPLE_MERCURE:
  case S_DOCTOR:
  case S_BARBER:
  case S_BATHS:
  case S_HOSPITAL:
  case S_SCHOOL:
  case S_LIBRARY:
  case S_COLLEGE:
  case S_THEATER:
  case S_AMPHITHEATER:
  case S_COLLOSSEUM:
  case S_HIPPODROME:
  case S_FORUM:
    setServiceAccess(service, 100);
    break;
  case S_MARKET:
    buyMarket(walker);
    break;
  case S_SENATE:
  case S_TEMPLE_ORACLE:  
  case S_ENGINEER:
  case S_PREFECT:
  case S_MAX:
    break;
  }
}

float House::evaluateService(ServiceWalker &walker)
{
   float res = 0.0;
   ServiceType service = walker.getService();
   if (_reservedServices.count(service) == 1)
   {
      // service is already reserved
      return 0.0;
   }

   if (service == S_ENGINEER)
   {
      res = _damageLevel;
   }
   else if (service == S_PREFECT)
   {
      res = _fireLevel;
   }
   else if (service == S_MARKET)
   {
      Market &market = dynamic_cast<Market&> (walker.getServiceBuilding());
      GoodStore &marketStore = market.getGoodStore();
      SimpleGoodStore &houseStore = getGoodStore();
      for (int i = 0; i < G_MAX; ++i)
      {
         GoodType goodType = (GoodType) i;
         int houseQty  = houseStore.getCurrentQty(goodType);
         int houseSafeQty = _houseLevelSpec->computeMonthlyConsumption(*this, goodType)
                            + _nextHouseLevelSpec->computeMonthlyConsumption(*this, goodType);
         int marketQty = marketStore.getCurrentQty(goodType);
         if (houseQty < houseSafeQty && marketQty > 0)
         {
            res += std::min(houseSafeQty - houseQty, marketQty);
         }
      }
   }
   else
   {
      return _houseLevelSpec->evaluateServiceNeed(*this, service);
   }

   // std::cout << "House evaluateService " << service << "=" << res << std::endl;

   return res;
}

bool House::hasServiceAccess(const ServiceType service)
{
   bool res = (_serviceAccessMap[service] > 0);
   return res;
}

int House::getServiceAccess(const ServiceType service)
{
   int res = _serviceAccessMap[service];
   return res;
}

void House::setServiceAccess(const ServiceType service, const int access)
{
   _serviceAccessMap[service] = access;
}

int House::getNbHabitants()
{
   return _currentHabitants;
}

int House::getMaxHabitants()
{
   return _maxHabitants;
}

int House::collectTaxes()
{
   int res = 0;
   if (_serviceAccessMap[S_FORUM] > 0)
   {
      // this house pays taxes
      res = _houseLevelSpec->getTaxRate() * _currentHabitants;
   }
   return res;
}

void House::_update()
{
    Uint8 picId = ( _houseId == smallHovel && _currentHabitants == 0 ) ? 45 : _houseId; 
    setPicture( PicLoader::instance().get_picture( rcGrourName, picId ) );
    _size = (_picture->get_surface()->w+2)/60;
    _maxHabitants = _houseLevelSpec->getMaxHabitantsByTile() * _size * _size;
}

Uint8 House::getMaxDistance2Road() const
{
  return 2;
}

void House::addHabitants( const Uint8 newHabitCount )
{
    _currentHabitants = (std::min)( _currentHabitants + newHabitCount, _maxHabitants );
    _update();
}