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



#include "oc3_good.hpp"

#include "oc3_gettext.hpp"
#include "oc3_exception.hpp"
#include "oc3_variant.hpp"
#include "oc3_stringhelper.hpp"

#include <iostream>
#include <algorithm>

std::string Good::getName()
{
   return _name;
}

int Good::getImportPrice()
{
   return _importPrice;
}

int Good::getExportPrice()
{
   return _exportPrice;
}

bool Good::isAllowUsage()
{
   return _allowUsage;
}

bool Good::isAllowStorage()
{
   return _allowStorage;
}

bool Good::isAllowImport()
{
   return _allowImport;
}

bool Good::isAllowExport()
{
   return _allowExport;
}

void Good::init(const GoodType &goodType)
{
   _goodType = goodType;
   _name = "";
   _importPrice = 0;
   _exportPrice = 0;

   _allowUsage = true;
   _allowStorage = true;
   _allowImport = false;
   _allowExport = false;

   _usageQuota = 100;     // full usage
   _importTreshold = 16;  // lots of import
   _exportTreshold = 0;   // lots of export

   // init static values
   switch (goodType)
   {
   case G_NONE:
      _name = _("None");
      break;
   case G_WHEAT:
      _name = _("Wheat");
      _importPrice = 28;
      _exportPrice = 22;
      break;
   case G_FISH:
      _name = _("Fish");  // no import/export!
      break;
   case G_MEAT:
      _name = _("Meat");
      _importPrice = 44;
      _exportPrice = 36;
      break;
   case G_FRUIT:
      _name = _("Fruits");
      _importPrice = 38;
      _exportPrice = 30;
      break;
   case G_VEGETABLE:
      _name = _("Vegetables");
      _importPrice = 38;
      _exportPrice = 30;
      break;
   case G_OLIVE:
      _name = _("Olives");
      _importPrice = 42;
      _exportPrice = 34;
      break;
   case G_OIL:
      _name = _("Oil");
      _importPrice = 180;
      _exportPrice = 140;
      break;
   case G_GRAPE:
      _name = _("Grape");
      _importPrice = 44;
      _exportPrice = 36;
      break;
   case G_WINE:
      _name = _("Vine");
      _importPrice = 215;
      _exportPrice = 160;
      break;
   case G_TIMBER:
      _name = _("Timber");
      _importPrice = 50;
      _exportPrice = 35;
      break;
   case G_FURNITURE:
      _name = _("Furniture");
      _importPrice = 200;
      _exportPrice = 150;
      break;
   case G_CLAY:
      _name = _("Clay");
      _importPrice = 40;
      _exportPrice = 30;
      break;
   case G_POTTERY:
      _name = _("Pottery");
      _importPrice = 180;
      _exportPrice = 140;
      break;
   case G_IRON:
      _name = _("Iron");
      _importPrice = 60;
      _exportPrice = 40;
      break;
   case G_WEAPON:
      _name = _("Weapon");
      _importPrice = 250;
      _exportPrice = 180;
      break;
   case G_MARBLE:
      _name = _("Marble");
      _importPrice = 200;
      _exportPrice = 140;
      break;
   default:
      THROW("Unexpected goodType:" << goodType);
   }
}


GoodStock::GoodStock()
{
   _goodType = G_NONE;
   _maxQty = 0;
   _currentQty = 0;
}

GoodStock::GoodStock(const GoodType &goodType, const int maxQty, const int currentQty)
{
   _goodType = goodType;
   _maxQty = maxQty;
   _currentQty = currentQty;
}

void GoodStock::addStock(GoodStock &stock, const int iAmount)
{
  if (stock._goodType == G_NONE)
  {
     // nothing to add => nothing to do!
     return;
  }
  if (_goodType != G_NONE && _goodType != stock._goodType) 
  {
    std::string errorStr = StringHelper::format( 0xff, "GoodTypes do not match: %d vs %d", _goodType, stock._goodType );
    _OC3_DEBUG_BREAK_IF( errorStr.c_str() );
  }

  int amount = iAmount;  // not const
  if (amount == -1)
  {
     amount = stock._currentQty;
  }
  if (amount > stock._currentQty)
  {
    _OC3_DEBUG_BREAK_IF( "GoodStock:Not enough quantity in stock." );
    return;
  }
  if (amount+_currentQty > _maxQty)
  {
    _OC3_DEBUG_BREAK_IF( "GoodStock:Not enough free room for storage");
    return;
  }

  _goodType = stock._goodType;  // in case goodType was G_NONE
  _currentQty += amount;
  stock._currentQty -= amount;
  
  if (stock._currentQty == 0)
  {
     stock._goodType = G_NONE;
  }
}

