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
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "dock.hpp"
#include "gfx/helper.hpp"
#include "core/variant_map.hpp"
#include "game/resourcegroup.hpp"
#include "city/helper.hpp"
#include "gfx/tilemap.hpp"
#include "good/goodhelper.hpp"
#include "core/foreach.hpp"
#include "walker/seamerchant.hpp"
#include "core/foreach.hpp"
#include "walker/cart_supplier.hpp"
#include "good/goodstore_simple.hpp"
#include "constants.hpp"
#include "events/event.hpp"
#include "game/gamedate.hpp"
#include "walker/cart_pusher.hpp"
#include "events/fundissue.hpp"
#include "pathway/pathway_helper.hpp"
#include "objects_factory.hpp"

using namespace constants;
using namespace gfx;

REGISTER_CLASS_IN_OVERLAYFACTORY(objects::dock, Dock)

class Dock::Impl
{
public:
  enum { southPic=29, northPic=5, westPic=41, eastPic=17 };

  good::SimpleStore exportGoods;
  good::SimpleStore importGoods;
  good::SimpleStore requestGoods;
  DateTime dateSendGoods;
  std::vector<int> saveTileInfo;
  Direction direction;

  bool isFlatCoast( const Tile& tile ) const;
  Direction getDirection(PlayerCityPtr city, TilePos pos, Size size);
  bool isConstructibleArea( const TilesArray& tiles );
  bool isCoastalArea( const TilesArray& tiles );
  SeaMerchantList getMerchantsOnWait( PlayerCityPtr city, TilePos pos );
  void initStores();
};

Dock::Dock(): WorkingBuilding( objects::dock, Size(3) ), _d( new Impl )
{
  // dock pictures
  // transport 5        animation = 6~16
  // transport 17       animation = 18~28
  // transport 29       animation = 30~40
  // transport 41       animation = 42~51
  setPicture( ResourceGroup::transport, 5);

  _d->initStores();

  _fgPicturesRef().resize(1);
  _animationRef().setDelay( 5 );
  _setClearAnimationOnStop( false );
}

bool Dock::canBuild( const CityAreaInfo& areaInfo ) const
{
  bool is_constructible = true;//Construction::canBuild( city, pos );

  Direction direction = _d->getDirection( areaInfo.city, areaInfo.pos, size() );

  const_cast< Dock* >( this )->_setDirection( direction );

  return (is_constructible && direction != noneDirection );
}

bool Dock::build( const CityAreaInfo& info )
{
  _setDirection( _d->getDirection( info.city, info.pos, size() ) );

  TilesArray area = info.city->tilemap().getArea( info.pos, size() );

  foreach( tile, area ) { _d->saveTileInfo.push_back( tile::encode( *(*tile) ) ); }

  WorkingBuilding::build( info );

  TilePos landingPos = landingTile().pos();
  Pathway way = PathwayHelper::create( landingPos, info.city->borderInfo().boatEntry, PathwayHelper::deepWater );
  if( !way.isValid() )
  {
    _setError( "##inland_lake_has_no_access_to_sea##" );
  }

  return true;
}

void Dock::destroy()
{
  city::Helper helper( _city() );

  TilesArray area = helper.getArea( this );

  int index=0;
  foreach( tile, area ) { tile::decode( *(*tile), _d->saveTileInfo[ index++ ] ); }

  WorkingBuilding::destroy();
}

void Dock::timeStep(const unsigned long time)
{
  if( time % 25 == 0 )
  {
    if( _d->dateSendGoods < game::Date::current() )
    {
      _tryReceiveGoods();
      _tryDeliverGoods();

      _d->dateSendGoods = game::Date::current();
      _d->dateSendGoods.appendWeek();
    }
  }

  WorkingBuilding::timeStep( time );
}

void Dock::save(VariantMap& stream) const
{
  WorkingBuilding::save( stream );

  VARIANT_SAVE_ANY_D( stream, _d, direction );
  VARIANT_SAVE_ANY_D( stream, _d, dateSendGoods );

  stream[ "saved_tile"] = VariantList( _d->saveTileInfo );
  stream[ "exportGoods" ] = _d->exportGoods.save();
  stream[ "importGoods" ] = _d->importGoods.save();
  stream[ "requestGoods" ] = _d->requestGoods.save();
}

void Dock::load(const VariantMap& stream)
{
  Building::load( stream );

  _d->direction = (Direction)stream.get( CAESARIA_STR_EXT(direction), (int)southWest ).toInt();
  _d->saveTileInfo << stream.get( "saved_tile" ).toList();

  Variant tmp = stream.get( "exportGoods" );
  if( tmp.isValid() ) _d->exportGoods.load( tmp.toMap() );

  tmp = stream.get( "importGoods" );
  if( tmp.isValid() ) _d->importGoods.load( tmp.toMap() );

  tmp = stream.get( "requestGoods" );
  if( tmp.isValid() ) _d->requestGoods.load( tmp.toMap() );

  VARIANT_LOAD_TIME_D( _d, dateSendGoods, stream );

  _updatePicture( _d->direction );
}

