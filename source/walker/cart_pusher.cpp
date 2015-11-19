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

#include "cart_pusher.hpp"

#include "objects/metadata.hpp"
#include "name_generator.hpp"
#include "objects/factory.hpp"
#include "gfx/tilemap.hpp"
#include "objects/warehouse.hpp"
#include "objects/granary.hpp"
#include "good/store.hpp"
#include "pathway/path_finding.hpp"
#include "city/city.hpp"
#include "core/variant_map.hpp"
#include "core/variant_list.hpp"
#include "game/gamedate.hpp"
#include "events/removecitizen.hpp"
#include "game/resourcegroup.hpp"
#include "corpse.hpp"
#include "gfx/tilemap_config.hpp"
#include "gfx/cart_animation.hpp"
#include "pathway/pathway_helper.hpp"
#include "walkers_factory.hpp"
#include "core/logger.hpp"
#include "city/trade_options.hpp"
#include "config.hpp"

using namespace gfx;
using namespace events;
using namespace config;

REGISTER_CLASS_IN_WALKERFACTORY(walker::cartPusher, CartPusher)

namespace {
CAESARIA_LITERALCONST(stock)
CAESARIA_LITERALCONST(producerPos)
CAESARIA_LITERALCONST(consumerPos)
}

class CartPusher::Impl
{
public:
  good::Stock stock;
  BuildingPtr producerBuilding;
  BuildingPtr consumerBuilding;
  int brokePathCounter;
  CartAnimation anim;
  int maxDistance;
  long reservationID;
  bool cantUnloadGoods;

  BuildingPtr getWalkerDestination_factory(Propagator& pathPropagator, Pathway& oPathWay);
  BuildingPtr getWalkerDestination_warehouse(Propagator& pathPropagator, Pathway& oPathWay);
  BuildingPtr getWalkerDestination_granary(Propagator& pathPropagator, Pathway& oPathWay);
};

CartPusher::CartPusher(PlayerCityPtr city, CartCapacity cap)
  : Human( city, walker::cartPusher  ), _d( new Impl )
{
  _d->producerBuilding = nullptr;
  _d->consumerBuilding = nullptr;
  _d->cantUnloadGoods = false;
  _d->brokePathCounter = 0;
  _d->stock.setCapacity( cap );
  _d->maxDistance = distance::maxDeliver;
  _d->stock.setCapacity( simpleCart );
}

void CartPusher::_reachedPathway()
{
  Walker::_reachedPathway();
  _d->anim = CartAnimation();

  if( consumerBuilding() != nullptr )
  {   
    good::Store& store = consumerBuilding()->store();

    if( store.capacity() > 0 )
    {
      int saveQty = _d->stock.qty();
      wait( _d->stock.qty() );
      store.applyStorageReservation(_d->stock, _d->reservationID);
      _d->reservationID = 0;
      _d->cantUnloadGoods = (saveQty == _d->stock.qty());

      if( producerBuilding() != nullptr )
      {
        int storedQty = saveQty - _d->stock.qty();
        producerBuilding()->store().confirmDeliver( _d->stock.type(), storedQty,
                                                    consumerBuilding()->pos().hash(),
                                                    game::Date::current() );
      }
    }
  }
  //
  if( !_pathway().isReverse() )
  {
    _pathway().toggleDirection();
    _centerTile();
    go();
    _d->consumerBuilding = nullptr;
  }
  else
  {
    deleteLater();
  }
}

void CartPusher::_changeTile()
{
  _d->brokePathCounter = 0;
  Human::_changeTile();
}

void CartPusher::_brokePathway(TilePos pos)
{
  _d->brokePathCounter++;
  if( _pathway().isValid() && _d->brokePathCounter < 5 )
  {
    Pathway way = PathwayHelper::create( pos, _pathway().stopPos(), PathwayHelper::roadFirst );
    if( way.isValid() )
    {
      setPathway( way );
      go();
      return;
    }
  }

  Logger::warning( "WARNING !!! CartPusher::_brokePathway not destination point [{},{}]", pos.i(), pos.j() );
  deleteLater();
}

good::Stock& CartPusher::stock() {   return _d->stock;}
void CartPusher::setProducerBuilding(BuildingPtr building){   _d->producerBuilding = building;}
void CartPusher::setConsumerBuilding(BuildingPtr building){   _d->consumerBuilding = building;}

BuildingPtr CartPusher::producerBuilding()
{
   Logger::warningIf( _d->producerBuilding.isNull(), "!!! WARNING: ProducerBuilding is not initialized");
   return _d->producerBuilding;
}

