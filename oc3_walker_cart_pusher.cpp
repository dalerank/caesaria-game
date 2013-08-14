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
#include "oc3_city.hpp"
#include "oc3_positioni.hpp"
#include "oc3_granary.hpp"
#include "oc3_building_warehouse.hpp"
#include "oc3_tile.hpp"
#include "oc3_goodhelper.hpp"
#include "oc3_variant.hpp"
#include "oc3_path_finding.hpp"
#include "oc3_picture_bank.hpp"
#include "oc3_factory_building.hpp"
#include "oc3_goodstore.hpp"

class CartPusher::Impl
{
public:
  CityPtr city;
  GoodStock stock;
  BuildingPtr producerBuilding;
  BuildingPtr consumerBuilding;
  Picture cartPicture;
  int maxDistance;
  long reservationID;

  BuildingPtr getWalkerDestination_factory(Propagator& pathPropagator, PathWay &oPathWay);
  BuildingPtr getWalkerDestination_warehouse(Propagator& pathPropagator, PathWay &oPathWay);
  BuildingPtr getWalkerDestination_granary(Propagator& pathPropagator, PathWay &oPathWay);
};

CartPusher::CartPusher( CityPtr city ) : _d( new Impl )
{
   _walkerGraphic = WG_PUSHER;
   _walkerType = WT_CART_PUSHER;
   _d->producerBuilding = NULL;
   _d->consumerBuilding = NULL;
   _d->maxDistance = 25;
   _d->city = city;
}

