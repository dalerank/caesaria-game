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
#include "gfx/helper.hpp"
#include "game/resourcegroup.hpp"
#include "core/variant.hpp"
#include "walker/cart_pusher.hpp"
#include "good/goodstore.hpp"
#include "city/city.hpp"
#include "core/foreach.hpp"
#include "core/utils.hpp"
#include "core/logger.hpp"
#include "constants.hpp"
#include "good/goodhelper.hpp"
#include "game/gamedate.hpp"
#include "walker/cart_supplier.hpp"
#include "extension.hpp"
#include "warehouse_store.hpp"

#include <list>

using namespace gfx;

namespace {
CAESARIA_LITERALCONST(tiles)
CAESARIA_LITERALCONST(goodStore)
}

Warehouse::Room::Room(const TilePos &pos)
{
  location = pos;
  setCapacity( basicCapacity );
  computePicture();
}

void Warehouse::Room::computePicture()
{
  int picIdx = 0;
  good::Type gtype = empty() ? good::none : type();

  switch( gtype )
  {
  case good::none: picIdx = 19; break;
  case good::wheat: picIdx = 20; break;
  case good::vegetable: picIdx = 24; break;
  case good::fruit: picIdx = 28; break;
  case good::olive: picIdx = 32; break;
  case good::grape: picIdx = 36; break;
  case good::meat: picIdx = 40; break;
  case good::wine: picIdx = 44; break;
  case good::oil: picIdx = 48; break;
  case good::iron: picIdx = 52; break;
  case good::timber: picIdx = 56; break;
  case good::clay: picIdx = 60; break;
  case good::marble: picIdx = 64; break;
  case good::weapon: picIdx = 68; break;
  case good::furniture: picIdx = 72; break;
  case good::pottery: picIdx = 76; break;
  case good::fish: picIdx = 80; break;
  case good::prettyWine: picIdx = 44; break;
  case good::goodCount:
    Logger::warning( "Unexpected good type: in warehouse");
    setType( good::none );
    picIdx = 19;
  break;
  default:
    _CAESARIA_DEBUG_BREAK_IF( "Unexpected good type: " );
  }  

  if( gtype != good::none)
  {

    // (0  , 100] -> 0
    // (100, 200] -> 1
    // (200, 300] -> 2
    // (300, 400] -> 3
    picIdx += math::min<int>( 0, good::Helper::convQty2Units( qty() ) - 1 );
  }

  picture = Picture::load( ResourceGroup::warehouse, picIdx );
  picture.addOffset( tile::tilepos2screen( location ) );
}

class Warehouse::Impl
{
public:
  Animation animFlag;  // the flag above the warehouse

  Warehouse::Rooms rooms;
  WarehouseStore goodStore;
};

Warehouse::Warehouse() : WorkingBuilding( constants::objects::warehouse, Size( 3 )), _d( new Impl )
{
   // _name = _("Entrepot");
  setPicture( ResourceGroup::warehouse, 19 );
  _fgPicturesRef().resize(12+1);  // 8 tiles + 4 + 1 animation slot

  _animationRef().load( ResourceGroup::warehouse, 2, 16 );
  _animationRef().setDelay( 4 );

  _d->animFlag.load( ResourceGroup::warehouse, 84, 8 );

  _setClearAnimationOnStop( false );

  _fgPicturesRef()[ 0 ] = Picture::load(ResourceGroup::warehouse, 1);
  _fgPicturesRef()[ 1 ] = Picture::load(ResourceGroup::warehouse, 18);
  _fgPicturesRef()[ 2 ] = _animationRef().currentFrame();
  _fgPicturesRef()[ 3 ] = _d->animFlag.currentFrame();

  // add subTiles in Z-order (from far to near)
  _d->rooms.clear();
  _d->rooms.push_back( Room( TilePos( 1, 2 ) ));
  _d->rooms.push_back( Room( TilePos( 0, 1 ) ));
  _d->rooms.push_back( Room( TilePos( 2, 2 ) ));
  _d->rooms.push_back( Room( TilePos( 1, 1 ) ));
  _d->rooms.push_back( Room( TilePos( 0, 0 ) ));
  _d->rooms.push_back( Room( TilePos( 2, 1 ) ));
  _d->rooms.push_back( Room( TilePos( 1, 0 ) ));
  _d->rooms.push_back( Room( TilePos( 2, 0 ) ));

  _d->goodStore.init(*this);

  computePictures();
}

