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
#include "city/helper.hpp"
#include "core/position.hpp"
#include "objects/granary.hpp"
#include "objects/warehouse.hpp"
#include "gfx/tile.hpp"
#include "game/gamedate.hpp"
#include "good/goodhelper.hpp"
#include "core/variant.hpp"
#include "pathway/path_finding.hpp"
#include "gfx/animation_bank.hpp"
#include "objects/factory.hpp"
#include "name_generator.hpp"
#include "good/goodstore.hpp"
#include "objects/constants.hpp"
#include "core/direction.hpp"

using namespace constants;
using namespace gfx;

namespace {
const int defaultDeliverDistance = 40;
}

class CartSupplier::Impl
{
public:
  PlayerCityPtr city;
  GoodStock stock;
  TilePos storageBuildingPos;
  TilePos baseBuildingPos;
  Picture cartPicture;
  int maxDistance;
  long rcvReservationID;
  long reservationID;
};

CartSupplier::CartSupplier( PlayerCityPtr city )
  : Walker( city ), _d( new Impl )
{
  _setType( walker::cartPusher );

  _d->storageBuildingPos = TilePos( -1, -1 );
  _d->baseBuildingPos = TilePos( -1, -1 );
  _d->maxDistance = defaultDeliverDistance;
  _d->city = city;

  setName( NameGenerator::rand( NameGenerator::male ) );
}

void CartSupplier::_reachedPathway()
{
  Walker::_reachedPathway();
  city::Helper helper( _d->city );

  if( _pathwayRef().isReverse() )
  {
    // walker is back in the market
    deleteLater();
    // put the content of the stock to receiver
    BuildingPtr building = helper.find<Building>( building::any, _d->baseBuildingPos );
    GoodStore* storage = 0;
    FactoryPtr f = ptr_cast<Factory>( building );
    GranaryPtr g = ptr_cast<Granary>( building );
    WarehousePtr w = ptr_cast<Warehouse>( building );
    if( f.isValid() ) { storage = &f->store(); }
    else if( g.isValid() ) { storage = &g->store(); }
    else if( w.isValid() ){ storage = &w->store(); }

    if( storage )
    {
      storage->applyStorageReservation( _d->stock, _d->rcvReservationID );
      storage->store( _d->stock, _d->stock.qty() );
    }
    else
    {
      if( building.isValid() )
      {
        building->storeGoods( _d->stock );
      }
    }
  }
  else
  {
    // get goods from destination building
    BuildingPtr building = helper.find<Building>( building::any, _d->storageBuildingPos );
    GoodStore* storage = 0;
    FactoryPtr f = ptr_cast<Factory>( building );
    GranaryPtr g = ptr_cast<Granary>( building );
    WarehousePtr w = ptr_cast<Warehouse>( building );
    if( f.isValid() ) { storage = &f->store(); }
    else if( g.isValid() ) { storage = &g->store(); }
    else if( w.isValid() ){ storage = &w->store(); }

    if( storage )
    {
      storage->applyRetrieveReservation(_d->stock, _d->reservationID);
      _reserveStorage();
    }

    // walker is near the granary/warehouse
    _pathwayRef().rbegin();
    _centerTile();
    go();
  }
}


const Picture& CartSupplier::getCartPicture()
{
  if( !_d->cartPicture.isValid() )
  {
    _d->cartPicture = GoodHelper::getCartPicture( _d->stock, getDirection() );
  }

  return _d->cartPicture;
}

void CartSupplier::_changeDirection()
{
   Walker::_changeDirection();
   _d->cartPicture = Picture();  // need to get the new graphic
}

void CartSupplier::getPictures( Pictures& oPics)
{
   oPics.clear();

   // depending on the walker direction, the cart is ahead or behind
   switch (getDirection())
   {
   case constants::west:
   case constants::northWest:
   case constants::north:
   case constants::northEast:
      oPics.push_back( getCartPicture() );
      oPics.push_back( getMainPicture() );
   break;

   case constants::east:
   case constants::southEast:
   case constants::south:
   case constants::southWest:
      oPics.push_back( getMainPicture() );
      oPics.push_back( getCartPicture() );
   break;

   default:
   break;
   }
}

