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

#include "objects/metadata.hpp"
#include "core/exception.hpp"
#include "city/city.hpp"
#include "game/gamedate.hpp"
#include "core/position.hpp"
#include "objects/granary.hpp"
#include "objects/warehouse.hpp"
#include "gfx/tile.hpp"
#include "good/goodhelper.hpp"
#include "core/variant.hpp"
#include "pathway/path_finding.hpp"
#include "gfx/picture_bank.hpp"
#include "objects/factory.hpp"
#include "good/goodstore.hpp"
#include "core/stringhelper.hpp"
#include "name_generator.hpp"
#include "gfx/tilemap.hpp"
#include "core/logger.hpp"
#include "objects/constants.hpp"
#include "corpse.hpp"
#include "core/foreach.hpp"
#include "game/resourcegroup.hpp"

using namespace constants;
using namespace gfx;

namespace {
const int defaultDeliverDistance = 40;
CAESARIA_LITERALCONST(stock)
CAESARIA_LITERALCONST(producerPos)
}

class CartPusher::Impl
{
public:
  GoodStock stock;
  BuildingPtr producerBuilding;
  BuildingPtr consumerBuilding;
  Picture cartPicture;
  int maxDistance;
  long reservationID;

  BuildingPtr getWalkerDestination_factory(Propagator& pathPropagator, Pathway& oPathWay);
  BuildingPtr getWalkerDestination_warehouse(Propagator& pathPropagator, Pathway& oPathWay);
  BuildingPtr getWalkerDestination_granary(Propagator& pathPropagator, Pathway& oPathWay);
};

CartPusher::CartPusher(PlayerCityPtr city )
  : Walker( city ), _d( new Impl )
{
  _setType( walker::cartPusher );
  _d->producerBuilding = NULL;
  _d->consumerBuilding = NULL;
  _d->maxDistance = defaultDeliverDistance;
  _d->stock.setCapacity( defaultCartCapacity );

  setName( NameGenerator::rand( NameGenerator::male ) );
}

void CartPusher::_reachedPathway()
{
  Walker::_reachedPathway();
  _d->cartPicture = Picture();

  if( _d->consumerBuilding != NULL )
  {
    GranaryPtr granary = ptr_cast<Granary>(_d->consumerBuilding);
    WarehousePtr warehouse = ptr_cast<Warehouse>(_d->consumerBuilding);
    FactoryPtr factory = ptr_cast<Factory>(_d->consumerBuilding);

    GoodStore* goodStore = 0;
    if( granary.isValid() ) { goodStore = &granary->store(); }
    else if( warehouse.isValid() ) { goodStore = &warehouse->store(); }
    else if( factory.isValid() ) { goodStore = &factory->store(); }

    if( goodStore )
    {
      goodStore->applyStorageReservation(_d->stock, _d->reservationID);
      _d->reservationID = 0;
    }
  }
  //
  if( !_pathwayRef().isReverse() )
  {
    _pathwayRef().toggleDirection();
    _centerTile();
    go();
    _d->consumerBuilding = 0;
  }
  else
  {
    deleteLater();
  }
}

GoodStock& CartPusher::getStock() {   return _d->stock;}
void CartPusher::setProducerBuilding(BuildingPtr building){   _d->producerBuilding = building;}
void CartPusher::setConsumerBuilding(BuildingPtr building){   _d->consumerBuilding = building;}

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

void CartPusher::getPictures( gfx::Pictures& oPics)
{
   oPics.clear();
   Point offset;

   switch( getDirection() )
   {
   case constants::west: offset = Point( 10, -5 ); break;
   case constants::east: offset = Point( -10, 5 ); break;
   case constants::north: offset = Point( -5, -5 ); break;
   case constants::south: offset = Point( 5, 5 ); break;
   default: break;
   }

   // depending on the walker direction, the cart is ahead or behind
   switch( getDirection() )
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

   foreach( it, oPics ) { it->addOffset( offset ); }
}

void CartPusher::computeWalkerDestination()
{
   // get the list of buildings within reach
   Pathway pathWay;
   Propagator pathPropagator( _city() );
   pathPropagator.setAllDirections( false );
   _d->consumerBuilding = 0;

   if( _d->producerBuilding.isNull() )
   {
     Logger::warning( "CartPusher destroyed: producerBuilding can't be NULL" );
     deleteLater();
     return;
   }

   pathPropagator.init( ptr_cast<Construction>(_d->producerBuilding) );
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
     setPos( pathWay.getStartPos() );
     setPathway( pathWay );
     go();
   }
   else
   {
     if( _d->producerBuilding->getAccessRoads().empty() )
     {
       deleteLater();
     }
     else
     {
       _setDirection( constants::north );
       Walker::wait( -1 );
       setPos( _d->producerBuilding->getAccessRoads().front()->pos() );
       _walk();
     }
   }
}

