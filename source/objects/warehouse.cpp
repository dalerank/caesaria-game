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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

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
#include "objects_factory.hpp"
#include "core/metric.hpp"

#include <list>

using namespace gfx;
using namespace constants;
using namespace metric;

REGISTER_CLASS_IN_OVERLAYFACTORY(objects::warehouse, Warehouse)

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
  good::Product gtype = empty() ? good::none : type();

  if( gtype == good::none ) picIdx = 19;
  else if( gtype == good::wheat ) picIdx = 20;
  else if( gtype == good::vegetable ) picIdx = 24;
  else if( gtype == good::fruit ) picIdx = 28;
  else if( gtype == good::olive ) picIdx = 32;
  else if( gtype == good::grape ) picIdx = 36;
  else if( gtype == good::meat ) picIdx = 40;
  else if( gtype == good::wine ) picIdx = 44;
  else if( gtype == good::oil ) picIdx = 48;
  else if( gtype == good::iron ) picIdx = 52;
  else if( gtype == good::timber ) picIdx = 56;
  else if( gtype == good::clay ) picIdx = 60;
  else if( gtype == good::marble ) picIdx = 64;
  else if( gtype == good::weapon ) picIdx = 68;
  else if( gtype == good::furniture ) picIdx = 72;
  else if( gtype == good::pottery ) picIdx = 76;
  else if( gtype == good::fish ) picIdx = 80;
  else if( gtype == good::prettyWine ) picIdx = 44;
  else
  {
    Logger::warning( "Unexpected good type: in warehouse");
    setType( good::none );
    picIdx = 19;
  }

  if( gtype != good::none)
  {

    // (0  , 100] -> 0
    // (100, 200] -> 1
    // (200, 300] -> 2
    // (300, 400] -> 3
    Unit units = Unit::fromQty( qty() );
    picIdx += math::max<int>( 0, units.ivalue() - 1 );
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

Warehouse::Rooms& Warehouse::rooms() { return _d->rooms; }

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
  for( good::Product gType=good::wheat; gType <= good::goodCount; ++gType )
  {
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
    for( good::Product goodType=good::wheat; goodType <= good::goodCount; ++goodType )
    {
      int goodQty = _d->goodStore.qty( goodType );
      goodQty = math::clamp( goodQty, 0, maxCapacity);

      if( goodQty > 0 )
      {
        good::Stock stock( goodType, goodQty, goodQty);
        CartPusherPtr cart = CartPusher::create( _city() );
        cart->stock().setCapacity( maxCapacity );
        cart->send2city( BuildingPtr( this ), stock );

        if( !cart->isDeleted() )
        {
          good::Stock tmpStock( goodType, goodQty );;
          _d->goodStore.retrieve( tmpStock, goodQty );
          addWalker( cart.object() );
        }
      }
    }   
  }
}
