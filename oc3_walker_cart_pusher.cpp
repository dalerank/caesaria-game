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

#include "oc3_walker_cart_pusher.hpp"

#include "oc3_building_data.hpp"
#include "oc3_exception.hpp"
#include "oc3_scenario.hpp"
#include "oc3_positioni.hpp"
#include "oc3_granary.hpp"
#include "oc3_warehouse.hpp"
#include "oc3_tile.hpp"
#include "oc3_variant.hpp"

class CartPusher::Impl
{
public:
  GoodStock stock;
  BuildingPtr producerBuilding;
  BuildingPtr consumerBuilding;
  Picture* cartPicture;
  int maxDistance;
  long reservationID;
};

CartPusher::CartPusher() : _d( new Impl )
{
   _d->cartPicture = NULL;
   _walkerGraphic = WG_PUSHER;
   _walkerType = WT_CART_PUSHER;
   _d->producerBuilding = NULL;
   _d->consumerBuilding = NULL;
   _d->maxDistance = 25;
}

void CartPusher::onDestination()
{
  Walker::onDestination();
  _d->cartPicture = NULL;

  if( _d->consumerBuilding != NULL )
  {
    GranaryPtr granary = _d->consumerBuilding.as<Granary>();
    WarehousePtr warehouse = _d->consumerBuilding.as<Warehouse>();
    FactoryPtr factory = _d->consumerBuilding.as<Factory>(); 
    if( granary != NULL )
    {
       granary->getGoodStore().applyStorageReservation(_d->stock, _d->reservationID);
       granary->computePictures();
       _d->reservationID = 0;
    }
    else if ( warehouse != NULL )
    {
       warehouse->getGoodStore().applyStorageReservation(_d->stock, _d->reservationID);
       warehouse->computePictures();
       _d->reservationID = 0;
    }
    else if( factory != NULL )
    {
       factory->getGoodStore().applyStorageReservation(_d->stock, _d->reservationID);
       // factory->computePictures();
       _d->reservationID = 0;
    }
  }
  //
  if( !_pathWay.isReverse() )
  {
    _pathWay.toggleDirection();
    _action._action=WA_MOVE;
    computeDirection();
    _d->consumerBuilding = 0;
  }
  else
  {
    deleteLater();
  }
}

void CartPusher::setStock(const GoodStock &stock)
{
   _d->stock = stock;
}

void CartPusher::setProducerBuilding(BuildingPtr building)
{
   _d->producerBuilding = building;
}

void CartPusher::setConsumerBuilding(BuildingPtr building)
{
   _d->consumerBuilding = building;
}

BuildingPtr CartPusher::getProducerBuilding()
{
   if( _d->producerBuilding.isNull() ) 
     THROW("ProducerBuilding is not initialized");
   return _d->producerBuilding;
}

BuildingPtr CartPusher::getConsumerBuilding()
{
   if( _d->consumerBuilding.isNull() ) 
     THROW("ConsumerBuilding is not initialized");
   
   return _d->consumerBuilding;
}

Picture& CartPusher::getCartPicture()
{
   if( _d->cartPicture == NULL )
   {
      _d->cartPicture = &CartLoader::instance().getCart(_d->stock, getDirection());
   }

   return *_d->cartPicture;
}

void CartPusher::onNewDirection()
{
   Walker::onNewDirection();
   _d->cartPicture = NULL;  // need to get the new graphic
}

void CartPusher::getPictureList(std::vector<Picture*> &oPics)
{
   oPics.clear();

   // depending on the walker direction, the cart is ahead or behind
   switch (getDirection())
   {
   case D_WEST:
   case D_NORTH_WEST:
   case D_NORTH:
   case D_NORTH_EAST:
      oPics.push_back(&getCartPicture());
      oPics.push_back(&getMainPicture());
      break;
   case D_EAST:
   case D_SOUTH_EAST:
   case D_SOUTH:
   case D_SOUTH_WEST:
      oPics.push_back(&getMainPicture());
      oPics.push_back(&getCartPicture());
      break;
   default:
      break;
   }
}

void CartPusher::computeWalkerDestination()
{
   // get the list of buildings within reach
   PathWay pathWay;
   Propagator pathPropagator;
   _d->consumerBuilding = 0;
   pathPropagator.init( *_d->producerBuilding.object() );
   pathPropagator.propagate(_d->maxDistance);

   BuildingPtr destBuilding;
   if (destBuilding == NULL)
   {
      // try send that good to a factory
      destBuilding = getWalkerDestination_factory(pathPropagator, pathWay);
   }

   if (destBuilding == NULL)
   {
      // try send that good to a granary
      destBuilding = getWalkerDestination_granary(pathPropagator, pathWay);
   }

   if (destBuilding == NULL)
   {
      // try send that good to a warehouse
      destBuilding = getWalkerDestination_warehouse( pathPropagator, pathWay );
   }

   if( destBuilding != NULL)
   {
      //_isDeleted = true;  // no destination!
     setConsumerBuilding( destBuilding );
     setPathWay( pathWay );
     setIJ( _pathWay.getOrigin().getIJ() );
     setSpeed( 1 );
   }
   else
   {
     _action._direction = D_NORTH;
     setSpeed( 0 );
     setIJ( _d->producerBuilding->getAccessRoads().front()->getIJ() );
     walk();
   }
}