template< class T >
BuildingPtr reserveShortestPath( const TileOverlay::Type buildingType,
                                 GoodStock& stock, long& reservationID,
                                 Propagator &pathPropagator, Pathway& oPathWay )
{
  BuildingPtr res;
  DirectRoutes pathWayList = pathPropagator.getRoutes( buildingType );

  //remove factories with improrer storage
  DirectRoutes::iterator pathWayIt= pathWayList.begin();
  while( pathWayIt != pathWayList.end() )
  {
    // for every factory within range
    SmartPtr<T> building = ptr_cast<T>( pathWayIt->first );

    if( stock.qty() > building->store().getMaxStore( stock.type() ) )
    {
      pathWayList.erase( pathWayIt++ );
    }
    else
    {
      ++pathWayIt;
    }
  }

  //find shortest path
  int maxLength = 999;
  PathwayPtr shortestPath = 0;
  foreach( pathIt, pathWayList )
  {
    if( pathIt->second->length() < maxLength )
    {
      shortestPath = pathIt->second;
      maxLength = pathIt->second->length();
      res = ptr_cast<Building>( pathIt->first );
    }
  }

  if( res.isValid() )
  {
    SmartPtr<T> ptr = ptr_cast<T>( res );
    reservationID = ptr->store().reserveStorage( stock, GameDate::current() );
    if (reservationID != 0)
    {
      oPathWay = *(shortestPath.object());
    }
    else
    {
      res = BuildingPtr();
    }
  }


  return res;
}

BuildingPtr CartPusher::Impl::getWalkerDestination_factory(Propagator &pathPropagator, Pathway& oPathWay)
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

BuildingPtr CartPusher::Impl::getWalkerDestination_warehouse(Propagator &pathPropagator, Pathway& oPathWay)
{
  BuildingPtr res;

  res = reserveShortestPath<Warehouse>( building::warehouse, stock, reservationID, pathPropagator, oPathWay );

  return res;
}

BuildingPtr CartPusher::Impl::getWalkerDestination_granary(Propagator &pathPropagator, Pathway& oPathWay)
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

void CartPusher::send2city( BuildingPtr building, GoodStock& carry )
{
  _d->stock.append( carry );
  setProducerBuilding( building  );

  computeWalkerDestination();

  if( !isDeleted() )
  {
    _city()->addWalker( this );
  }
}

void CartPusher::timeStep( const unsigned long time )
{
  if( GameDate::isWeekChanged() && !_pathwayRef().isValid() )
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
  
  stream[ lc_stock ] = _d->stock.save();
  stream[ lc_producerPos ] = _d->producerBuilding.isValid()
                                ? _d->producerBuilding->pos() : TilePos( -1, -1 );

  stream[ "consumerPos" ] = _d->consumerBuilding.isValid() 
                                ? _d->consumerBuilding->pos() : TilePos( -1, -1 );

  stream[ "maxDistance" ] = _d->maxDistance;
  stream[ "reservationID" ] = static_cast<int>(_d->reservationID);
}

void CartPusher::load( const VariantMap& stream )
{
  Walker::load( stream );

  _d->stock.load( stream.get( lc_stock ).toList() );

  TilePos prPos( stream.get( lc_producerPos ).toTilePos() );
  Tile& prTile = _city()->tilemap().at( prPos );
  _d->producerBuilding = ptr_cast<Building>( prTile.overlay() );

  if( is_kind_of<WorkingBuilding>( _d->producerBuilding ) )
  {
    WorkingBuildingPtr wb = ptr_cast<WorkingBuilding>( _d->producerBuilding );
    wb->addWalker( this );
  }
  else
  {
    Logger::warning( "WARNING: cartPusher producer building is NULL uid=[%d]", uniqueId() );
  }

  TilePos cnsmPos( stream.get( "consumerPos" ).toTilePos() );
  _d->consumerBuilding = ptr_cast<Building>( _city()->getOverlay( cnsmPos ) );

  _d->maxDistance = stream.get( "maxDistance" ).toInt();
  _d->reservationID = stream.get( "reservationID" ).toInt();
}

bool CartPusher::die()
{
  bool created = Walker::die();

  if( !created )
  {
    Corpse::create( _city(), pos(), ResourceGroup::citizen1, 1025, 1032 );
    return true;
  }

  return created;
}

std::string CartPusher::getThinks() const
{
  if( !getPathway().isValid() )
  {
    return "##cartpusher_cantfind_destination##";
  }

  return Walker::getThinks();
}
