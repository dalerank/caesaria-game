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

#include "oc3_warehouse.hpp"

#include <iostream>

#include "oc3_picture.hpp"
#include "oc3_exception.hpp"
#include "oc3_gui_info_box.hpp"
#include "oc3_gettext.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_variant.hpp"
#include "oc3_walker_cart_pusher.hpp"
#include "oc3_scenario.hpp"
#include "oc3_goodstore.hpp"
#include "oc3_city.hpp"

#include <list>

class WarehouseTile
{
public:
  WarehouseTile(const TilePos& pos );
  void computePicture();

  TilePos _pos;
  GoodStock _stock;
  Picture _picture;
};

WarehouseTile::WarehouseTile( const TilePos& pos )
{
  _pos = pos;
  _stock._maxQty = 400;
  computePicture();
}

void WarehouseTile::computePicture()
{
  int picIdx = 0;
  switch (_stock._goodType)
  {
  case G_NONE: picIdx = 19; break;
  case G_WHEAT: picIdx = 20; break;
  case G_VEGETABLE: picIdx = 24; break;
  case G_FRUIT: picIdx = 28; break;
  case G_OLIVE: picIdx = 32; break;
  case G_GRAPE: picIdx = 36; break;
  case G_MEAT: picIdx = 40; break;
  case G_WINE: picIdx = 44; break;
  case G_OIL: picIdx = 48; break;
  case G_IRON: picIdx = 52; break;
  case G_TIMBER: picIdx = 56; break;
  case G_CLAY: picIdx = 60; break;
  case G_MARBLE: picIdx = 64; break;
  case G_WEAPON: picIdx = 68; break;
  case G_FURNITURE: picIdx = 72; break;
  case G_POTTERY: picIdx = 76; break;
  case G_FISH: picIdx = 80; break;
  default:
    _OC3_DEBUG_BREAK_IF( _stock._goodType && "Unexpected good type: " );
  }
  if (_stock._goodType != G_NONE)
  {
    picIdx += _stock._currentQty/100 -1;
  }

  _picture = Picture::load( ResourceGroup::warehouse, picIdx );
  _picture.addOffset(30*(_pos.getI()+_pos.getJ()), 15*(_pos.getJ()-_pos.getI()));
}


class WarehouseStore : public GoodStore
{
public:
  using GoodStore::applyStorageReservation;
  using GoodStore::applyRetrieveReservation;

  WarehouseStore();

  void init(Warehouse &_warehouse);

  virtual int getCurrentQty(const GoodType &goodType) const;
  virtual int getCurrentQty() const;
  virtual int getMaxQty() const;

  // returns the max quantity that can be stored now
  virtual int getMaxStore(const GoodType goodType);

  // store/retrieve
  virtual void applyStorageReservation(GoodStock &stock, const long reservationID);
  virtual void applyRetrieveReservation(GoodStock &stock, const long reservationID);

private:
  Warehouse* _warehouse;
};

class Warehouse::Impl
{
public:
  Animation animFlag;  // the flag above the warehouse
  typedef std::vector<WarehouseTile> WhTiles;
  WhTiles subTiles;
  WarehouseStore goodStore;
};

WarehouseStore::WarehouseStore()
{
  _warehouse = NULL;

  for( int goodType=G_WHEAT; goodType <= G_MARBLE; goodType++ )
  {
    setOrder( (GoodType)goodType, GoodOrders::accept );
  }
}


void WarehouseStore::init(Warehouse &warehouse)
{
  _warehouse = &warehouse;
}

int WarehouseStore::getCurrentQty(const GoodType &goodType) const
{
  int amount = 0;

  for( Warehouse::Impl::WhTiles::iterator subTilesIt=_warehouse->_d->subTiles.begin(); 
    subTilesIt!=_warehouse->_d->subTiles.end(); ++subTilesIt)
  {
    if ( (*subTilesIt)._stock._goodType == goodType)
    {
      amount += (*subTilesIt)._stock._currentQty;
    }
  }

  return amount;
}

int WarehouseStore::getCurrentQty() const
{
  if( _warehouse->getWorkers() == 0 )
    return 0;

  int amount = 0;

  for( Warehouse::Impl::WhTiles::iterator subTilesIt=_warehouse->_d->subTiles.begin(); 
    subTilesIt!=_warehouse->_d->subTiles.end(); ++subTilesIt)
  {
    amount += (*subTilesIt)._stock._currentQty;
  }

  return amount;
}

