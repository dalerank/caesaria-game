// This file is part of CaesarIA.
//
// CaesarIA is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CaesarIA is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CaesarIA.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

#include "warehouse.hpp"

#include <iostream>

#include "gfx/picture.hpp"
#include "core/exception.hpp"
#include "gui/info_box.hpp"
#include "core/gettext.hpp"
#include "game/resourcegroup.hpp"
#include "core/variant.hpp"
#include "walker/cart_pusher.hpp"
#include "good/goodstore.hpp"
#include "city/city.hpp"
#include "core/foreach.hpp"
#include "core/stringhelper.hpp"
#include "core/logger.hpp"
#include "constants.hpp"
#include "good/goodhelper.hpp"
#include "game/gamedate.hpp"
#include "walker/cart_supplier.hpp"

#include <list>

class WarehouseTile : public ReferenceCounted
{
public:
  WarehouseTile( const TilePos& pos )
  {
    _pos = pos;
    _stock.setCapacity( 400 );
    computePicture();
  }

  void computePicture();

  TilePos _pos;
  GoodStock _stock;
  Picture _picture;
};


void WarehouseTile::computePicture()
{
  int picIdx = 0;
  switch (_stock.type() )
  {
  case Good::none: picIdx = 19; break;
  case Good::wheat: picIdx = 20; break;
  case Good::vegetable: picIdx = 24; break;
  case Good::fruit: picIdx = 28; break;
  case Good::olive: picIdx = 32; break;
  case Good::grape: picIdx = 36; break;
  case Good::meat: picIdx = 40; break;
  case Good::wine: picIdx = 44; break;
  case Good::oil: picIdx = 48; break;
  case Good::iron: picIdx = 52; break;
  case Good::timber: picIdx = 56; break;
  case Good::clay: picIdx = 60; break;
  case Good::marble: picIdx = 64; break;
  case Good::weapon: picIdx = 68; break;
  case Good::furniture: picIdx = 72; break;
  case Good::pottery: picIdx = 76; break;
  case Good::fish: picIdx = 80; break;
  default:
      _CAESARIA_DEBUG_BREAK_IF( "Unexpected good type: " );
  }

  if (_stock.type() != Good::none)
  {
    picIdx += _stock.qty()/100 -1;
  }

  _picture = Picture::load( ResourceGroup::warehouse, picIdx );
  _picture.addOffset(30*(_pos.i()+_pos.j()), 15*(_pos.j()-_pos.i()));
}


class WarehouseStore : public GoodStore
{
public:
  typedef std::map< Good::Type, int > StockMap;

  //using GoodStore::applyStorageReservation;
  //using GoodStore::applyRetrieveReservation;

  WarehouseStore();

  void init(Warehouse &_warehouse);

  virtual int qty(const Good::Type &goodType) const;
  virtual int qty() const;
  virtual int capacity() const;
  virtual void setCapacity( const int maxcap);
  virtual int capacity(const Good::Type& goodType ) const;

  // returns the max quantity that can be stored now
  virtual int getMaxStore(const Good::Type goodType);

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
  int devastateModeInterval;
};

WarehouseStore::WarehouseStore()
{
  _warehouse = NULL;

  for( int goodType=Good::wheat; goodType <= Good::goodCount; goodType++ )
  {
    setOrder( (Good::Type)goodType, GoodOrders::accept );
  }
}

void WarehouseStore::init(Warehouse &warehouse){  _warehouse = &warehouse; }

int WarehouseStore::qty(const Good::Type &goodType) const
{
  if( _warehouse->numberWorkers() == 0 )
    return 0;

  int amount = 0;

  foreach( whTile, _warehouse->_d->subTiles )
  {
    if ( whTile->_stock.type() == goodType || goodType == Good::goodCount )
    {
      amount += whTile->_stock.qty();
    }
  }

  return amount;
}

int WarehouseStore::qty() const {  return qty( Good::goodCount ); }

int WarehouseStore::getMaxStore(const Good::Type goodType)
{
  if( getOrder( goodType ) == GoodOrders::reject || isDevastation() || _warehouse->onlyDispatchGoods() )
  { 
    return 0;
  }

  // compute the quantity of each goodType in the warehouse, taking in account all reservations
  StockMap maxStore;

  // init the map
  for (int i = Good::none; i != Good::goodCount; ++i)
  {
    maxStore[ (Good::Type)i ] = 0;
  }
  // put current stock in the map
  foreach( whTile, _warehouse->_d->subTiles )
  {
    GoodStock &subTileStock = whTile->_stock;
    maxStore[ subTileStock.type() ] += subTileStock.qty();
  }

  // add reservations
  Reservations& r = _getStoreReservations();
  for( Reservations::iterator i=r.begin(); i != r.end(); i++ )
  {
    const GoodStock& reservationStock = i->stock;
    maxStore[ reservationStock.type() ] += reservationStock.qty();
  }

  // compute number of free tiles
  int nbFreeTiles = _warehouse->_d->subTiles.size();
  foreach( mapItem, maxStore )
  {
    Good::Type otherGoodType = mapItem->first;
    if (otherGoodType == goodType)
    {
      // don't count this goodType
      continue;
    }
    int qty = mapItem->second;
    int nbTiles = ((qty/100)+3)/4;  // nb of subTiles this goodType occupies
    nbFreeTiles -= nbTiles;
  }

  int freeRoom = 400 * nbFreeTiles - maxStore[goodType];

  // std::cout << "MaxStore for good is " << freeRoom << " on free tiles:" << nbFreeTiles << std::endl;

  return freeRoom;
}

