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

#include "cart_supplier.hpp"

#include "building/metadata.hpp"
#include "core/exception.hpp"
#include "game/city.hpp"
#include "core/position.hpp"
#include "building/granary.hpp"
#include "building/warehouse.hpp"
#include "gfx/tile.hpp"
#include "game/goodhelper.hpp"
#include "core/variant.hpp"
#include "game/path_finding.hpp"
#include "gfx/animation_bank.hpp"
#include "building/factory.hpp"
#include "game/name_generator.hpp"
#include "game/goodstore.hpp"
#include "building/constants.hpp"

using namespace constants;

class CartSupplier::Impl
{
public:
  CityPtr city;
  GoodStock stock;
  TilePos storageBuildingPos;
  TilePos baseBuildingPos;
  Picture cartPicture;
  int maxDistance;
  long rcvReservationID;
  long reservationID;
};

CartSupplier::CartSupplier( CityPtr city )
  : Walker( city ), _d( new Impl )
{
  _setGraphic( WG_PUSHER );
  _setType( walker::cartPusher );

  _d->storageBuildingPos = TilePos( -1, -1 );
  _d->baseBuildingPos = TilePos( -1, -1 );
  _d->maxDistance = 25;
  _d->city = city;

  setName( NameGenerator::rand( NameGenerator::male ) );
}

void CartSupplier::onDestination()
{
  Walker::onDestination();
  CityHelper helper( _d->city );
  
  if( _getPathway().isReverse() )
  {
    // walker is back in the market
    deleteLater();
    // put the content of the stock to receiver
    BuildingPtr building = helper.find<Building>( _d->baseBuildingPos );

    GoodStore* storage = 0;
    if( building.is<Factory>() )
    {
      storage = &building.as<Factory>()->getGoodStore();
    }
    else if( building.is<Granary>() )
    {
      storage = &building.as<Granary>()->getGoodStore();
    }
    else if( building.is<Warehouse>() )
    {
      storage = &building.as<Warehouse>()->getGoodStore();
    }

    if( storage )
    {
      storage->applyStorageReservation( _d->stock, _d->rcvReservationID );
      storage->store( _d->stock, _d->stock._currentQty );
    }
  }
  else
  {
    // walker is near the granary/warehouse
    _getPathway().rbegin();
    computeDirection();
    go();

    // get goods from destination building
    
    BuildingPtr building = helper.find<Building>( _d->storageBuildingPos );

    if( building.is<Granary>() )
    {
      GranaryPtr granary = building.as<Granary>();
      // this is a granary!
      // std::cout << "MarketBuyer arrives at granary, res=" << _reservationID << std::endl;
      granary->getGoodStore().applyRetrieveReservation( _d->stock, _d->reservationID );      
    }
    else if( building.is<Warehouse>() )
    {
      WarehousePtr warehouse = building.as<Warehouse>();
      // this is a warehouse!
      // std::cout << "Market buyer takes IRON from warehouse" << std::endl;
      // warehouse->retrieveGoods(_basket.getStock(G_IRON));
      warehouse->getGoodStore().applyRetrieveReservation(_d->stock, _d->reservationID);
    }
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

void CartSupplier::onNewDirection()
{
   Walker::onNewDirection();
   _d->cartPicture = Picture();  // need to get the new graphic
}

void CartSupplier::getPictureList(std::vector<Picture> &oPics)
{
   oPics.clear();

   // depending on the walker direction, the cart is ahead or behind
   switch (getDirection())
   {
   case D_WEST:
   case D_NORTH_WEST:
   case D_NORTH:
   case D_NORTH_EAST:
      oPics.push_back( getCartPicture() );
      oPics.push_back( getMainPicture() );
      break;
   case D_EAST:
   case D_SOUTH_EAST:
   case D_SOUTH:
   case D_SOUTH_WEST:
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
                                 PathWay &oPathWay, long& reservId )
{
  SmartPtr< T > res;

  Propagator::Routes pathWayList = pathPropagator.getRoutes( type );

  int max_qty = 0;

  // select the warehouse with the max quantity of requested goods
  for( Propagator::Routes::iterator pathWayIt= pathWayList.begin(); 
       pathWayIt != pathWayList.end(); ++pathWayIt)
  {
    // for every warehouse within range
    BuildingPtr building= pathWayIt->first.as<Building>();
    PathWay& pathWay= pathWayIt->second;

    SmartPtr< T > destBuilding = building.as< T >();
    int qty = destBuilding->getGoodStore().getMaxRetrieve( what );
    if( qty > max_qty )
    {
      res = destBuilding;
      oPathWay = pathWay;
      max_qty = qty;
    }
  }

  if( res.isValid() )
  {
    // a warehouse/granary has been found!
    // reserve some goods from that warehouse/granary
    int qty = math::clamp( needQty, 0, max_qty );
    GoodStock tmpStock( what, qty, qty);
    reservId = res->getGoodStore().reserveRetrieval( tmpStock );
    return res->getTilePos();
  }
  else
  {
    return TilePos( -1, -1 );
  }
}

void CartSupplier::computeWalkerDestination(BuildingPtr building, const Good::Type type, const int qty )
{
  _d->baseBuildingPos = building->getTilePos();
  _d->storageBuildingPos = TilePos( -1, -1 );  // no destination yet

  // we have something to buy!
  // get the list of buildings within reach
  PathWay pathWay;
  Propagator pathPropagator( _d->city );
  pathPropagator.init( building.as<Construction>() );
  pathPropagator.propagate( _d->maxDistance);

  // try get that good from a granary
  _d->storageBuildingPos = getSupplierDestination2<Granary>( pathPropagator, building::granary,
                                                             type, qty, pathWay, _d->reservationID );

  if( _d->storageBuildingPos.getI() < 0 )
  {
    // try get that good from a warehouse
    _d->storageBuildingPos = getSupplierDestination2<Warehouse>( pathPropagator, building::B_WAREHOUSE,
                                                                 type, qty, pathWay, _d->reservationID );
  }

  if( _d->storageBuildingPos.getI() >= 0 )
  {
    // we found a destination!
    setPathWay(pathWay);    
  }
  else
  {
    // we have nothing to buy, or cannot find what we need to buy
    deleteLater();
    return;
  }

  setIJ( _getPathway().getOrigin().getIJ() );
}

void CartSupplier::send2City( BuildingPtr building, const Good::Type type, const int qty )
{
  computeWalkerDestination( building, type, qty );

  GoodStore* storage = 0;
  if( building.is<Factory>() )
  {
    storage = &building.as<Factory>()->getGoodStore();
  }
  else if( building.is<Granary>() )
  {
    storage = &building.as<Granary>()->getGoodStore();
  }
  else if( building.is<Warehouse>() )
  {
    storage = &building.as<Warehouse>()->getGoodStore();
  }

  if( !isDeleted() && storage )
  {   
    _d->stock.setType( type );
    _d->stock._maxQty = qty;
    _d->rcvReservationID = storage->reserveStorage( _d->stock );
    _d->city->addWalker( WalkerPtr( this ) );
  }
}

CartSupplierPtr CartSupplier::create( CityPtr city )
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
