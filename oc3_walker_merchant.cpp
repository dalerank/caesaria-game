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

#include "oc3_walker_merchant.hpp"
#include "oc3_goodstore_simple.hpp"
#include "oc3_building_warehouse.hpp"
#include "oc3_path_finding.hpp"
#include "oc3_city.hpp"
#include "oc3_tile.hpp"
#include "oc3_empire.hpp"
#include "oc3_astarpathfinding.hpp"

class Merchant::Impl
{
public:
  TilePos destBuildingPos;  // warehouse
  CityPtr city;
  SimpleGoodStore basket;
  std::string baseCityName;
  long reservationID;
  int maxDistance;

  void computeDestination( WalkerPtr wlk, const TilePos& position );
};

Merchant::Merchant() : _d( new Impl )
{
  _walkerGraphic = WG_HORSE_CARAVAN;
  _walkerType = WT_MERCHANT;
  _d->maxDistance = 60;
  _d->basket.setMaxQty(800);  // this is a big basket!

  _d->basket.setMaxQty(G_WHEAT, 800);
  _d->basket.setMaxQty(G_FRUIT, 800);
  _d->basket.setMaxQty(G_VEGETABLE, 800);
  _d->basket.setMaxQty(G_MEAT, 800);

  _d->basket.setMaxQty(G_POTTERY, 100);
  _d->basket.setMaxQty(G_FURNITURE, 100);
  _d->basket.setMaxQty(G_OIL, 100);
  _d->basket.setMaxQty(G_WINE, 100);
}

Merchant::~Merchant()
{
}

WarehousePtr getAvailableWarehouse( Propagator &pathPropagator,
                                    SimpleGoodStore& basket, const GoodType what, 
                                    PathWay &oPathWay, int minQty, long& reservId )
{
  WarehousePtr ret;

  Propagator::Ways pathWayList;
  pathPropagator.getReachedBuildings( B_WAREHOUSE, pathWayList);

  // select the warehouse with the max quantity of requested goods
  if( what != G_MAX )
  {
    Propagator::Ways::iterator pathWayIt = pathWayList.begin(); 
    while( pathWayIt != pathWayList.end() )
    {
      // for every warehouse within range
      WarehousePtr warehouse= pathWayIt->first.as< Warehouse >();

      int qty = warehouse->getGoodStore().getMaxRetrieve( what );
      if( qty < minQty )
      {
        pathWayList.erase( pathWayIt++ );
      }
      else
      {
        pathWayIt++;
      }
    }
  }

  //have only available warehouses, find nearest of it
  PathWay* retWay = 0;
  int minLength = 999;
  for( Propagator::Ways::iterator pathWayIt= pathWayList.begin(); 
       pathWayIt != pathWayList.end(); ++pathWayIt)
  {
    // for every warehouse within range
    BuildingPtr building= pathWayIt->first;
    PathWay& pathWay= pathWayIt->second;

    WarehousePtr destBuilding = building.as< Warehouse >();
    if( pathWay.getLength() < minLength )
    {
      minLength = pathWay.getLength();
      ret = destBuilding;
      retWay = &pathWay;
    }
  }

  if( ret.isValid() )
  {
    // a warehouse has been found!
    // reserve some goods from that warehouse
    int qty = std::min(qty, basket.getMaxQty( what ) - basket.getCurrentQty( what ));
    GoodStock stock( what, qty, qty);
    oPathWay = *retWay;
    reservId = ret->getGoodStore().reserveRetrieval( stock );
  }

  return ret;
}

