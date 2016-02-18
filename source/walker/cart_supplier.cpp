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
// Copyright 2012-2014 dalerank, dalerankn8@gmail.com

#include "cart_supplier.hpp"

#include "objects/metadata.hpp"
#include "core/exception.hpp"
#include "core/position.hpp"
#include "objects/granary.hpp"
#include "objects/warehouse.hpp"
#include "gfx/tile.hpp"
#include "city/city.hpp"
#include "core/variant_map.hpp"
#include "game/gamedate.hpp"
#include "good/helper.hpp"
#include "pathway/path_finding.hpp"
#include "gfx/animation_bank.hpp"
#include "objects/factory.hpp"
#include "name_generator.hpp"
#include "good/store.hpp"
#include "core/variant_list.hpp"
#include "objects/constants.hpp"
#include "events/removecitizen.hpp"
#include "city/trade_options.hpp"
#include "core/direction.hpp"
#include "walkers_factory.hpp"
#include "gfx/tilemap_config.hpp"
#include "gfx/tilemap.hpp"
#include "gfx/cart_animation.hpp"

using namespace gfx;

REGISTER_CLASS_IN_WALKERFACTORY(walker::supplier, CartSupplier)

namespace {
const int defaultDeliverDistance = 40;
}

class CartSupplier::Impl
{
public:
  good::Stock stock;
  TilePos storageBuildingPos;
  TilePos baseBuildingPos;
  CartAnimation anim;
  int maxDistance;
  long rcvReservationID;
  long reservationID;
};

CartSupplier::CartSupplier( PlayerCityPtr city )
  : Citizen( city, walker::supplier ), _d( new Impl )
{
  _d->storageBuildingPos = TilePos::invalid();
  _d->baseBuildingPos = TilePos::invalid();
  _d->maxDistance = defaultDeliverDistance;

  setName( NameGenerator::rand( NameGenerator::plebMale ) );
}

void CartSupplier::_reachedPathway()
{
  Walker::_reachedPathway();

  if( _pathway().isReverse() )
  {
    // walker is back in the market
    deleteLater();
    // put the content of the stock to receiver
    auto building = _map().overlay<Building>( _d->baseBuildingPos );

    if( building.isValid() )
    {
      good::Store& storage = building->store();

      if( storage.capacity() > 0 )
      {
        storage.applyStorageReservation( _d->stock, _d->rcvReservationID );
        storage.store( _d->stock, _d->stock.qty() );
      }
      else
      {
        building->storeGoods( _d->stock );
      }
    }
  }
  else
  {
    // get goods from destination building
    auto building = _map().overlay<Building>( _d->storageBuildingPos );

    if( building.isValid() )
    {
      good::Store& storage = building->store();

      if( storage.capacity() > 0 )
      {
        storage.applyRetrieveReservation(_d->stock, _d->reservationID);
        _reserveStorage();
      }

      //wait while load cart
      wait( _d->stock.qty() );

      // walker is near the granary/warehouse
      _pathway().move( Pathway::reverse );
      _centerTile();
      go();
    }
  }
}

const gfx::CartAnimation& CartSupplier::_cart()
{
  if( !_d->anim.isValid() )
  {
    _d->anim.load( _d->stock, direction() );
  }

  return _d->anim;
}

void CartSupplier::_changeDirection()
{
   Walker::_changeDirection();
   _d->anim = CartAnimation();  // need to get the new graphic
}

void CartSupplier::getPictures( Pictures& oPics)
{
  oPics.clear();

  // depending on the walker direction, the cart is ahead or behind
  switch (direction())
  {
  case direction::west:
  case direction::northWest:
  case direction::north:
  case direction::northEast:
     oPics.push_back( _cart().currentFrame() );
     oPics.push_back( getMainPicture() );
  break;

  case direction::east:
  case direction::southEast:
  case direction::south:
  case direction::southWest:
     oPics.push_back( getMainPicture() );
     oPics.push_back( _cart().currentFrame() );
  break;

  default:
  break;
  }

  if( _d->anim.isBack() )
  {
    std::iter_swap( oPics.begin(), oPics.begin() + 1);
  }
}