BuildingPtr CartPusher::getWalkerDestination_factory(Propagator &pathPropagator, PathWay &oPathWay)
{
   BuildingPtr res;
   GoodType goodType = _d->stock._goodType;
   BuildingType buildingType = BuildingDataHolder::instance().getBuildingTypeByInGood(goodType);

   if (buildingType == B_NONE)
   {
      // no factory can use this good
      return NULL;
   }

   Propagator::ReachedBuldings pathWayList;
   pathPropagator.getReachedBuildings(buildingType, pathWayList);

   for( Propagator::ReachedBuldings::iterator pathWayIt= pathWayList.begin(); pathWayIt != pathWayList.end(); ++pathWayIt)
   {
      // for every factory within range
      BuildingPtr building= pathWayIt->first;
      PathWay& pathWay= pathWayIt->second;

      SmartPtr<Factory> factory = building.as<Factory>();
      _d->reservationID = factory->getGoodStore().reserveStorage(_d->stock);
      if (_d->reservationID != 0)
      {
         res = factory.as<Building>();
         oPathWay = pathWay;
         break;
      }
   }

   return res;
}

BuildingPtr CartPusher::getWalkerDestination_warehouse(Propagator &pathPropagator, PathWay &oPathWay)
{
   BuildingPtr res;

   Propagator::ReachedBuldings pathWayList;
   pathPropagator.getReachedBuildings(B_WAREHOUSE, pathWayList);

   for( Propagator::ReachedBuldings::iterator pathWayIt= pathWayList.begin(); pathWayIt != pathWayList.end(); ++pathWayIt)
   {
      // for every warehouse within range
      BuildingPtr building= pathWayIt->first;
      PathWay& pathWay= pathWayIt->second;

      SmartPtr<Warehouse> warehouse= building.as<Warehouse>();
      _d->reservationID = warehouse->getGoodStore().reserveStorage(_d->stock);
      if (_d->reservationID != 0)
      {
         res = warehouse.as<Building>();
         oPathWay = pathWay;
         break;
      }
   }

   return res;
}


BuildingPtr CartPusher::getWalkerDestination_granary(Propagator &pathPropagator, PathWay &oPathWay)
{
   BuildingPtr res;

   GoodType goodType = _d->stock._goodType;
   if (!(goodType == G_WHEAT || goodType == G_FISH || goodType == G_MEAT || goodType == G_FRUIT || goodType == G_VEGETABLE))
   {
      // this good cannot be stored in a granary
      return NULL;
   }

   Propagator::ReachedBuldings pathWayList;
   pathPropagator.getReachedBuildings( B_GRANARY, pathWayList);

   // find a granary with enough storage
   for( Propagator::ReachedBuldings::iterator pathWayIt= pathWayList.begin(); pathWayIt != pathWayList.end(); ++pathWayIt)
   {
      // for every granary within range
      BuildingPtr building= pathWayIt->first;
      PathWay& pathWay= pathWayIt->second;

      SmartPtr<Granary> granary= building.as<Granary>();
      _d->reservationID = granary->getGoodStore().reserveStorage(_d->stock);
      if (_d->reservationID != 0)
      {
         res = granary.as<Building>();
         oPathWay = pathWay;
         break;
      }
   }

   return res;
}


void CartPusher::send2City()
{
  computeWalkerDestination();

  if( !isDeleted() )
    Scenario::instance().getCity().addWalker( WalkerPtr( this ) );
}

void CartPusher::timeStep( const unsigned long time )
{
  if( (time % 22 == 1) && (_pathWay.getLength() < 2) )
  {
    computeWalkerDestination();
  }

  Walker::timeStep( time );
}

CartPusherPtr CartPusher::create( BuildingPtr building, const GoodStock& stock )
{
  CartPusherPtr ret( new CartPusher() );
  ret->drop(); //delete automaticlly
  ret->setStock(stock);
  ret->setProducerBuilding( building  );

  return ret;
}

void CartPusher::save( VariantMap& stream ) const
{
  Walker::save( stream );
  VariantMap vm_stock;
  _d->stock.save( vm_stock );
  stream[ "stock" ] = vm_stock;
  TilePos prPos = _d->producerBuilding->getTile().getIJ();
  stream[ "producerI" ] = prPos.getI();
  stream[ "producesJ" ] = prPos.getJ();
  TilePos cnsmPos = _d->consumerBuilding->getTile().getIJ();
  stream[ "consumerI" ] = cnsmPos.getI();
  stream[ "consumerJ" ] = cnsmPos.getJ();

  stream[ "maxDistance" ] = _d->maxDistance;
  stream[ "reservationID" ] = _d->reservationID;
}

void CartPusher::load( const VariantMap& stream )
{
  Walker::load( stream );
  VariantMap vm_stock = stream.get( "stock" ).toMap();
  _d->stock.load( vm_stock );

  TilePos prPos( stream.get( "producerI" ).toInt(), stream.get( "producesJ" ).toInt() );
  Tile& prTile = Scenario::instance().getCity().getTilemap().at( prPos );
  _d->producerBuilding = prTile.get_terrain().getOverlay().as<Building>();

  TilePos cnsmPos( stream.get( "consumerI" ).toInt(), stream.get( "consumerJ" ).toInt() );
  Tile& cnsmTile = Scenario::instance().getCity().getTilemap().at( cnsmPos );
  _d->producerBuilding = prTile.get_terrain().getOverlay().as<Building>();

  _d->maxDistance = stream.get( "maxDistance" ).toInt();
  _d->reservationID = stream.get( "reservationID" ).toInt();
}