void GoodStock::save( VariantList& stream ) const
{
  stream.push_back( (int)_goodType );
  stream.push_back( _maxQty );
  stream.push_back( _currentQty );
}

void GoodStock::load( const VariantList& stream )
{
  if( stream.size() < 3 )
    return;

  VariantList::const_iterator it=stream.begin();
  _goodType = (GoodType)(*it).toInt(); it++;
  _maxQty = (*it).toInt(); it++;
  _currentQty = (*it).toInt();
}


GoodStore::GoodStore()
{
   _nextReservationID = 1;
   _devastation = false;
}


int GoodStore::getMaxRetrieve(const GoodType goodType)
{
   // current good quantity
   int qty = getCurrentQty(goodType);

   // remove all retrieval reservations
   for (std::map<long, GoodStock>::iterator reservationIt = _retrieveReservations.begin(); reservationIt != _retrieveReservations.end(); ++reservationIt)
   {
      GoodStock &reservationStock = reservationIt->second;
      qty -= reservationStock._currentQty;
   }

   return qty;
}


long GoodStore::reserveStorage(GoodStock &stock)
{
  long reservationID = 0;

  // current free capacity
  if( getMaxStore(stock._goodType) >= stock._currentQty )
  {
    // the stock can be stored!
    reservationID = _nextReservationID;
    _storeReservations.insert(std::make_pair(reservationID, stock));
    _nextReservationID++;
  }
  // std::cout << "GoodStore, reserve store qty=" << stock._currentQty << " resID=" << reservationID << std::endl;

  return reservationID;
}


long GoodStore::reserveRetrieval(GoodStock &stock)
{
   long reservationID = 0;

   // current good quantity
   if (getMaxRetrieve(stock._goodType) >= stock._currentQty)
   {
      // the stock can be retrieved!
      reservationID = _nextReservationID;
      _retrieveReservations.insert(std::make_pair(reservationID, stock));
      _nextReservationID++;
   }
   // std::cout << "GoodStore, reserve retrieve qty=" << stock._currentQty << " resID=" << reservationID << std::endl;

   return reservationID;
}


GoodStock GoodStore::getStorageReservation(const long reservationID, const bool pop)
{
   std::map<long, GoodStock>::iterator mapIt = _storeReservations.find(reservationID);

   if (mapIt == _storeReservations.end())
   {
      THROW("Unknown reservationID");
   }

   GoodStock reservedStock = mapIt->second;

   if (pop)
   {
      _storeReservations.erase(mapIt);
   }

   return reservedStock;
}


GoodStock GoodStore::getRetrieveReservation(const long reservationID, const bool pop)
{
   std::map<long, GoodStock>::iterator mapIt = _retrieveReservations.find(reservationID);

   if (mapIt == _retrieveReservations.end())
   {
      THROW("Unknown reservationID");
   }

   GoodStock reservedStock = mapIt->second;

   if (pop)
   {
      _retrieveReservations.erase(mapIt);
   }

   return reservedStock;
}


void GoodStore::applyStorageReservation(SimpleGoodStore &goodStore, const long reservationID)
{
   GoodStock reservedStock = getStorageReservation(reservationID);
   GoodStock &stock = goodStore.getStock(reservedStock._goodType);

   applyStorageReservation(stock, reservationID);
}


void GoodStore::applyRetrieveReservation(SimpleGoodStore &goodStore, const long reservationID)
{
   GoodStock reservedStock = getRetrieveReservation(reservationID);
   GoodStock &stock = goodStore.getStock(reservedStock._goodType);

   applyRetrieveReservation(stock, reservationID);
}