void Warehouse::timeStep(const unsigned long time)
{
  if( numberWorkers() > 0 )
  {
   _d->animFlag.update( time );

   _fgPicturesRef()[3] = _d->animFlag.currentFrame();
  }

  if( game::Date::isWeekChanged() )
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
  foreach( room, _d->rooms )
  {
     room->computePicture();
     _fgPicture( index ) = room->picture;
     index++;
  }
}

good::Store& Warehouse::store() {   return _d->goodStore; }
const good::Store& Warehouse::store() const {   return _d->goodStore; }

void Warehouse::save( VariantMap& stream ) const
{
  WorkingBuilding::save( stream );

  stream[ "__debug_typeName" ] = Variant( std::string( CAESARIA_STR_EXT(Warehouse) ) );
  stream[ lc_goodStore ] = _d->goodStore.save();

  VariantList vm_tiles;
  foreach( room, _d->rooms ) { vm_tiles.push_back( room->save() ); }

  stream[ lc_tiles ] = vm_tiles;
}

void Warehouse::load( const VariantMap& stream )
{
  WorkingBuilding::load( stream );

  _d->goodStore.load( stream.get( lc_goodStore ).toMap() );
  
  VariantList vm_tiles = stream.get( lc_tiles ).toList();
  int tileIndex = 0;
  foreach( it, vm_tiles )
  {
    _d->rooms[ tileIndex ].load( it->toList() );
    tileIndex++;
  }

  computePictures();
}

bool Warehouse::onlyDispatchGoods() const {  return numberWorkers() < maximumWorkers() / 3; }

bool Warehouse::isGettingFull() const
{
  foreach( room, _d->rooms )
  {
    if( room->qty() == 0 )
      return false;
  }

  return true;
}

float Warehouse::tradeBuff(Warehouse::Buff type) const
{
  SmartList<WarehouseBuff> buffs;
  buffs << extensions();

  float res = 0;
  foreach( it, buffs )
  {
    if( (*it)->group() == type )
      res += (*it)->value();
  }

  return res;
}

Warehouse::Rooms &Warehouse::rooms() { return _d->rooms; }

std::string Warehouse::troubleDesc() const
{
  std::string ret = WorkingBuilding::troubleDesc();

  if( ret.empty() )
  {
    if( onlyDispatchGoods() )  { ret = "##warehouse_low_personal_warning##";  }
    else if( store().freeQty() == 0 ) { ret = "##warehouse_full_warning##";  }
    else if( isGettingFull() ) { ret == "##warehouse_gettinfull_warning##"; }
  }

  if( ret.empty() && _d->goodStore.isDevastation() )
  {
    ret = _("##warehouse_devastation_mode_text##");
  }

  return ret;
}

void Warehouse::_resolveDeliverMode()
{
  if( walkers().size() > 0 )
  {
    return;
  }
  //if warehouse in devastation mode need try send cart pusher with goods to other granary/warehouse/factory
  for( int goodType=good::wheat; goodType <= good::goodCount; goodType++ )
  {
    good::Type gType = (good::Type)goodType;
    good::Orders::Order order = _d->goodStore.getOrder( gType );
    int goodFreeQty = math::clamp<int>( _d->goodStore.freeQty( gType ), 0, Room::basicCapacity );

    if( good::Orders::deliver == order && goodFreeQty > 0 )
    {
      CartSupplierPtr walker = CartSupplier::create( _city() );
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
  if( (_d->goodStore.qty() > 0) && walkers().empty() )
  {
    const int maxCapacity = CartPusher::megaCart;
    for( int goodType=good::wheat; goodType <= good::goodCount; goodType++ )
    {
      int goodQty = _d->goodStore.qty( (good::Type)goodType );
      goodQty = math::clamp( goodQty, 0, maxCapacity);

      if( goodQty > 0 )
      {
        good::Stock stock( (good::Type)goodType, goodQty, goodQty);
        CartPusherPtr cart = CartPusher::create( _city() );
        cart->stock().setCapacity( maxCapacity );
        cart->send2city( BuildingPtr( this ), stock );

        if( !cart->isDeleted() )
        {
          good::Stock tmpStock( (good::Type)goodType, goodQty );;
          _d->goodStore.retrieve( tmpStock, goodQty );
          addWalker( cart.object() );
        }
      }
    }   
  }
}