void WarehouseStore::applyStorageReservation( GoodStock &stock, const long reservationID )
{
  GoodStock reservedStock = getStorageReservation(reservationID, true);

  if (stock.type() != reservedStock.type())
  {
    Logger::warning( "Warehouse: GoodType does not match reservation" );
    return;
  }

  if (stock.qty() < reservedStock.qty())
  {
    Logger::warning( "Warehouse: Quantity does not match reservation" );
    return;
  }


  int amount = reservedStock.qty();
  // std::cout << "WarehouseStore, store qty=" << amount << " resID=" << reservationID << std::endl;

  // first we look at the half filled subTiles
  foreach( whTile, _warehouse->_d->subTiles )
  {
    if (amount == 0)
    {
      break;
    }

    GoodStock& whStock = whTile->_stock;
    if( whStock.type() == stock.type() && whStock.freeQty() > 0 )
    {
      int tileAmount = std::min(amount, whStock.capacity() - whStock.qty());
      // std::cout << "put in half filled" << std::endl;
      whStock.append(stock, tileAmount);
      amount -= tileAmount;
    }
  }

  // then we look at the empty subTiles
  foreach( whTile, _warehouse->_d->subTiles )
  {
    if (amount == 0)
    {
      break;
    }

    GoodStock& whStock = whTile->_stock;
    if( whStock.type() == Good::none)
    {
      int tileAmount = std::min(amount, whStock.capacity() );
      // std::cout << "put in empty tile" << std::endl;
      whStock.append(stock, tileAmount);
      amount -= tileAmount;
    }
  }

  _warehouse->computePictures();
}

void WarehouseStore::applyRetrieveReservation(GoodStock &stock, const long reservationID)
{
  GoodStock reservedStock = getRetrieveReservation(reservationID, true);

  if( stock.type() != reservedStock.type() )
  {   
    Logger::warning( "Warehouse: GoodType does not match reservation need=%s have=%s",
                     GoodHelper::getName(reservedStock.type()).c_str(),
                     GoodHelper::getName(stock.type()).c_str() );
    return;
  }
  if( stock.capacity() < stock.qty() + reservedStock.qty() )
  {
    Logger::warning( "Warehouse: Retrieve stock[%s] less reserve qty, decrease from %d to &%d",
                     GoodHelper::getName(stock.type()).c_str(),
                     reservedStock.qty(), stock.freeQty() );
    reservedStock.setQty( stock.freeQty() );
  }

  int amount = reservedStock.qty();

  // first we look at the half filled subTiles
  foreach( whTile, _warehouse->_d->subTiles )
  {
    if (amount == 0)
    {
      break;
    }

    GoodStock& whStock = whTile->_stock;
    if( whStock.type() == stock.type() && whStock.freeQty() > 0 )
    {
      int tileAmount = std::min(amount, whStock.qty());
      // std::cout << "retrieve from half filled" << std::endl;
      stock.append( whStock, tileAmount);
      amount -= tileAmount;
    }
  }

  // then we look at the filled subTiles
  foreach( whTile, _warehouse->_d->subTiles )
  {
    if (amount == 0)
    {
      break;
    }

    GoodStock& whStock = whTile->_stock;
    if( whStock.type() == stock.type())
    {
      int tileAmount = std::min(amount, whStock.qty());
      // std::cout << "retrieve from filled" << std::endl;
      stock.append(whStock, tileAmount);
      amount -= tileAmount;
    }
  }

  _warehouse->computePictures();
}

int WarehouseStore::capacity() const{  return 400 * _warehouse->_d->subTiles.size();}
void WarehouseStore::setCapacity(const int){}
int WarehouseStore::capacity( const Good::Type& goodType ) const{  return capacity();}

Warehouse::Warehouse() : WorkingBuilding( constants::building::warehouse, Size( 3 )), _d( new Impl )
{
   // _name = _("Entrepot");
  setPicture( ResourceGroup::warehouse, 19 );
  _fgPicturesRef().resize(12);  // 8 tiles + 4

  _animationRef().load( ResourceGroup::warehouse, 2, 16 );
  _animationRef().setDelay( 4 );

  _d->animFlag.load( ResourceGroup::warehouse, 84, 8 );
  _d->devastateModeInterval = GameDate::ticksInMonth() / 5;

  _fgPicturesRef()[ 0 ] = Picture::load(ResourceGroup::warehouse, 1);
  _fgPicturesRef()[ 1 ] = Picture::load(ResourceGroup::warehouse, 18);
  _fgPicturesRef()[ 2 ] = _animationRef().getFrame();
  _fgPicturesRef()[ 3 ] = _d->animFlag.getFrame();

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

  _d->goodStore.init(*this);

  computePictures();
}