int WarehouseStore::getMaxStore(const GoodType goodType)
{
  if( getOrder( goodType ) == GoodOrders::reject || isDevastation() || _warehouse->getWorkers() == 0 )
  { 
    return 0;
  }

  // compute the quantity of each goodType in the warehouse, taking in account all reservations
  std::map<GoodType, int> stockList;

  // init the map
  for (int i = G_NONE; i != G_MAX; ++i)
  {
    GoodType goodType = (GoodType) i;
    stockList[goodType] = 0;
  }
  // put current stock in the map
  for( Warehouse::Impl::WhTiles::iterator subTilesIt=_warehouse->_d->subTiles.begin(); 
    subTilesIt!=_warehouse->_d->subTiles.end(); ++subTilesIt)
  {
    WarehouseTile &subTile = *subTilesIt;
    GoodStock &subTileStock = subTile._stock;
    stockList[subTileStock._goodType] += subTileStock._currentQty;
  }

  // add reservations
  for( _Reservations::iterator reservationIt = _getStoreReservations().begin(); 
    reservationIt != _getStoreReservations().end(); ++reservationIt)
  {
    GoodStock &reservationStock = reservationIt->second;
    stockList[reservationStock._goodType] += reservationStock._currentQty;
  }

  // compute number of free tiles
  int nbFreeTiles = _warehouse->_d->subTiles.size();
  for (std::map<GoodType, int>::iterator stockListIt=stockList.begin(); stockListIt!=stockList.end(); ++stockListIt)
  {
    GoodType otherGoodType = stockListIt->first;
    if (otherGoodType == goodType)
    {
      // don't count this goodType
      continue;
    }
    int qty = stockListIt->second;
    int nbTiles = ((qty/100)+3)/4;  // nb of subTiles this goodType occupies
    nbFreeTiles -= nbTiles;
  }

  int freeRoom = 400 * nbFreeTiles - stockList[goodType];

  // std::cout << "MaxStore for good is " << freeRoom << " on free tiles:" << nbFreeTiles << std::endl;

  return freeRoom;
}


void WarehouseStore::applyStorageReservation(GoodStock &stock, const long reservationID)
{
  GoodStock reservedStock = getStorageReservation(reservationID, true);

  if (stock._goodType != reservedStock._goodType)
  {
    _OC3_DEBUG_BREAK_IF( "GoodType does not match reservation" );
    return;
  }

  if (stock._currentQty < reservedStock._currentQty)
  {
    _OC3_DEBUG_BREAK_IF( "Quantity does not match reservation" );
    return;
  }


  int amount = reservedStock._currentQty;
  // std::cout << "WarehouseStore, store qty=" << amount << " resID=" << reservationID << std::endl;

  // first we look at the half filled subTiles
  for( Warehouse::Impl::WhTiles::iterator subTilesIt=_warehouse->_d->subTiles.begin(); 
    subTilesIt!=_warehouse->_d->subTiles.end(); ++subTilesIt)
  {
    if (amount == 0)
    {
      break;
    }

    WarehouseTile &subTile = *subTilesIt;
    if (subTile._stock._goodType == stock._goodType && subTile._stock._currentQty < subTile._stock._maxQty)
    {
      int tileAmount = std::min(amount, subTile._stock._maxQty - subTile._stock._currentQty);
      // std::cout << "put in half filled" << std::endl;
      subTile._stock.addStock(stock, tileAmount);
      amount -= tileAmount;
    }
  }

  // then we look at the empty subTiles
  for( Warehouse::Impl::WhTiles::iterator subTilesIt=_warehouse->_d->subTiles.begin(); 
    subTilesIt!=_warehouse->_d->subTiles.end(); ++subTilesIt)
  {
    if (amount == 0)
    {
      break;
    }

    WarehouseTile& subTile = *subTilesIt;
    if (subTile._stock._goodType == G_NONE)
    {
      int tileAmount = std::min(amount, subTile._stock._maxQty);
      // std::cout << "put in empty tile" << std::endl;
      subTile._stock.addStock(stock, tileAmount);
      amount -= tileAmount;
    }
  }

  _warehouse->computePictures();
}


void WarehouseStore::applyRetrieveReservation(GoodStock &stock, const long reservationID)
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
  // std::cout << "WarehouseStore, retrieve qty=" << amount << " resID=" << reservationID << std::endl;

  // first we look at the half filled subTiles
  for( Warehouse::Impl::WhTiles::iterator subTilesIt=_warehouse->_d->subTiles.begin(); 
    subTilesIt!=_warehouse->_d->subTiles.end(); ++subTilesIt)
  {
    if (amount == 0)
    {
      break;
    }

    WarehouseTile &subTile = *subTilesIt;
    if( subTile._stock._goodType == stock._goodType && subTile._stock._currentQty < subTile._stock._maxQty)
    {
      int tileAmount = std::min(amount, subTile._stock._currentQty);
      // std::cout << "retrieve from half filled" << std::endl;
      stock.addStock(subTile._stock, tileAmount);
      amount -= tileAmount;
    }
  }

  // then we look at the filled subTiles
  for( Warehouse::Impl::WhTiles::iterator subTilesIt=_warehouse->_d->subTiles.begin(); 
    subTilesIt!=_warehouse->_d->subTiles.end(); ++subTilesIt)
  {
    if (amount == 0)
    {
      break;
    }

    WarehouseTile& subTile = *subTilesIt;
    if (subTile._stock._goodType == stock._goodType)
    {
      int tileAmount = std::min(amount, subTile._stock._currentQty);
      // std::cout << "retrieve from filled" << std::endl;
      stock.addStock(subTile._stock, tileAmount);
      amount -= tileAmount;
    }
  }

  _warehouse->computePictures();
}

