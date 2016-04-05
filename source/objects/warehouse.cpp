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
#include "game/resourcegroup.hpp"
#include "core/variant.hpp"
#include "walker/cart_pusher.hpp"
#include "good/store.hpp"
#include "city/city.hpp"
#include "core/foreach.hpp"
#include "core/utils.hpp"
#include "core/logger.hpp"
#include "constants.hpp"
#include "good/helper.hpp"
#include "game/gamedate.hpp"
#include "core/variant_list.hpp"
#include "walker/cart_supplier.hpp"
#include "extension.hpp"
#include "warehouse_store.hpp"
#include "objects_factory.hpp"
#include "core/metric.hpp"
#include "config.hpp"

#include <list>

using namespace gfx;
using namespace metric;
using namespace config;

REGISTER_CLASS_IN_OVERLAYFACTORY(object::warehouse, Warehouse)

namespace
{
GAME_LITERALCONST(tiles)
GAME_LITERALCONST(goodStore)
}

namespace config
{

namespace fgpic
{
  enum { idxWhRoof=1, idxAnimWork, idxAnimFlag };
}

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
  if( empty() && type() != good::none  )
    setType( good::none );

  good::Product gtype = type();

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

  picture.load( ResourceGroup::warehouse, picIdx );
  picture.addOffset( location.toScreenCoordinates() );
}

class Warehouse::Impl
{
public:
  Animation animFlag;  // the flag above the warehouse
  bool isTradeCenter;

  Warehouse::Rooms rooms;
  WarehouseStore goodStore;
};

static Picture strafePic(const std::string& rc, int index, int i, int j )
{
  Picture ret( rc, index );
  ret.addOffset( TilePos( i, j ).toScreenCoordinates() );

  return ret;
}

Warehouse::Warehouse() : WorkingBuilding( object::warehouse, Size::square( 3 )), _d( new Impl )
{
  setPicture( strafePic( ResourceGroup::warehouse, 19, 0, 2 ) );

  _fgPictures().resize(12+1);  // 8 tiles + 4 + 1 animation slot

  _animation().load( ResourceGroup::warehouse, 2, 16 );
  _animation().setDelay( 4 );

  _d->animFlag.load( ResourceGroup::warehouse, 84, 8 );

  _setClearAnimationOnStop( false );

  _fgPictures()[ fgpic::idxMainPic ] = strafePic( ResourceGroup::warehouse, 1,  0, 2);
  _fgPictures()[ fgpic::idxWhRoof  ] = strafePic( ResourceGroup::warehouse, 18, 0, 2);
  _fgPictures()[ fgpic::idxAnimWork] = _animation().currentFrame();
  _fgPictures()[ fgpic::idxAnimFlag] = _d->animFlag.currentFrame();

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
  _d->isTradeCenter = false;

  computePictures();
}

void Warehouse::timeStep(const unsigned long time)
{
  if( numberWorkers() > 0 )
  {
    _d->animFlag.update( time );

    _fgPicture(3) = _d->animFlag.currentFrame();
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

    _animation().setDelay( 4 + needWorkers() + math::random(2) );
    _d->goodStore.removeExpired( game::Date::current() );
  }

  WorkingBuilding::timeStep( time );
}

void Warehouse::computePictures()
{
  int index = 4;
  std::string rc = _d->isTradeCenter ? ResourceGroup::tradecenter : ResourceGroup::warehouse;
  _fgPictures()[ fgpic::idxMainPic ] = strafePic( rc, 1, 0, 2);
  for( auto& room : _d->rooms )
  {
     room.computePicture();
     _fgPicture( index ) = room.picture;
     index++;
  }
}

good::Store& Warehouse::store() {   return _d->goodStore; }
const good::Store& Warehouse::store() const {   return _d->goodStore; }

void Warehouse::save( VariantMap& stream ) const
{
  WorkingBuilding::save( stream );

  stream[ "__debug_typeName" ] = Variant( std::string( TEXT(Warehouse) ) );
  stream[ literals::goodStore ] = _d->goodStore.save();

  VARIANT_SAVE_ANY_D( stream, _d, isTradeCenter)

  VariantList vm_tiles;
  for( auto& room : _d->rooms )
    vm_tiles.push_back( room.save() );

  stream[ literals::tiles ] = vm_tiles;
}

void Warehouse::load( const VariantMap& stream )
{
  WorkingBuilding::load( stream );

  _d->goodStore.load( stream.get( literals::goodStore ).toMap() );

  VariantList vm_tiles = stream.get( literals::tiles ).toList();
  int tileIndex = 0;
  for( const auto& it : vm_tiles )
  {
    _d->rooms[ tileIndex ].load( it.toList() );
    tileIndex++;
  }

  VARIANT_LOAD_ANY_D( _d, isTradeCenter, stream )

  computePictures();
}

bool Warehouse::onlyDispatchGoods() const {  return numberWorkers() < maximumWorkers() / 3; }
bool Warehouse::isTradeCenter() const { return _d->isTradeCenter; }
Warehouse::Rooms& Warehouse::rooms() { return _d->rooms; }

void Warehouse::setTradeCenter(bool enabled)
{
  _d->isTradeCenter = enabled;
  computePictures();
}

Variant Warehouse::getProperty(const std::string& name) const
{
  if (name == "isTradeCenter") return isTradeCenter();

  return WorkingBuilding::getProperty(name);
}

bool Warehouse::isGettingFull() const
{
  for( auto& room : _d->rooms )
  {
    if( room.qty() == 0 )
      return false;
  }

  return true;
}

float Warehouse::tradeBuff(Warehouse::Buff type) const
{
  auto buffs = extensions().select<WarehouseBuff>();

  float res = 0;
  for( auto& buff : buffs )
  {
    if( buff->group() == type )
      res += buff->value();
  }

  return res;
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
  for( auto& gType : good::all() )
  {
    good::Orders::Order order = _d->goodStore.getOrder( gType );
    int goodFreeQty = math::clamp<int>( _d->goodStore.freeQty( gType ), 0, Room::basicCapacity );

    if( good::Orders::deliver == order && goodFreeQty > 0 )
    {
      auto supplier = Walker::create<CartSupplier>( _city() );
      supplier->send2city( this, gType, goodFreeQty );

      if( !supplier->isDeleted() )
      {
        addWalker( supplier.object() );
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
    for( auto& goodType : good::all() )
    {
      int goodQty = _d->goodStore.qty( goodType );
      goodQty = math::clamp( goodQty, 0, maxCapacity);

      if( goodQty > 0 )
      {
        good::Stock stock( goodType, goodQty, goodQty);
        auto pusher = Walker::create<CartPusher>( _city() );
        pusher->stock().setCapacity( maxCapacity );
        pusher->send2city( BuildingPtr( this ), stock );

        if( !pusher->isDeleted() )
        {
          good::Stock tmpStock( goodType, goodQty );;
          _d->goodStore.retrieve( tmpStock, goodQty );
          addWalker( pusher.object() );
          break;
        }
      }
    }
  }
}