void Warehouse::timeStep(const unsigned long time)
{
  if( numberWorkers() > 0 )
  {
   _animationRef().update( time );
   _d->animFlag.update( time );

   _fgPicturesRef()[2] = _animationRef().getFrame();
   _fgPicturesRef()[3] = _d->animFlag.getFrame();
  }

  if( (time % _d->devastateModeInterval == 1 ) )
  {
    if( _d->goodStore.isDevastation() )
    {
      _resolveDevastationMode();
    }
    else
    {
      _resolveDeliverMode();
    }
  }

  WorkingBuilding::timeStep( time );
}

void Warehouse::computePictures()
{
  int index = 4;
  foreach( whTile, _d->subTiles )
  {
     whTile->computePicture();
     _fgPicturesRef()[index] = whTile->_picture;
     index++;
  }
}

GoodStore& Warehouse::store() {   return _d->goodStore; }
const GoodStore& Warehouse::store() const {   return _d->goodStore; }

void Warehouse::save( VariantMap& stream ) const
{
  WorkingBuilding::save( stream );

  stream[ "__debug_typeName" ] = Variant( std::string( CAESARIA_STR_EXT(Warehouse) ) );
  stream[ "goodStore" ] = _d->goodStore.save();

  VariantList vm_tiles;
  foreach( whTile, _d->subTiles ) { vm_tiles.push_back( whTile->_stock.save() ); }

  stream[ "tiles" ] = vm_tiles;
}

void Warehouse::load( const VariantMap& stream )
{
  WorkingBuilding::load( stream );

  _d->goodStore.load( stream.get( "goodStore" ).toMap() );
  
  VariantList vm_tiles = stream.get( "tiles" ).toList();
  int tileIndex = 0;
  foreach( it, vm_tiles )
  {
    _d->subTiles[ tileIndex ]._stock.load( it->toList() );
    tileIndex++;
  }

  computePictures();
}

bool Warehouse::onlyDispatchGoods() const {  return numberWorkers() < maxWorkers() / 3; }

bool Warehouse::isGettingFull() const
{
  foreach( whTile, _d->subTiles )
  {
    if( whTile->_stock.qty() == 0 )
      return false;
  }

  return true;
}

std::string Warehouse::troubleDesc() const
{
  std::string ret = WorkingBuilding::troubleDesc();

  if( ret.empty() )
  {
    if( onlyDispatchGoods() )  { ret = "##warehouse_low_personal_warning##";  }
    else if( store().freeQty() == 0 ) { ret = "##warehouse_full_warning##";  }
    else if( isGettingFull() ) { ret == "##warehouse_gettinfull_warning##"; }
  }

  return ret;
}

void Warehouse::_resolveDeliverMode()
{
  if( getWalkers().size() > 0 )
  {
    return;
  }
  //if warehouse in devastation mode need try send cart pusher with goods to other granary/warehouse/factory
  for( int goodType=Good::wheat; goodType <= Good::goodCount; goodType++ )
  {
    Good::Type gType = (Good::Type)goodType;
    GoodOrders::Order order = _d->goodStore.getOrder( gType );
    int goodFreeQty = math::clamp( _d->goodStore.freeQty( gType ), 0, 400 );

    if( GoodOrders::deliver == order && goodFreeQty > 0 )
    {
      CartSupplierPtr walker = CartSupplier::create( _getCity() );
      walker->send2city( BuildingPtr( this ), gType, goodFreeQty );

      if( !walker->isDeleted() )
      {
        addWalker( walker.object() );
        return;
      }
    }
  }
}

void Warehouse::_resolveDevastationMode()
{
  //if warehouse in devastation mode need try send cart pusher with goods to other granary/warehouse/factory
  if( (_d->goodStore.qty() > 0) && getWalkers().empty() )
  {
    for( int goodType=Good::wheat; goodType <= Good::goodCount; goodType++ )
    {
      int goodQty = _d->goodStore.qty( (Good::Type)goodType );
      goodQty = math::clamp( goodQty, 0, 400);

      if( goodQty > 0 )
      {
        GoodStock stock( (Good::Type)goodType, goodQty, goodQty);
        CartPusherPtr walker = CartPusher::create( _getCity() );
        walker->send2city( BuildingPtr( this ), stock );

        if( !walker->isDeleted() )
        {
          GoodStock tmpStock( (Good::Type)goodType, goodQty );;
          _d->goodStore.retrieve( tmpStock, goodQty );
          addWalker( walker.object() );
        }
      }
    }   
  }
}