BuildingPtr CartPusher::consumerBuilding()
{
   Logger::warningIf( _d->consumerBuilding.isNull(), "!!! WARNING: ConsumerBuilding is not initialized");
   
   return _d->consumerBuilding;
}

Animation& CartPusher::getCartPicture()
{
   if( !_d->anim.isValid() )
   {
     _d->anim.load(_d->stock, direction());
   }

   return _d->anim;
}

void CartPusher::_changeDirection()
{
   Walker::_changeDirection();
   _d->anim = CartAnimation();  // need to get the new graphic
}

void CartPusher::getPictures( gfx::Pictures& oPics)
{
   oPics.clear();
   Point offset;

   switch( direction() )
   {
   case direction::west: offset = Point( 10, -5 ); break;
   case direction::east: offset = Point( -10, 5 ); break;
   case direction::north: offset = Point( -5, -5 ); break;
   case direction::south: offset = Point( 5, 5 ); break;
   default: break;
   }

   // depending on the walker direction, the cart is ahead or behind
   switch( direction() )
   {
   case direction::west:
   case direction::northWest:
   case direction::north:
   case direction::northEast:
      oPics.push_back( getCartPicture().currentFrame() );
      oPics.push_back( getMainPicture() );
   break;

   case direction::east:
   case direction::southEast:
   case direction::south:
   case direction::southWest:
      oPics.push_back( getMainPicture() );
      oPics.push_back( getCartPicture().currentFrame() );
   break;

   default:
   break;
   }

   if( !oPics.empty() && _d->anim.isBack() )
   {
     std::iter_swap( oPics.begin(), oPics.begin() + 1);
   }

   for( auto& pic : oPics )
     pic.addOffset( offset );
}

void CartPusher::_computeWalkerDestination()
{
   // get the list of buildings within reach
   Pathway pathWay;
   Propagator pathPropagator( _city() );
   pathPropagator.setAllDirections( false );
   _d->consumerBuilding = NULL;

   if( _d->producerBuilding.isNull() )
   {
     Logger::warning( "CartPusher destroyed: producerBuilding can't be NULL" );
     deleteLater();
     return;
   }

   pathPropagator.init( ptr_cast<Construction>(_d->producerBuilding) );
   pathPropagator.propagate(_d->maxDistance);

   BuildingPtr destBuilding;
   //if city save goods, need find warehouse first
   if( _city()->tradeOptions().isStacking(_d->stock.type()) )
   {
      destBuilding = _d->getWalkerDestination_warehouse( pathPropagator, pathWay );
   }

   if(destBuilding == NULL)
   {
      // try send that good to a factory
      destBuilding = _d->getWalkerDestination_factory(pathPropagator, pathWay);
   }

   if(destBuilding == NULL)
   {
      // try send that good to a granary
      destBuilding = _d->getWalkerDestination_granary(pathPropagator, pathWay);
   }

   if(destBuilding == NULL)
   {
      // try send that good to a warehouse
      destBuilding = _d->getWalkerDestination_warehouse( pathPropagator, pathWay );
   }

   if(destBuilding != NULL)
   {
      //_isDeleted = true;  // no destination!
     setConsumerBuilding( destBuilding );
     setPos( pathWay.startPos() );
     setPathway( pathWay );
     go();
   }
   else
   {
     if( _d->producerBuilding->roadside().empty() )
     {
       deleteLater();
     }
     else
     {
       Walker::wait( Walker::infiniteWait );
       setPos( _d->producerBuilding->roadside().front()->pos() );
       _changeDirection();
       turn( _d->producerBuilding->pos() );
       getMainPicture();
     }
   }
}