std::string Dock::workersProblemDesc() const
{
  return WorkingBuildingHelper::productivity2desc( const_cast<Dock*>( this ), isBusy() ? "busy" : "" );
}

bool Dock::isBusy() const
{
  city::Helper helper( _city() );
  SeaMerchantList merchants = helper.find<SeaMerchant>( walker::seaMerchant, landingTile().pos() );

  return !merchants.empty();
}

const Tile& Dock::landingTile() const
{
  Tilemap& tmap = _city()->tilemap();
  TilePos offset( -999, -999 );
  switch( _d->direction )
  {
  case south: offset = TilePos( 0, -1 ); break;
  case west: offset = TilePos( -1, 0 ); break;
  case north: offset = TilePos( 0, 3 ); break;
  case east: offset = TilePos( 3, 0 ); break;

  default: break;
  }

  return tmap.at( pos() + offset );
}

int Dock::queueSize() const
{
  city::Helper helper( _city() );
  TilePos offset( 3, 3 );
  SeaMerchantList merchants = helper.find<SeaMerchant>( walker::seaMerchant,
                                                        pos() - offset, pos() + offset );

  for( SeaMerchantList::iterator it=merchants.begin(); it != merchants.end(); )
  {
    if( !(*it)->isWaitFreeDock() ) { it = merchants.erase( it ); }
    else { ++it; }
  }

  return merchants.size();
}

const Tile& Dock::queueTile() const
{
  TilePos offset( 3, 3 );
  city::Helper helper( _city() );
  TilesArray tiles = helper.getArea( pos() - offset, pos() + offset );

  foreach( it, tiles )
  {
    if( (*it)->getFlag( Tile::tlDeepWater ) )
    {
      bool needMove;
      bool busyTile = helper.isTileBusy<SeaMerchant>( (*it)->pos(), WalkerPtr(), needMove );
      if( !busyTile )
      {
        return *(*it);
      }
    }
  }

  return _city()->tilemap().at( TilePos( -1, -1 ) );
}

void Dock::requestGoods(good::Stock& stock)
{
  int maxRequest = std::min( stock.qty(), _d->requestGoods.getMaxStore( stock.type() ) );
  maxRequest -= _d->exportGoods.qty( stock.type() );

  if( maxRequest > 0 )
  {
    _d->requestGoods.store( stock, maxRequest );
  }
}

int Dock::importingGoods( good::Stock& stock)
{
  const good::Store& cityOrders = _city()->importingGoods();

  //try sell goods
  int traderMaySell = std::min( stock.qty(), cityOrders.capacity( stock.type() ) );
  int dockMayStore = _d->importGoods.freeQty( stock.type() );

  traderMaySell = std::min( traderMaySell, dockMayStore );
  int cost = 0;
  if( traderMaySell > 0 )
  {
    _d->importGoods.store( stock, traderMaySell );

    events::GameEventPtr e = events::FundIssueEvent::import( stock.type(), traderMaySell );
    e->dispatch();

    cost = good::Helper::importPrice( _city(), stock.type(), traderMaySell );
  }

  return cost;
}

void Dock::storeGoods( good::Stock &stock, const int)
{
  _d->exportGoods.store( stock, stock.qty() );
}

int Dock::exportingGoods( good::Stock& stock, int qty )
{
  qty = std::min( qty, _d->exportGoods.getMaxRetrieve( stock.type() ) );
  _d->exportGoods.retrieve( stock, qty );

  int cost = 0;
  if( qty > 0 )
  {
    events::GameEventPtr e = events::FundIssueEvent::exportg( stock.type(), qty );
    e->dispatch();

    cost = good::Helper::exportPrice( _city(), stock.type(), qty );
  }

  return cost;
}

Dock::~Dock(){}

void Dock::_updatePicture(Direction direction)
{
  int index=0;
  Point offset;
  switch( direction )
  {
  case south: index = Impl::southPic; offset = Point( 35, 51 ); break;
  case north: index = Impl::northPic; offset = Point( 107, 61 );break;
  case west:  index = Impl::westPic;  offset = Point( 48, 70 ); break;
  case east:  index = Impl::eastPic;  offset = Point( 62, 36 ); break;

  default: break;
  }

  setPicture( ResourceGroup::transport, index );
  _animationRef().clear();
  _animationRef().load( ResourceGroup::transport, index+1, 10 );

  //now fill in reverse order
  _animationRef().load( ResourceGroup::transport, index+10, 10, Animation::reverse );
  _animationRef().setOffset( offset );
}