template< class T >
TilePos getSupplierDestination2( Propagator &pathPropagator, const TileOverlay::Type type,
                                 const Good::Type what, const int needQty,
                                 Pathway &oPathWay, long& reservId )
{
  SmartPtr< T > res;

  DirectRoutes pathWayList = pathPropagator.getRoutes( type );

  int max_qty = 0;

  // select the warehouse with the max quantity of requested goods
  for( DirectRoutes::iterator pathWayIt= pathWayList.begin();
       pathWayIt != pathWayList.end(); ++pathWayIt)
  {
    // for every warehouse within range
    BuildingPtr building= ptr_cast<Building>( pathWayIt->first );
    PathwayPtr pathWay= pathWayIt->second;

    SmartPtr< T > destBuilding = ptr_cast<T>( building );
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
    reservId = res->store().reserveRetrieval( what, qty, GameDate::current() );
    return res->pos();
  }
  else
  {
    return TilePos( -1, -1 );
  }
}

void CartSupplier::computeWalkerDestination(BuildingPtr building, const Good::Type type, const int qty )
{
  _d->storageBuildingPos = TilePos( -1, -1 );  // no destination yet

  // we have something to buy!
  // get the list of buildings within reach
  Pathway pathWay;
  Propagator pathPropagator( _d->city );
  pathPropagator.init( ptr_cast<Construction>( building ) );
  pathPropagator.setAllDirections( false );
  pathPropagator.propagate( _d->maxDistance);

  // try get that good from a granary
  _d->storageBuildingPos = getSupplierDestination2<Granary>( pathPropagator, building::granary,
                                                             type, qty, pathWay, _d->reservationID );

  if( _d->storageBuildingPos.i() < 0 )
  {
    // try get that good from a warehouse
    _d->storageBuildingPos = getSupplierDestination2<Warehouse>( pathPropagator, building::warehouse,
                                                                 type, qty, pathWay, _d->reservationID );
  }

  if( _d->storageBuildingPos.i() >= 0 )
  {
    // we found a destination!
    setPos( pathWay.getStartPos() );
    setPathway(pathWay);    
  }
  else
  {
    // we have nothing to buy, or cannot find what we need to buy
    deleteLater();
    return;
  }
}

void CartSupplier::send2city( BuildingPtr building, Good::Type what, const int qty )
{
  _d->stock.setType( what );
  _d->stock.setCapacity( qty );
  _d->baseBuildingPos = building->pos();

  computeWalkerDestination( building, what, qty );

  if( !isDeleted()  )
  {           
    _d->city->addWalker( WalkerPtr( this ) );
  }
}

void CartSupplier::_reserveStorage()
{
  city::Helper helper( _d->city );
  BuildingPtr b = helper.find<Building>( building::any, _d->baseBuildingPos );

  GoodStore* storage = 0;
  FactoryPtr f = ptr_cast<Factory>( b );
  GranaryPtr g = ptr_cast<Granary>( b );
  WarehousePtr w = ptr_cast<Warehouse>( b );
  if( f.isValid() ) { storage = &f->store(); }
  else if( g.isValid() ) { storage = &g->store(); }
  else if( w.isValid() ){ storage = &w->store(); }

  if( storage != 0 )
  {
    _d->rcvReservationID = storage->reserveStorage( _d->stock, GameDate::current() );
  }
  else
  {}
}

CartSupplierPtr CartSupplier::create(PlayerCityPtr city )
{
  CartSupplierPtr ret( new CartSupplier( city ) );
  ret->drop(); //delete automatically

  return ret;
}

void CartSupplier::save( VariantMap& stream ) const
{
  Walker::save( stream );
}

void CartSupplier::load( const VariantMap& stream )
{
  Walker::load( stream );
}

void CartSupplier::timeStep(const unsigned long time)
{
  Walker::timeStep( time );
}