void CartPusher::onDestination()
{
  Walker::onDestination();
  _d->cartPicture = Picture();

  if( _d->consumerBuilding != NULL )
  {
    GranaryPtr granary = _d->consumerBuilding.as<Granary>();
    WarehousePtr warehouse = _d->consumerBuilding.as<Warehouse>();
    FactoryPtr factory = _d->consumerBuilding.as<Factory>(); 
    if( granary != NULL )
    {
       granary->getGoodStore().applyStorageReservation(_d->stock, _d->reservationID);
       _d->reservationID = 0;
    }
    else if ( warehouse != NULL )
    {
       warehouse->getGoodStore().applyStorageReservation(_d->stock, _d->reservationID);
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
  if( !_getPathway().isReverse() )
  {
    _getPathway().toggleDirection();
    _setAction( WA_MOVE );
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
   if( !_d->cartPicture.isValid() )
   {
     _d->cartPicture = GoodHelper::getCartPicture(_d->stock, getDirection());
   }

   return _d->cartPicture;
}

void CartPusher::onNewDirection()
{
   Walker::onNewDirection();
   _d->cartPicture = Picture();  // need to get the new graphic
}

void CartPusher::getPictureList(std::vector<Picture> &oPics)
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

void CartPusher::computeWalkerDestination()
{
   // get the list of buildings within reach
   PathWay pathWay;
   Propagator pathPropagator( _d->city );
   _d->consumerBuilding = 0;

   _OC3_DEBUG_BREAK_IF( _d->producerBuilding.isNull() && "CartPusher: producerBuilding can't be NULL" );
   if( _d->producerBuilding.isNull() )
   {
     deleteLater();
     return;
   }

   pathPropagator.init( _d->producerBuilding.as<Construction>() );
   pathPropagator.propagate(_d->maxDistance);

   BuildingPtr destBuilding;
   if (destBuilding == NULL)
   {
      // try send that good to a factory
      destBuilding = _d->getWalkerDestination_factory(pathPropagator, pathWay);
   }

   if (destBuilding == NULL)
   {
      // try send that good to a granary
      destBuilding = _d->getWalkerDestination_granary(pathPropagator, pathWay);
   }

   if (destBuilding == NULL)
   {
      // try send that good to a warehouse
      destBuilding = _d->getWalkerDestination_warehouse( pathPropagator, pathWay );
   }

   if( destBuilding != NULL)
   {
      //_isDeleted = true;  // no destination!
     setConsumerBuilding( destBuilding );
     setPathWay( pathWay );
     setIJ( _getPathway().getOrigin().getIJ() );
     setSpeed( 1 );
   }
   else
   {
     _setDirection( D_NORTH );
     setSpeed( 0 );
     setIJ( _d->producerBuilding->getAccessRoads().front()->getIJ() );
     walk();
   }
}


BuildingPtr CartPusher::Impl::getWalkerDestination_factory(Propagator &pathPropagator, PathWay &oPathWay)
{
   BuildingPtr res;
   GoodType goodType = stock._goodType;
   BuildingType buildingType = BuildingDataHolder::instance().getBuildingTypeByInGood(goodType);

   if (buildingType == B_NONE)
   {
      // no factory can use this good
      return NULL;
   }

   Propagator::Ways pathWayList;
   pathPropagator.getReachedBuildings(buildingType, pathWayList);

   for( Propagator::Ways::iterator pathWayIt= pathWayList.begin(); pathWayIt != pathWayList.end(); ++pathWayIt)
   {
      // for every factory within range
      BuildingPtr building= pathWayIt->first;
      PathWay& pathWay= pathWayIt->second;

      FactoryPtr factory = building.as<Factory>();
      reservationID = factory->getGoodStore().reserveStorage( stock );
      if (reservationID != 0)
      {
         res = factory.as<Building>();
         oPathWay = pathWay;
         break;
      }
   }

   return res;
}

BuildingPtr CartPusher::Impl::getWalkerDestination_warehouse(Propagator &pathPropagator, PathWay &oPathWay)
{
   BuildingPtr res;

   Propagator::Ways pathWayList;
   pathPropagator.getReachedBuildings(B_WAREHOUSE, pathWayList);

   for( Propagator::Ways::iterator pathWayIt= pathWayList.begin(); pathWayIt != pathWayList.end(); ++pathWayIt)
   {
      // for every warehouse within range
      BuildingPtr building= pathWayIt->first;
      PathWay& pathWay= pathWayIt->second;

      WarehousePtr warehouse= building.as<Warehouse>();
      reservationID = warehouse->getGoodStore().reserveStorage( stock );
      if (reservationID != 0)
      {
         res = warehouse.as<Building>();
         oPathWay = pathWay;
         break;
      }
   }

   return res;
}

BuildingPtr CartPusher::Impl::getWalkerDestination_granary(Propagator &pathPropagator, PathWay &oPathWay)
{
   BuildingPtr res;

   GoodType goodType = stock._goodType;
   if (!(goodType == G_WHEAT || goodType == G_FISH || goodType == G_MEAT || goodType == G_FRUIT || goodType == G_VEGETABLE))
   {
      // this good cannot be stored in a granary
      return NULL;
   }

   Propagator::Ways pathWayList;
   pathPropagator.getReachedBuildings( B_GRANARY, pathWayList);

   // find a granary with enough storage
   for( Propagator::Ways::iterator pathWayIt= pathWayList.begin(); pathWayIt != pathWayList.end(); ++pathWayIt)
   {
      // for every granary within range
      BuildingPtr building= pathWayIt->first;
      PathWay& pathWay= pathWayIt->second;

      SmartPtr<Granary> granary= building.as<Granary>();
      reservationID = granary->getGoodStore().reserveStorage( stock );
      if (reservationID != 0)
      {
         res = granary.as<Building>();
         oPathWay = pathWay;
         break;
      }
   }

   return res;
}


void CartPusher::send2City( BuildingPtr building, const GoodStock& stock )
{
  setStock(stock);
  setProducerBuilding( building  );

  computeWalkerDestination();

  if( !isDeleted() )
  {
    _d->city->addWalker( this );
  }
}

void CartPusher::timeStep( const unsigned long time )
{
  if( (time % 22 == 1) && (_getPathway().getLength() < 2) )
  {
    computeWalkerDestination();
  }

  Walker::timeStep( time );
}

CartPusherPtr CartPusher::create( CityPtr city )
{
  CartPusherPtr ret( new CartPusher( city ) );
  ret->drop(); //delete automatically

  return ret;
}

void CartPusher::save( VariantMap& stream ) const
{
  Walker::save( stream );
  
  stream[ "stock" ] = _d->stock.save();
  stream[ "producerPos" ] = _d->producerBuilding->getTile().getIJ();
  stream[ "consumerPos" ] = _d->consumerBuilding.isValid() 
                                      ? _d->consumerBuilding->getTile().getIJ()
                                      : TilePos( -1, -1 );

  stream[ "maxDistance" ] = _d->maxDistance;
  stream[ "reservationID" ] = static_cast<int>(_d->reservationID);
}

void CartPusher::load( const VariantMap& stream )
{
  Walker::load( stream );

  _d->stock.load( stream.get( "stock" ).toList() );

  TilePos prPos( stream.get( "producerPos" ).toTilePos() );
  Tile& prTile = _d->city->getTilemap().at( prPos );
  _d->producerBuilding = prTile.getTerrain().getOverlay().as<Building>();
  
  if( _d->producerBuilding.is<WorkingBuilding>() )
  {
    _d->producerBuilding.as<WorkingBuilding>()->addWalker( this );
  }

  TilePos cnsmPos( stream.get( "consumerPos" ).toTilePos() );
  Tile& cnsmTile = _d->city->getTilemap().at( cnsmPos );
  _d->consumerBuilding = cnsmTile.getTerrain().getOverlay().as<Building>();

  _d->maxDistance = stream.get( "maxDistance" ).toInt();
  _d->reservationID = stream.get( "reservationID" ).toInt();
}
