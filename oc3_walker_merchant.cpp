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

TilePos getNearestWarehousePos( Propagator &pathPropagator,
                                SimpleGoodStore& basket, const GoodType what, 
                                PathWay &oPathWay, long& reservId )
{
  WarehousePtr res;

  Propagator::ReachedBuldings pathWayList;
  pathPropagator.getReachedBuildings( B_WAREHOUSE, pathWayList);

  int max_qty = 0;

  // select the warehouse with the max quantity of requested goods
  for( Propagator::ReachedBuldings::iterator pathWayIt= pathWayList.begin(); 
    pathWayIt != pathWayList.end(); ++pathWayIt)
  {
    // for every warehouse within range
    BuildingPtr building= pathWayIt->first;
    PathWay& pathWay= pathWayIt->second;

    WarehousePtr destBuilding = building.as< Warehouse >();
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
    // a warehouse has been found!
    // reserve some goods from that warehouse
    int qty = std::min(qty, basket.getMaxQty( what ) - basket.getCurrentQty( what ));
    GoodStock stock( what, qty, qty);
    reservId = res->getGoodStore().reserveRetrieval( stock );
    return res->getTilePos();
  }

  return TilePos(-1, -1);
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

  // try to find the most needed good
  for( int i = G_WHEAT; i < G_MAX; i++ )
  {
    GoodType gtype = (GoodType)i;

    if( basket.getCurrentQty( gtype ) < basket.getMaxQty( gtype ) )
    {
      // try get that good from a warehouse
      destBuildingPos = getNearestWarehousePos( pathPropagator, basket, gtype, pathWay, reservationID );
    }

    if( destBuildingPos.getI() >= 0 )
    {
      // we found a destination!
      wlk->setPathWay(pathWay);
      pathFound = true;
      break;
    }
  }  

  // we have nothing to buy, or cannot find what we need to buy
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
    // put the content of the basket in the market
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
}

void Merchant::load( const VariantMap& stream)
{
  Walker::load( stream );
  _d->destBuildingPos = stream.get( "destBuildPos" ).toTilePos();
  CityHelper helper( _d->city );
  _d->basket.load( stream.get( "basket" ).toMap() );
  _d->maxDistance = stream.get( "maxDistance" ).toInt();
  _d->reservationID = stream.get( "reserationId" ).toInt();
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