void Merchant::Impl::computeDestination( WalkerPtr wlk, const TilePos& position )
{
  destBuildingPos = TilePos( -1, -1 );  // no destination yet

  // get the list of buildings within reach
  PathWay pathWay;
  Propagator pathPropagator( city );
  Tilemap& tmap = city->getTilemap();
  pathPropagator.init( tmap.at( position ) );
  pathPropagator.propagate( maxDistance );
  bool pathFound = false;

  WarehousePtr warehouse;

  // try to find goods for city export 
  for( int i = G_WHEAT; i < G_MAX; i++ )
  {
    GoodType gtype = (GoodType)i;

    int qty =  basket.getCurrentQty( gtype );
    if( qty == 0 && qty < basket.getMaxQty( gtype ) )
    {
      // try get that good from a warehouse
      warehouse = getAvailableWarehouse( pathPropagator, basket, gtype, 
                                         pathWay, qty, reservationID );
    }

    if( warehouse.isValid() )
    {
      // we found a destination!
      wlk->setPathWay(pathWay);
      pathFound = true;
      destBuildingPos = warehouse->getTilePos();
      break;
    }
  }  

  // city have no goods to sell, may be out city want buy some import goods
  if( !pathFound )
  {
    const GoodStore& buyOrders = city->getBuys();

    if( buyOrders.getMaxQty() > 0 )
    {
      for( int i = G_WHEAT; i < G_MAX; i++ )
      {
        GoodType gtype = (GoodType)i;
  
        if( buyOrders.getMaxQty( gtype ) > 0 && basket.getCurrentQty( gtype ) > 0 )
        {
          warehouse = getAvailableWarehouse( pathPropagator, basket, G_MAX, pathWay, 0, reservationID );
          break;
        }
      }
    }

    if( warehouse.isValid() )
    {
      // we found a warehouse!
      wlk->setPathWay(pathWay);
      destBuildingPos = warehouse->getTilePos();
      pathFound = true;
    }
  }

  // we have nothing to buy, or cannot find warehouse
  if( !pathFound )
  {
    pathFound = Pathfinder::getInstance().getPath( position, city->getRoadExit(), pathWay, 
                                                   false, 1 );
    if( pathFound )
    {
      wlk->setPathWay( pathWay );
    }
  }

  if( !pathFound )
  {
    wlk->deleteLater();
    return;
  }

  wlk->setIJ( pathWay.getOrigin().getIJ() );
}

void Merchant::onDestination()
{
  Walker::onDestination();
  if( getIJ() == _d->city->getRoadExit() )
  {
    // walker on exit from city
    deleteLater();

    EmpirePtr empire = _d->city->getEmpire();
    const std::string& ourCityName = _d->city->getName();
    EmpireTradeRoutePtr route = empire->getTradeRoute( ourCityName, _d->baseCityName );
    if( route.isValid() )
    {
      route->addMerchant( ourCityName, _d->basket );
    }

    deleteLater();
  }
  else
  {
    // get goods from destination building
    CityHelper helper( _d->city );
    WarehousePtr warehouse = helper.getBuilding<Warehouse>( _d->destBuildingPos );

    if( warehouse.isValid() )
    {
      // this is a warehouse!
      warehouse->getGoodStore().applyRetrieveReservation(_d->basket, _d->reservationID);

      // take other goods if possible
      for (int n = G_WHEAT; n<G_MAX; ++n)
      {
        // for all types of good (except G_NONE)
        GoodType goodType = (GoodType) n;
        int qty = _d->basket.getLeftQty( goodType );
        if( qty > 0 )
        {
          int qty = std::min( qty, warehouse->getGoodStore().getMaxRetrieve( goodType ) );
          if (qty != 0)
          {
            // std::cout << "extra retrieve qty=" << qty << " basket=" << _basket.getStock(goodType)._currentQty << std::endl;
            GoodStock& stock = _d->basket.getStock( goodType );
            warehouse->getGoodStore().retrieve( stock, qty );

            _d->basket.store( stock, -1 );
          }
        }
      }

      // walker is near the warehouse
      _d->computeDestination( this, getIJ() );
    }
  }
}

void Merchant::send2City( CityPtr city )
{
  _d->city = city;
  _d->computeDestination( this, city->getRoadEntry() );

  if( !isDeleted() )
  {
    _d->city->addWalker( WalkerPtr( this ) );   
  }
}

void Merchant::save( VariantMap& stream ) const
{
  Walker::save( stream );
  stream[ "destBuildPos" ] = _d->destBuildingPos;
  stream[ "basket" ] = _d->basket.save();
  stream[ "maxDistance" ] = _d->maxDistance;
  stream[ "reservationId" ] = static_cast<unsigned int>(_d->reservationID);
  stream[ "baseCity" ] = Variant( _d->baseCityName );
}

void Merchant::load( const VariantMap& stream)
{
  Walker::load( stream );
  _d->destBuildingPos = stream.get( "destBuildPos" ).toTilePos();
  _d->basket.load( stream.get( "basket" ).toMap() );
  _d->maxDistance = stream.get( "maxDistance" ).toInt();
  _d->reservationID = stream.get( "reserationId" ).toInt();
  _d->baseCityName = stream.get( "baseCity" ).toString();
}

WalkerPtr Merchant::create( EmpireMerchantPtr merchant )
{
  Merchant* cityMerchant( new Merchant() );
  cityMerchant->_d->basket.storeAll( merchant->getGoods() );
  cityMerchant->_d->baseCityName = merchant->getBaseCity()->getName();
  
  WalkerPtr ret( cityMerchant );
  ret->drop();

  return ret;
}