void GoodStore::store(GoodStock &stock, const int amount)
{
  GoodStock reservedStock;
  reservedStock._goodType = stock._goodType;
  reservedStock._currentQty = amount;

  long reservationID = reserveStorage(reservedStock);
  
  _OC3_DEBUG_BREAK_IF( reservationID == 0 && "GoodStore:Impossible to store goods");
  if( reservationID > 0 )
  {
    applyStorageReservation(stock, reservationID);
  }
}

void GoodStore::retrieve(GoodStock &stock, int amount)
{
  GoodStock reservedStock;
  reservedStock._goodType = stock._goodType;
  reservedStock._currentQty = amount;

  long reservationID = reserveRetrieval(reservedStock);
  _OC3_DEBUG_BREAK_IF( reservationID == 0 && "GoodStore:Impossible to retrieve goods");
  
  if( reservationID > 0 )
  {
    applyRetrieveReservation(stock, reservationID);
  }
}

void GoodStore::storeAll(SimpleGoodStore &goodStore)
{
   for (int n = 1; n<G_MAX; ++n)
   {
      // for all types of good (except G_NONE)
      GoodType goodType = (GoodType) n;
      GoodStock stock = goodStore.getStock(goodType);
      if (stock._currentQty != 0)
      {
         store(stock, stock._currentQty);
      }
   }
}
 
void GoodStore::save( VariantMap& stream) const 
{
  stream[ "nextReservationId" ] = static_cast<int>(_nextReservationID);
  
  VariantList vm_storereservations;
  for( std::map<long, GoodStock>::const_iterator itRes = _storeReservations.begin(); itRes != _storeReservations.end(); itRes++)
  {
    VariantList vm_stocksave;
    itRes->second.save( vm_stocksave );
    vm_storereservations.push_back( itRes->first );
    vm_storereservations.push_back( vm_stocksave );
  }
  stream[ "storeReservations" ] = vm_storereservations;
  stream[ "devastation" ] = _devastation;

  VariantList vm_retrievereservations;
  for (std::map<long, GoodStock>::const_iterator itRes = _retrieveReservations.begin(); itRes != _retrieveReservations.end(); itRes++)
  {
    VariantList vm_stockrtvsave;
    itRes->second.save( vm_stockrtvsave );
    vm_retrievereservations.push_back( itRes->first );
    vm_retrievereservations.push_back( vm_stockrtvsave );
  }
  stream[ "retrieveReservation" ] = vm_retrievereservations;
}

void GoodStore::load( const VariantMap& stream )
{
   _devastation = stream.get( "devastation" ).toBool();
   _nextReservationID = stream.get( "nextReservationId" ).toInt();

   VariantList vm_storereservations = stream.get( "storeReservations" ).toList();
   for( VariantList::iterator it=vm_storereservations.begin(); it != vm_storereservations.end(); it++ )
   {
     GoodStock stock;
     int index = (*it).toInt(); it++;
     stock.load( (*it).toList() );
     _storeReservations[ index ] = stock;
   }

   VariantList vm_retrievereservations = stream.get( "retrieveReservation" ).toList();
   for( VariantList::iterator it=vm_retrievereservations.begin(); it != vm_retrievereservations.end(); it++ )
   {
     GoodStock stock;
     int index = (*it).toInt(); it++;
     stock.load( (*it).toList() );
     _retrieveReservations[ index ] = stock;
   }
}

bool GoodStore::isDevastation() const
{
  return _devastation;
}

void GoodStore::setDevastation( bool value )
{
  _devastation = value;
}

GoodStore::~GoodStore()
{

}

SimpleGoodStore::SimpleGoodStore()
{
   _maxQty = 0;
   _currentQty = 0;

   _goodStockList.resize(G_MAX);
   for (int n = 0; n < (int) G_MAX; ++n)
   {
      _goodStockList[n] = GoodStock((GoodType)n, 0, 0);
   }
}


void SimpleGoodStore::setMaxQty(const int maxQty)
{
   _maxQty = maxQty;
}


int SimpleGoodStore::getMaxQty()
{
   return _maxQty;
}


int SimpleGoodStore::getCurrentQty() const
{
   return _currentQty;
}


GoodStock& SimpleGoodStore::getStock(const GoodType &goodType)
{
   return _goodStockList[goodType];
}


