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
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "granary.hpp"
#include "game/resourcegroup.hpp"
#include "gfx/picture.hpp"
#include "core/variant_map.hpp"
#include "walker/cart_pusher.hpp"
#include "good/storage.hpp"
#include "city/statistic.hpp"
#include "constants.hpp"
#include "game/gamedate.hpp"
#include "walker/cart_supplier.hpp"
#include "objects_factory.hpp"
#include "good/turnover.hpp"
#include "config.hpp"

using namespace gfx;

REGISTER_CLASS_IN_OVERLAYFACTORY(object::granery, Granary)

namespace {
CAESARIA_LITERALCONST(goodStore)
static const Renderer::Pass rpass[2] = { Renderer::overlayAnimation, Renderer::overWalker };
static const Renderer::PassQueue granaryPass = Renderer::PassQueue( rpass, rpass + 1 );
}

class GranaryStore : public good::Storage
{
public:
  static const int maxCapacity = 2400;

  GranaryStore()
  {
    for( auto& gtype : good::foods() )
      setOrder( gtype, good::Orders::accept );

    setOrder( good::fish, good::Orders::none );
    setCapacity( GranaryStore::maxCapacity );

    granary = nullptr;
  }

  // returns the reservationID if stock can be retrieved (else 0)
  virtual int reserveStorage( good::Stock &stock, DateTime time )
  {
    return granary->numberWorkers() > 0
              ? good::Storage::reserveStorage( stock, time )
              : 0;
  }

  virtual void store( good::Stock& stock, const int amount)
  {
    if( granary->numberWorkers() == 0 )
    {
      return;
    }
    
    good::Storage::store( stock, amount );
  }

  virtual bool applyStorageReservation(good::Stock& stock, const int reservationID)
  {
    bool isOk = good::Storage::applyStorageReservation( stock, reservationID );
    _providers().append( stock );
    granary->computePictures();
    return isOk;
  }

  virtual bool applyRetrieveReservation(good::Stock &stock, const int reservationID)
  {
    bool isOk = good::Storage::applyRetrieveReservation( stock, reservationID );
    granary->computePictures();
    return isOk;
  }
  
  virtual void setOrder( const good::Product type, const good::Orders::Order order )
  {
    good::Storage::setOrder( type, order );
    setCapacity( type, (order == good::Orders::reject || order == good::Orders::none ) ? 0 : GranaryStore::maxCapacity );
  }

  virtual TilePos owner() const { return granary ? granary->pos() : gfx::tilemap::invalidLocation(); }

  Granary* granary;
};

class Granary::Impl
{
public:
  GranaryStore store;
  Pictures granarySprite;
  bool devastateThis;
};

Granary::Granary() : WorkingBuilding( object::granery, Size(3) ), _d( new Impl )
{
  _d->store.granary = this;

  _picture().load( ResourceGroup::commerce, 140 );
  _fgPictures().resize(6);  // 1 upper level + 4 windows + animation

  _animation().load(ResourceGroup::commerce, 146, 7, Animation::straight);
  // do the animation in reverse
  _animation().load(ResourceGroup::commerce, 151, 6, Animation::reverse);
  _animation().setDelay( 4 );

  _fgPicture( 0 ) = Picture( ResourceGroup::commerce, 141 );
  _fgPicture( 5 ) = _animation().currentFrame();
  computePictures();

  _d->devastateThis = false;  
  _d->granarySprite.push_back( Picture( ResourceGroup::commerce, 141 ) );
  _d->granarySprite.push_back( Picture::getInvalid() );
}

void Granary::timeStep(const unsigned long time)
{
  WorkingBuilding::timeStep( time );
  if( !(mayWork() && isActive()) )
    return;

  if( game::Date::isWeekChanged() )
  {
    _weekUpdate();
    //animate workers need
    _animation().setDelay( 4 + needWorkers() + math::random(2) );
  }
}

good::Store& Granary::store() {  return _d->store; }

void Granary::initTerrain(Tile& terrain)
{
  WorkingBuilding::initTerrain( terrain );
  //TilePos offset = terrain.pos() - pos();
  //TilePos av[5] = { TilePos( 0, 1 ), TilePos( 1, 1 ), TilePos( 1, 2 ), TilePos( 2, 1 ), TilePos( 1, 0 ) };
  //bool walkable = offset == av[0] || offset == av[1] || offset == av[2] || offset == av[3] || offset == av[4];

  //              (0,2)N
  //         (0,1)Y    (1,2)Y
  //strt(0,0)N    (1,1)Y    (2,2)N
  //         (1,0)Y    (2,1)Y
  //              (2,0)N

  /* bool walkable = (offset.i() % 2 == 1 || offset.j() % 2 == 1); //au: VladRassokhin
  terrain.setFlag( Tile::tlRoad, walkable );
  terrain.setFlag( Tile::tlRock, !walkable ); // el muleta
  */
}