int WarehouseStore::getMaxQty() const
{
  return 400 * _warehouse->_d->subTiles.size();
}


Warehouse::Warehouse() : WorkingBuilding( B_WAREHOUSE, Size( 3 )), _d( new Impl )
{
   // _name = _("Entrepot");
  setPicture( Picture::load( ResourceGroup::warehouse, 19) );
  _fgPictures.resize(12);  // 8 tiles + 4

  _getAnimation().load( ResourceGroup::warehouse, 2, 16 );
  _getAnimation().setFrameDelay( 4 );

  _d->animFlag.load( ResourceGroup::warehouse, 84, 8 );

  init();

  setWorkers( 0 );
}

void Warehouse::init()
{
  _fgPictures[0] = &Picture::load(ResourceGroup::warehouse, 1);
  _fgPictures[1] = &Picture::load(ResourceGroup::warehouse, 18);
  _fgPictures[2] = _getAnimation().getCurrentPicture();
  _fgPictures[3] = _d->animFlag.getCurrentPicture();

  // add subTiles in Z-order (from far to near)
  _d->subTiles.clear();
  _d->subTiles.push_back( WarehouseTile( TilePos( 1, 2 ) ));
  _d->subTiles.push_back( WarehouseTile( TilePos( 0, 1 ) ));
  _d->subTiles.push_back( WarehouseTile( TilePos( 2, 2 ) ));
  _d->subTiles.push_back( WarehouseTile( TilePos( 1, 1 ) ));
  _d->subTiles.push_back( WarehouseTile( TilePos( 0, 0 ) ));
  _d->subTiles.push_back( WarehouseTile( TilePos( 2, 1 ) ));
  _d->subTiles.push_back( WarehouseTile( TilePos( 1, 0 ) ));
  _d->subTiles.push_back( WarehouseTile( TilePos( 2, 0 ) ));

  for (unsigned int n = 0; n<_d->subTiles.size(); ++n)
  {
     _fgPictures[n+4] = &_d->subTiles[n]._picture;
  }

  _d->goodStore.init(*this);

  computePictures();
}

void Warehouse::timeStep(const unsigned long time)
{
  if( getWorkers() > 0 )
  {
   _getAnimation().update( time );
   _d->animFlag.update( time );

   _fgPictures[2] = _getAnimation().getCurrentPicture();
   _fgPictures[3] = _d->animFlag.getCurrentPicture();
  }

  if( _d->goodStore.isDevastation() && (time % 22 == 1 ) )
  {
    _resolveDevastationMode();
  }
}

void Warehouse::computePictures()
{
  for (Impl::WhTiles::iterator subTilesIt=_d->subTiles.begin(); subTilesIt!=_d->subTiles.end(); ++subTilesIt)
  {
     WarehouseTile &subTile = *subTilesIt;
     subTile.computePicture();
  }
}

GoodStore& Warehouse::getGoodStore()
{
   return _d->goodStore;
}

void Warehouse::save( VariantMap& stream ) const
{
  WorkingBuilding::save( stream );

  stream[ "__debug_typeName" ] = Variant( std::string( OC3_STR_EXT(B_WAREHOUSE) ) );
  stream[ "goodStore" ] = _d->goodStore.save();

  VariantList vm_tiles;
  for(Impl::WhTiles::iterator subTilesIt=_d->subTiles.begin(); subTilesIt!=_d->subTiles.end(); ++subTilesIt)
  {
    vm_tiles.push_back( subTilesIt->_stock.save() );
  }
  stream[ "tiles" ] = vm_tiles;
}

void Warehouse::load( const VariantMap& stream )
{
  WorkingBuilding::load( stream );

  _d->goodStore.load( stream.get( "goodStore" ).toMap() );
  
  VariantList vm_tiles = stream.get( "tiles" ).toList();
  int tileIndex = 0;
  for( VariantList::iterator it=vm_tiles.begin(); it != vm_tiles.end(); it++, tileIndex++ )
  {
    _d->subTiles[ tileIndex ]._stock.load( it->toList() );
  }

  computePictures();
}

void Warehouse::_resolveDevastationMode()
{
  //if warehouse in devastation mode need try send cart pusher with goods to other granary/warehouse/factory
  if( (_d->goodStore.getCurrentQty() > 0) && getWalkerList().empty() )
  {
    for( int goodType=G_WHEAT; goodType <= G_VEGETABLE; goodType++ )
    {
      //int goodQtyMax = _d->goodStore.getCurrentQty( (GoodType)goodType );
      int goodQty = math::clamp( goodQty, 0, 400);

      if( goodQty > 0 )
      {
        GoodStock stock( (GoodType)goodType, goodQty, goodQty);
        CartPusherPtr walker = CartPusher::create( Scenario::instance().getCity() );
        walker->send2City( BuildingPtr( this ), stock );

        if( !walker->isDeleted() )
        {
          GoodStock tmpStock( (GoodType)goodType, goodQty );;
          _d->goodStore.retrieve( tmpStock, goodQty );
          addWalker( walker.as<Walker>() );
        }
      }
    }   
  }
}
