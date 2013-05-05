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

#include <iostream>

#include "oc3_building_data.hpp"
#include "oc3_exception.hpp"
#include "oc3_scenario.hpp"
#include "oc3_positioni.hpp"


CartPusher::CartPusher()
{
   _cartPicture = NULL;
   _walkerGraphic = WG_PUSHER;
   _walkerType = WT_CART_PUSHER;
   _producerBuilding = NULL;
   _consumerBuilding = NULL;
   _maxDistance = 25;
}

void CartPusher::onDestination()
{
  Walker::onDestination();
  _cartPicture = NULL;

  if( _consumerBuilding.isValid() )
  {
    SmartPtr<Granary> granary = _consumerBuilding.as<Granary>();
    SmartPtr<Warehouse> warehouse = _consumerBuilding.as<Warehouse>();
    SmartPtr<Factory> factory = _consumerBuilding.as<Factory>(); 
    if( granary.isValid() )
    {
       granary->getGoodStore().applyStorageReservation(_stock, _reservationID);
       granary->computePictures();
       _reservationID = 0;
    }
    else if ( warehouse.isValid() )
    {
       warehouse->getGoodStore().applyStorageReservation(_stock, _reservationID);
       warehouse->computePictures();
       _reservationID = 0;
    }
    else if( factory.isValid() )
    {
       factory->getGoodStore().applyStorageReservation(_stock, _reservationID);
       // factory->computePictures();
       _reservationID = 0;
    }
  }
  //
  if( !_pathWay.isReverse() )
  {
    _pathWay.toggleDirection();
    _action._action=WA_MOVE;
    computeDirection();
    _consumerBuilding = 0;
  }
  else
  {
    SmartPtr<Factory> factory = _producerBuilding.as<Factory>();
    if( factory.isValid() )
    {
      factory->removeWalker( this );
    }

    deleteLater();
  }
}

void CartPusher::setStock(const GoodStock &stock)
{
   _stock = stock;
}

void CartPusher::setProducerBuilding(BuildingPtr building)
{
   _producerBuilding = building;
}

void CartPusher::setConsumerBuilding(BuildingPtr building)
{
   _consumerBuilding = building;
}

BuildingPtr CartPusher::getProducerBuilding()
{
   if( _producerBuilding.isNull() ) 
     THROW("ProducerBuilding is not initialized");
   return _producerBuilding;
}

BuildingPtr CartPusher::getConsumerBuilding()
{
   if( _consumerBuilding.isNull() ) 
     THROW("ConsumerBuilding is not initialized");
   
   return _consumerBuilding;
}

Picture& CartPusher::getCartPicture()
{
   if( _cartPicture == NULL )
   {
      _cartPicture = &CartLoader::instance().getCart(_stock, getDirection());
   }

   return *_cartPicture;
}

void CartPusher::onNewDirection()
{
   Walker::onNewDirection();
   _cartPicture = NULL;  // need to get the new graphic
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
   _consumerBuilding = 0;
   pathPropagator.init( *_producerBuilding.object() );
   pathPropagator.propagate(_maxDistance);

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
     setIJ( _producerBuilding->getAccessRoads().front()->getIJ() );
     walk();
   }
}


BuildingPtr CartPusher::getWalkerDestination_factory(Propagator &pathPropagator, PathWay &oPathWay)
{
   BuildingPtr res;
   GoodType goodType = _stock._goodType;
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
      _reservationID = factory->getGoodStore().reserveStorage(_stock);
      if (_reservationID != 0)
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
      _reservationID = warehouse->getGoodStore().reserveStorage(_stock);
      if (_reservationID != 0)
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

   GoodType goodType = _stock._goodType;
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
      _reservationID = granary->getGoodStore().reserveStorage(_stock);
      if (_reservationID != 0)
      {
         res = granary.as<Building>();
         oPathWay = pathWay;
         break;
      }
   }

   return res;
}


void CartPusher::start()
{
   computeWalkerDestination();
}

void CartPusher::timeStep( const unsigned long time )
{
  if( (time % 22 == 1) && (_pathWay.getLength() < 2) )
  {
    computeWalkerDestination();
  }

  Walker::timeStep( time );
}