void Granary::computePictures()
{
  int allQty = _d->store.qty();
  int maxQty = _d->store.capacity();

  for (int n = 0; n < 4; ++n)
  {
    // reset all window pictures
    _fgPictures()[n+1] = Picture::getInvalid();
  }

  if (allQty > 0){ _fgPictures()[1] = Picture( ResourceGroup::commerce, 142); }
  if (allQty > maxQty * 0.25) { _fgPictures()[2] = Picture( ResourceGroup::commerce, 143); }
  if (allQty > maxQty * 0.5){ _fgPictures()[3] = Picture( ResourceGroup::commerce, 144); }
  if (allQty > maxQty * 0.9){ _fgPictures()[4] = Picture( ResourceGroup::commerce, 145); }
}

void Granary::save( VariantMap& stream) const
{
   WorkingBuilding::save( stream );

   stream[ "__debug_typeName" ] = Variant( std::string( CAESARIA_STR_EXT(B_GRANARY) ) );
   stream[ literals::goodStore ] = _d->store.save();
}

void Granary::load( const VariantMap& stream)
{
  WorkingBuilding::load(stream);

  _d->store.load( stream.get( literals::goodStore ).toMap() );

  computePictures();
}

bool Granary::isWalkable() const { return false; }

void Granary::destroy()
{
  TilesArray tiles = area();
  for( auto tile : tiles )
  {
    tile->setFlag( Tile::tlRoad, false );
    tile->setFlag( Tile::tlRock, false );
  }
}

std::string Granary::troubleDesc() const
{
  std::string error = WorkingBuilding::troubleDesc();

  if( error.empty() && _d->store.isDevastation() )
  {
    error = "##granary_devastation_mode_text##";
  }

  return error;
}

const Pictures& Granary::pictures(Renderer::Pass pass) const
{
  switch( pass )
  {
  //case Renderer::overWalker: return _d->granarySprite;
  default: break;
  }

  return Building::pictures( pass );
}

void Granary::_updateAnimation(const unsigned long time)
{
  WorkingBuilding::_updateAnimation( time );

  _d->granarySprite[ 1 ] = _fgPicture( 5 );
}

Renderer::PassQueue Granary::passQueue() const { return granaryPass; }

void Granary::_resolveDeliverMode()
{
  //dont send walkers if they on way already
  if( haveWalkers() )
    return;

  //if warehouse in deliver mode some good then we need to send cart for other warehouses
  //and take it
  for( auto& gType : good::foods() )
  {
    good::Orders::Order order = _d->store.getOrder( gType );
    int goodFreeQty = math::clamp( _d->store.freeQty( gType ), 0, 400 );

    if( good::Orders::deliver == order && goodFreeQty > 0 )
    {
      auto supplier = Walker::create<CartSupplier>( _city() );
      supplier->send2city( this, gType, goodFreeQty );

      if( !supplier->isDeleted() )
      {
        addWalker( supplier );
        return;
      }
    }
  }
}

void Granary::_weekUpdate()
{
  if(  walkers().empty() )
  {
    if( _d->store.isDevastation() )
    {
      _tryDevastateGranary();
    }
    else
    {
      _resolveDeliverMode();
    }
  }
}

bool Granary::_trySendGoods(good::Product gtype, int qty )
{
  good::Stock stock( gtype, qty, qty);
  auto deliverer = Walker::create<CartPusher>( _city() );
  deliverer->send2city( this, stock );

  if( !deliverer->isDeleted() )
  {
    stock.setQty( 0 );
    _d->store.retrieve( stock, qty );
    addWalker( deliverer.object() );
    return true;
  }

  return false;
}

void Granary::_tryDevastateGranary()
{
  //if granary in devastation mode need try send cart pusher with goods to other granary/warehouse/factory
  const int maxSentTry = 3;
  for( auto& goodType : good::foods() )
  {
    int trySentQty[maxSentTry] = { 400, 200, 100 };

    int goodQty = _d->store.qty( goodType );
    for( int i=0; i < maxSentTry; ++i )
    {
      if( goodQty >= trySentQty[i] )
      {
        bool goodSended = _trySendGoods( goodType, trySentQty[i] );
        if( goodSended )
          return;
      }
    }
  }   
}