template< class T >
BuildingPtr reserveShortestPath( const object::Type buildingType,
                                 good::Stock& stock, long& reservationID,
                                 Propagator &pathPropagator, Pathway& oPathWay )
{
  BuildingPtr res;
  DirectPRoutes pathWayList = pathPropagator.getRoutes( buildingType );

  //remove factories with improrer storage
  auto pathWayIt= pathWayList.begin();
  while( pathWayIt != pathWayList.end() )
  {
    // for every factory within range
    SmartPtr<T> building = pathWayIt->first.as<T>();

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
  for( auto& item : pathWayList )
  {
    if( (int)item.second->length() < maxLength )
    {
      shortestPath = item.second;
      maxLength = item.second->length();
      res = item.first.as<Building>();
    }
  }

  if( res.isValid() )
  {
    SmartPtr<T> ptr = res.as<T>();
    reservationID = ptr->store().reserveStorage( stock, game::Date::current() );
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
  object::ProductConsumer info( stock.type() );
  object::Type buildingType = info.consumer();

  if (buildingType == object::unknown)
  {
     // no factory can use this good
     return BuildingPtr();
  }

  res = reserveShortestPath<Factory>( buildingType, stock, reservationID, pathPropagator, oPathWay );

  return res;
}

BuildingPtr CartPusher::Impl::getWalkerDestination_warehouse(Propagator &pathPropagator, Pathway& oPathWay)
{
  BuildingPtr res;

  res = reserveShortestPath<Warehouse>( object::warehouse, stock, reservationID, pathPropagator, oPathWay );

  return res;
}

BuildingPtr CartPusher::Impl::getWalkerDestination_granary(Propagator &pathPropagator, Pathway& oPathWay)
{
   BuildingPtr res;

   good::Product p = stock.type();
   if( !good::foods().contain( p ) )
   {
      // this good cannot be stored in a granary
      return BuildingPtr();
   }

   res = reserveShortestPath<Granary>( object::granery, stock, reservationID, pathPropagator, oPathWay );

   return res;
}

void CartPusher::send2city(BuildingPtr building, good::Stock& carry )
{
  _d->stock.takeFrom( carry );
  setProducerBuilding( building  );

  _computeWalkerDestination();
  attach();
}

void CartPusher::timeStep( const unsigned long time )
{
  _d->anim.update( time );
  if( game::Date::isWeekChanged() && !_pathway().isValid() )
  {
    _computeWalkerDestination();
  }

  Walker::timeStep( time );
}

CartPusher::~CartPusher(){}

void CartPusher::save( VariantMap& stream ) const
{
  Walker::save( stream );
  
  stream[ literals::stock ] = _d->stock.save();
  stream[ literals::producerPos ] = _d->producerBuilding.isValid()
                                ? _d->producerBuilding->pos() : gfx::tilemap::invalidLocation();

  stream[ literals::consumerPos ] = _d->consumerBuilding.isValid()
                                ? _d->consumerBuilding->pos() : gfx::tilemap::invalidLocation();

  VARIANT_SAVE_ANY_D( stream, _d, maxDistance )
  VARIANT_SAVE_ANY_D( stream, _d, cantUnloadGoods )
  VARIANT_SAVE_ENUM_D( stream, _d, reservationID )
}

void CartPusher::load( const VariantMap& stream )
{
  Walker::load( stream );

  _d->stock.load( stream.get( literals::stock ).toList() );

  TilePos prPos( stream.get( literals::producerPos ).toTilePos() );
  _d->producerBuilding = _map().overlay<Building>( prPos );

  if( _d->producerBuilding.is<WorkingBuilding>() )
  {
    auto workingBuilding = _d->producerBuilding.as<WorkingBuilding>();
    workingBuilding->addWalker( this );
  }
  else
  {
    Logger::warning( "WARNING: cartPusher producer building is NULL uid={}", uniqueId() );
  }

  TilePos cnsmPos( stream.get( literals::consumerPos ).toTilePos() );
  _d->consumerBuilding = _map().overlay<Building>( cnsmPos );

  VARIANT_LOAD_ANY_D( _d, maxDistance, stream )
  VARIANT_LOAD_ANY_D( _d, cantUnloadGoods, stream )
  VARIANT_LOAD_ENUM_D( _d, reservationID, stream )
}

bool CartPusher::die()
{
  bool created = Walker::die();

  events::dispatch<RemoveCitizens>( pos(), CitizenGroup( CitizenGroup::mature, 1) );

  if( !created )
  {
    Corpse::create( _city(), pos(), ResourceGroup::citizen1, 1025, 1032 );
    return true;
  }

  return created;
}

std::string CartPusher::thoughts( Thought thougthType )const
{
  switch( thougthType )
  {
  case thCurrent:
    if( !pathway().isValid() )
    {
      return "##cartpusher_cantfind_destination##";
    }
    else
    {
      if( pathway().isReverse() && _d->cantUnloadGoods )
      {
        if( _d->consumerBuilding.is<Factory>() )
        {
          return "##cartpusher_cant_unload_goods_in_factory##";
        }
      }
    }
  break;

  case thAction:

  break;

  default: break;
  }

  return Walker::thoughts( thougthType );
}

TilePos CartPusher::places(Walker::Place type) const
{
  switch( type )
  {
  case plOrigin: return _d->producerBuilding.isValid() ? _d->producerBuilding->pos() : gfx::tilemap::invalidLocation();
  case plDestination: return _d->consumerBuilding.isValid() ? _d->consumerBuilding->pos() : gfx::tilemap::invalidLocation();
  default: break;
  }

  return Human::places( type );
}

