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

#include "cart_pusher.hpp"

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
#include "gfx/picture_bank.hpp"
#include "building/factory.hpp"
#include "game/goodstore.hpp"
#include "core/stringhelper.hpp"
#include "game/name_generator.hpp"
#include "gfx/tilemap.hpp"
#include "core/logger.hpp"
#include "building/constants.hpp"
#include "corpse.hpp"
#include "game/resourcegroup.hpp"

using namespace constants;

class CartPusher::Impl
{
public:
  GoodStock stock;
  BuildingPtr producerBuilding;
  BuildingPtr consumerBuilding;
  Picture cartPicture;
  int maxDistance;
  long reservationID;

  BuildingPtr getWalkerDestination_factory(Propagator& pathPropagator, Pathway &oPathWay);
  BuildingPtr getWalkerDestination_warehouse(Propagator& pathPropagator, Pathway &oPathWay);
  BuildingPtr getWalkerDestination_granary(Propagator& pathPropagator, Pathway &oPathWay);
};

CartPusher::CartPusher(PlayerCityPtr city )
  : Walker( city ), _d( new Impl )
{
  _setAnimation( gfx::cartPusher );
  _setType( walker::cartPusher );
  _d->producerBuilding = NULL;
  _d->consumerBuilding = NULL;
  _d->maxDistance = 25;
  _d->stock._maxQty = 400;

  setName( NameGenerator::rand( NameGenerator::male ) );
}

void CartPusher::_reachedPathway()
{
  Walker::_reachedPathway();
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
  if( !_pathwayRef().isReverse() )
  {
    _pathwayRef().toggleDirection();
    _computeDirection();
    go();
    _d->consumerBuilding = 0;
  }
  else
  {
    deleteLater();
  }
}

GoodStock& CartPusher::getStock()
{
   return _d->stock;
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

void CartPusher::_changeDirection()
{
   Walker::_changeDirection();
   _d->cartPicture = Picture();  // need to get the new graphic
}

void CartPusher::getPictureList(std::vector<Picture> &oPics)
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

void CartPusher::computeWalkerDestination()
{
   // get the list of buildings within reach
   Pathway pathWay;
   Propagator pathPropagator( _getCity() );
   pathPropagator.setAllDirections( false );
   _d->consumerBuilding = 0;

   if( _d->producerBuilding.isNull() )
   {
     Logger::warning( "CartPusher destroyed: producerBuilding can't be NULL" );
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
     setIJ( pathWay.getOrigin().getIJ() );
     setPathway( pathWay );
     setSpeed( 1 );
   }
   else
   {
     _setDirection( constants::north );
     setSpeed( 0 );
     setIJ( _d->producerBuilding->getAccessRoads().front()->getIJ() );
     _walk();
   }
}

template< class T >
BuildingPtr reserveShortestPath( const TileOverlay::Type buildingType,
                                     GoodStock& stock, long& reservationID,
                                     Propagator &pathPropagator, Pathway &oPathWay )
{
  BuildingPtr res;
  Propagator::Routes pathWayList = pathPropagator.getRoutes( buildingType );

  //remove factories with improrer storage
  Propagator::Routes::iterator pathWayIt= pathWayList.begin();
  while( pathWayIt != pathWayList.end() )
  {
    // for every factory within range
    SmartPtr<T> building = pathWayIt->first.as<T>();

    if( stock._currentQty >  building->getGoodStore().getMaxStore( stock.type() ) )
    {
      pathWayList.erase( pathWayIt++ );
    }
    else
    {
      pathWayIt++;
    }
  }

  //find shortest path
  int maxLength = 999;
  Pathway* shortestPath = 0;
  for( Propagator::Routes::iterator pathIt = pathWayList.begin(); 
    pathIt != pathWayList.end(); pathIt++ )
  {
    if( pathIt->second.getLength() < maxLength )
    {
      shortestPath = &pathIt->second;
      maxLength = pathIt->second.getLength();
      res = pathIt->first.as<Building>();
    }
  }

  if( res.isValid() )
  {
    reservationID = res.as<T>()->getGoodStore().reserveStorage( stock );
    if (reservationID != 0)
    {
      oPathWay = *shortestPath;
    }
    else
    {
      res = BuildingPtr();
    }
  }


  return res;
}

BuildingPtr CartPusher::Impl::getWalkerDestination_factory(Propagator &pathPropagator, Pathway &oPathWay)
{
  BuildingPtr res;
  Good::Type goodType = stock.type();
  TileOverlay::Type buildingType = MetaDataHolder::instance().getConsumerType( goodType );

  if (buildingType == building::unknown)
  {
     // no factory can use this good
     return 0;
  }

  res = reserveShortestPath<Factory>( buildingType, stock, reservationID, pathPropagator, oPathWay );

  return res;
}

BuildingPtr CartPusher::Impl::getWalkerDestination_warehouse(Propagator &pathPropagator, Pathway &oPathWay)
{
  BuildingPtr res;

  res = reserveShortestPath<Warehouse>( building::warehouse, stock, reservationID, pathPropagator, oPathWay );

  return res;
}

BuildingPtr CartPusher::Impl::getWalkerDestination_granary(Propagator &pathPropagator, Pathway &oPathWay)
{
   BuildingPtr res;

   Good::Type goodType = stock.type();
   if (!(goodType == Good::wheat || goodType == Good::fish
         || goodType == Good::meat || goodType == Good::fruit || goodType == Good::vegetable))
   {
      // this good cannot be stored in a granary
      return 0;
   }

   res = reserveShortestPath<Granary>( building::granary, stock, reservationID, pathPropagator, oPathWay );

   return res;
}

void CartPusher::send2City( BuildingPtr building, GoodStock& carry )
{
  _d->stock.append( carry );
  setProducerBuilding( building  );

  computeWalkerDestination();

  if( !isDeleted() )
  {
    _getCity()->addWalker( this );
  }
}

void CartPusher::timeStep( const unsigned long time )
{
  if( (time % 22 == 1) && !_pathwayRef().isValid() )
  {
    computeWalkerDestination();
  }

  Walker::timeStep( time );
}

CartPusherPtr CartPusher::create(PlayerCityPtr city )
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
  Tile& prTile = _getCity()->getTilemap().at( prPos );
  _d->producerBuilding = prTile.getOverlay().as<Building>();
  
  if( _d->producerBuilding.is<WorkingBuilding>() )
  {
    _d->producerBuilding.as<WorkingBuilding>()->addWalker( this );
  }

  TilePos cnsmPos( stream.get( "consumerPos" ).toTilePos() );
  Tile& cnsmTile = _getCity()->getTilemap().at( cnsmPos );
  _d->consumerBuilding = cnsmTile.getOverlay().as<Building>();

  _d->maxDistance = stream.get( "maxDistance" ).toInt();
  _d->reservationID = stream.get( "reservationID" ).toInt();
}

void CartPusher::die()
{
  Walker::die();

  Corpse::create( _getCity(), getIJ(), ResourceGroup::citizen1, 1025, 1032 );
}