template< class T >
TilePos getSupplierDestination2( Propagator &pathPropagator, const object::Type type,
                                 const good::Product what, const int needQty,
                                 Pathway &oPathWay, long& reservId, BuildingPtr base )
{
  SmartPtr< T > res;

  DirectPRoutes pathWayList = pathPropagator.getRoutes( type );

  int max_qty = 0;

  // select the warehouse with the max quantity of requested goods
  for( auto& pathWayIt : pathWayList )
  {
    // for every warehouse within range
    BuildingPtr building= pathWayIt.first.as<Building>();

    if( building == base )
      continue;

    PathwayPtr pathWay= pathWayIt.second;

    SmartPtr<T> destBuilding = building.as<T>();
    int qty = destBuilding->store().getMaxRetrieve( what );
    if( qty > max_qty )
    {
      res = destBuilding;
      oPathWay = *pathWay.object();
      max_qty = qty;
    }
  }

  if( res.isValid() )
  {
    // a warehouse/granary has been found!
    // reserve some goods from that warehouse/granary
    int qty = math::clamp( needQty, 0, max_qty );
    reservId = res->store().reserveRetrieval( what, qty, game::Date::current() );
    return res->pos();
  }
  else
  {
    return TilePos::invalid();
  }
}

void CartSupplier::computeWalkerDestination(BuildingPtr building, const good::Product type, const int qty )
{
  _d->storageBuildingPos = TilePos::invalid();  // no destination yet

  if( _city()->tradeOptions().isStacking( type ) )
    return;

  // we have something to buy!
  // get the list of buildings within reach
  Pathway pathWay;
  Propagator pathPropagator( _city() );
  pathPropagator.init( building.as<Construction>() );
  pathPropagator.setAllDirections( false );
  pathPropagator.propagate( _d->maxDistance);

  // try get that good from a granary
  _d->storageBuildingPos = getSupplierDestination2<Granary>( pathPropagator, object::granery,
                                                             type, qty, pathWay, _d->reservationID,
                                                             building );

  if( _d->storageBuildingPos.i() < 0 )
  {
    // try get that good from a warehouse
    _d->storageBuildingPos = getSupplierDestination2<Warehouse>( pathPropagator, object::warehouse,
                                                                 type, qty, pathWay, _d->reservationID,
                                                                 building );
  }

  if( _d->storageBuildingPos.i() >= 0 )
  {
    // we found a destination!
    setPos( pathWay.startPos() );
    setPathway(pathWay);    
  }
  else
  {
    // we have nothing to buy, or cannot find what we need to buy
    deleteLater();
    return;
  }
}

void CartSupplier::send2city( BuildingPtr building, good::Product what, const int qty )
{
  _d->stock.setType( what );
  _d->stock.setCapacity( qty );
  _d->baseBuildingPos = building->pos();

  computeWalkerDestination( building, what, qty );
  attach();
}

void CartSupplier::_reserveStorage()
{
  auto building = _map().overlay<Building>( _d->baseBuildingPos );

  if( building.isValid() )
  {
    _d->rcvReservationID = building->store().reserveStorage( _d->stock, game::Date::current() );
  }
}

void CartSupplier::save( VariantMap& stream ) const
{
  Walker::save( stream );

  VARIANT_SAVE_CLASS_D( stream, _d, stock )
  VARIANT_SAVE_ANY_D( stream,   _d, storageBuildingPos )
  VARIANT_SAVE_ANY_D( stream,   _d, baseBuildingPos )
  VARIANT_SAVE_ANY_D( stream,   _d, maxDistance )
  VARIANT_SAVE_ENUM_D( stream,  _d, rcvReservationID )
  VARIANT_SAVE_ENUM_D( stream,  _d, reservationID )
}

void CartSupplier::load( const VariantMap& stream )
{
  Walker::load( stream );

  VARIANT_LOAD_CLASS_D_LIST( _d, stock,             stream );
  VARIANT_LOAD_ANY_D(        _d, storageBuildingPos,stream )
  VARIANT_LOAD_ANY_D(        _d, baseBuildingPos,   stream )
  VARIANT_LOAD_ANY_D(        _d, maxDistance,       stream )
  VARIANT_LOAD_ENUM_D(       _d, rcvReservationID,  stream )
  VARIANT_LOAD_ENUM_D(       _d, reservationID,     stream )
}

bool CartSupplier::die()
{
  events::dispatch<events::RemoveCitizens>( pos(), CitizenGroup( CitizenGroup::mature, 1) );

  return Walker::die();
}

void CartSupplier::timeStep(const unsigned long time)
{
  _d->anim.update( time );
  Walker::timeStep( time );
}

TilePos CartSupplier::places(Walker::Place type) const
{
  switch( type )
  {
  case plOrigin: return _d->baseBuildingPos;
  case plDestination: return _d->storageBuildingPos;
  default: break;
  }

  return Human::places( type );
}