int SimpleGoodStore::getCurrentQty(const GoodType &goodType) const
{
   return _goodStockList[goodType]._currentQty;
}


int SimpleGoodStore::getMaxQty(const GoodType &goodType)
{
   return _goodStockList[goodType]._maxQty;
}


void SimpleGoodStore::setMaxQty(const GoodType &goodType, const int maxQty)
{
   _goodStockList[goodType]._maxQty = maxQty;
}


void SimpleGoodStore::setCurrentQty(const GoodType &goodType, const int currentQty)
{
   _goodStockList[goodType]._currentQty = currentQty;
   computeCurrentQty();
}


void SimpleGoodStore::computeCurrentQty()
{
   int qty = 0;
   for (std::vector<GoodStock>::iterator goodIt = _goodStockList.begin(); goodIt != _goodStockList.end(); ++goodIt)
   {
      GoodStock &stock = *goodIt;
      qty += stock._currentQty;
   }

   _currentQty = qty;
}


int SimpleGoodStore::getMaxStore(const GoodType goodType)
{
  int freeRoom = 0;
  if( !_devastation )
  {
     // current free capacity
    freeRoom = math::clamp( _goodStockList[goodType]._maxQty - _currentQty, 0, _goodStockList[goodType]._maxQty );

    // remove all storage reservations
    for (std::map<long, GoodStock>::iterator reservationIt = _storeReservations.begin(); reservationIt != _storeReservations.end(); ++reservationIt)
    {
       GoodStock &reservationStock = reservationIt->second;
       freeRoom -= reservationStock._currentQty;
    }
  }

  return freeRoom;
}

void SimpleGoodStore::applyStorageReservation(GoodStock &stock, const long reservationID)
{
   GoodStock reservedStock = getStorageReservation(reservationID, true);

   if (stock._goodType != reservedStock._goodType)
   {
      THROW("GoodType does not match reservation");
   }
   if (stock._currentQty < reservedStock._currentQty)
   {
      THROW("Quantity does not match reservation");
   }


   int amount = reservedStock._currentQty;
   GoodStock &currentStock = _goodStockList[reservedStock._goodType];
   currentStock._currentQty += amount;
   stock._currentQty -= amount;
   _currentQty += amount;
   // std::cout << "SimpleGoodStore, store qty=" << amount << " resID=" << reservationID << std::endl;
}


void SimpleGoodStore::applyRetrieveReservation(GoodStock &stock, const long reservationID)
{
   GoodStock reservedStock = getRetrieveReservation(reservationID, true);

   if (stock._goodType != reservedStock._goodType)
   {
      THROW("GoodType does not match reservation");
   }
   if (stock._maxQty < stock._currentQty + reservedStock._currentQty)
   {
      THROW("Quantity does not match reservation");
   }

   int amount = reservedStock._currentQty;
   GoodStock &currentStock = getStock(reservedStock._goodType);
   currentStock._currentQty -= amount;
   stock._currentQty += amount;
   _currentQty -= amount;
   // std::cout << "SimpleGoodStore, retrieve qty=" << amount << " resID=" << reservationID << std::endl;
}


void SimpleGoodStore::save( VariantMap& stream ) const
{
  GoodStore::save( stream );
  stream[ "max" ] = _maxQty;
  stream[ "current" ] = _currentQty;

  VariantList stockSave;
  for( std::vector<GoodStock>::const_iterator itStock = _goodStockList.begin(); itStock != _goodStockList.end(); itStock++)
  {
    VariantList currentStockSave;
    (*itStock).save( currentStockSave );
    stockSave.push_back( currentStockSave );
  }

  stream[ "stock" ] = stockSave;
}

void SimpleGoodStore::load( const VariantMap& stream )
{
  _goodStockList.clear();

  GoodStore::load( stream );
  _maxQty = stream.get( "max" ).toInt();
  _currentQty = stream.get( "current" ).toInt();

  VariantList stockSave = stream.get( "stock" ).toList();
  for( VariantList::iterator it=stockSave.begin(); it!=stockSave.end(); it++ )
  {
    GoodStock restored;
    restored.load( (*it).toList() );
    _goodStockList.push_back( restored );
  }
}