void Dock::_setDirection(Direction direction)
{
  _d->direction = direction;
  _updatePicture( direction );
}

bool Dock::Impl::isFlatCoast(const Tile& tile) const
{
  int imgId = tile.originalImgId();
  return (imgId >= 372 && imgId <= 387);
}

Direction Dock::Impl::getDirection(PlayerCityPtr city, TilePos pos, Size size)
{
  Tilemap& tilemap = city->tilemap();

  int s = size.width();
  TilesArray constructibleTiles = tilemap.getArea( pos + TilePos( 0, 1 ), pos + TilePos( s-1, s-1 ) );
  TilesArray coastalTiles = tilemap.getArea( pos, pos + TilePos( s, 0 ) );

  if( isConstructibleArea( constructibleTiles ) && isCoastalArea( coastalTiles ) )
  { return south; }

  constructibleTiles = tilemap.getArea( pos, pos + TilePos( s-1, 1 ) );
  coastalTiles = tilemap.getArea( pos + TilePos( 0, s-1 ), pos + TilePos( s-1, s-1 ) );

  if( isConstructibleArea( constructibleTiles ) && isCoastalArea( coastalTiles ) )
  { return north; }

  constructibleTiles = tilemap.getArea( pos + TilePos( 1, 0 ), pos + TilePos( 2, 2 ) );
  coastalTiles = tilemap.getArea( pos, pos + TilePos( 0, 2 ) );

  if( isConstructibleArea( constructibleTiles ) && isCoastalArea( coastalTiles ) )
  { return west; }

  constructibleTiles = tilemap.getArea( pos, pos + TilePos( 1, 2 ) );
  coastalTiles = tilemap.getArea( pos + TilePos( 2, 0), pos + TilePos( 2, 2 ) );

  if( isConstructibleArea( constructibleTiles ) && isCoastalArea( coastalTiles ) )
  { return east; }

  return noneDirection;
}

bool Dock::Impl::isConstructibleArea(const TilesArray& tiles)
{
  bool ret = true;
  foreach( i, tiles )
  {
    ret &= (*i)->getFlag( Tile::isConstructible );
  }

  return ret;
}

bool Dock::Impl::isCoastalArea(const TilesArray& tiles)
{
  bool ret = true;
  foreach( i, tiles )
  {
    ret &= (*i)->getFlag( Tile::tlWater ) && isFlatCoast( *(*i) );
  }

  return ret;
}

void Dock::Impl::initStores()
{
  importGoods.setCapacity( good::goodCount, 1000 );
  exportGoods.setCapacity( good::goodCount, 1000 );
  requestGoods.setCapacity( good::goodCount, 1000 );

  importGoods.setCapacity( 1000 * good::goodCount.toInt() );
  exportGoods.setCapacity( 1000 * good::goodCount.toInt() );
  requestGoods.setCapacity( 1000 * good::goodCount.toInt() );
}

void Dock::_tryDeliverGoods()
{
  for( good::Product gtype=good::wheat; gtype < good::goodCount; ++gtype )
  {
    if( walkers().size() > 2 )
    {
      return;
    }

    int qty = std::min( _d->importGoods.getMaxRetrieve( gtype ), 400 );

    if( qty > 0 )
    {
      CartPusherPtr walker = CartPusher::create( _city() );
      good::Stock pusherStock( gtype, qty, 0 );
      _d->importGoods.retrieve( pusherStock, qty );
      walker->send2city( BuildingPtr( this ), pusherStock );

      //success to send cartpusher
      if( !walker->isDeleted() )
      {
        if( walker->pathway().isValid() )
        {
          addWalker( walker.object() );
        }
        else
        {
          _d->importGoods.store( pusherStock, qty );
          walker->deleteLater();
        }
      }
      else
      {
        _d->importGoods.store( pusherStock, qty );
      }
    }
  }
}

void Dock::_tryReceiveGoods()
{
  for( good::Product gtype=good::wheat; gtype < good::goodCount; ++gtype )
  {
    if( walkers().size() >= 2 )
    {
      return;
    }

    if( _d->requestGoods.qty( gtype ) > 0 )
    {
      CartSupplierPtr cart = CartSupplier::create( _city() );
      int qty = std::min( 400, _d->requestGoods.getMaxRetrieve( gtype ) );
      cart->send2city( this, gtype, qty );

      if( !cart->isDeleted() )
      {
        addWalker( cart.object() );
        good::Stock tmpStock( gtype, qty, 0 );
        _d->requestGoods.retrieve( tmpStock, qty );
        return;
      }
      else
      {
        _d->requestGoods.setQty( gtype, 0 );
      }
    }
  }
}
