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

#include "granary.hpp"
#include "game/resourcegroup.hpp"
#include "gfx/picture.hpp"
#include "core/variant.hpp"
#include "walker/cart_pusher.hpp"
#include "good/goodstore_simple.hpp"
#include "city/city.hpp"
#include "constants.hpp"
#include "game/gamedate.hpp"
#include "walker/cart_supplier.hpp"

class GranaryGoodStore : public SimpleGoodStore
{
public:
  static const int maxCapacity = 2400;

  GranaryGoodStore()
  {
    for( int type=Good::wheat; type <= Good::vegetable; type++ )
    {
      setOrder( (Good::Type)type, GoodOrders::accept );
    }

    setOrder( Good::fish, GoodOrders::none );
    setCapacity( GranaryGoodStore::maxCapacity );
  }

  // returns the reservationID if stock can be retrieved (else 0)
  virtual long reserveStorage( GoodStock &stock, DateTime time )
  {
    return granary->numberWorkers() > 0
              ? SimpleGoodStore::reserveStorage( stock, time )
              : 0;
  }

  virtual void store( GoodStock &stock, const int amount)
  {
    if( granary->numberWorkers() == 0 )
    {
      return;
    }
    
    SimpleGoodStore::store( stock, amount );
  }

  virtual void applyStorageReservation(GoodStock &stock, const long reservationID)
  {
    SimpleGoodStore::applyStorageReservation( stock, reservationID );

    granary->computePictures();
  }

  virtual void applyRetrieveReservation(GoodStock &stock, const long reservationID)
  {
    SimpleGoodStore::applyRetrieveReservation( stock, reservationID );

    granary->computePictures();
  }
  
  virtual void setOrder( const Good::Type type, const GoodOrders::Order order )
  {
    SimpleGoodStore::setOrder( type, order );
    setCapacity( type, (order == GoodOrders::reject || order == GoodOrders::none ) ? 0 : GranaryGoodStore::maxCapacity );
  }

  Granary* granary;
};

class Granary::Impl
{
public:
  GranaryGoodStore goodStore;
  bool devastateThis;
};

Granary::Granary() : WorkingBuilding( constants::building::granary, Size(3) ), _d( new Impl )
{
  _d->goodStore.granary = this;

  setPicture( ResourceGroup::commerce, 140 );
  _fgPicturesRef().resize(6);  // 1 upper level + 4 windows + animation

  _animationRef().load(ResourceGroup::commerce, 146, 7, Animation::straight);
  // do the animation in reverse
  _animationRef().load(ResourceGroup::commerce, 151, 6, Animation::reverse);
  _animationRef().setDelay( 4 );

  _fgPicturesRef()[0] = Picture::load( ResourceGroup::commerce, 141);
  _fgPicturesRef()[5] = _animationRef().currentFrame();
  computePictures();

  _d->devastateThis = false;  
}

void Granary::timeStep(const unsigned long time)
{
  WorkingBuilding::timeStep( time );
  if( numberWorkers() > 0 )
  {
    _animationRef().update( time );

    _fgPicturesRef()[5] = _animationRef().currentFrame();

    if( time % (GameDate::ticksInMonth() / 2) == 1 )
    {
      if(  getWalkers().empty() )
      {
        if( _d->goodStore.isDevastation() )
        {
          _tryDevastateGranary();
        }
        else
        {
          _resolveDeliverMode();
        }
      }
    }
  }
}

GoodStore& Granary::store() {  return _d->goodStore; }

void Granary::computePictures()
{
  int allQty = _d->goodStore.qty();
  int maxQty = _d->goodStore.capacity();

  for (int n = 0; n < 4; ++n)
  {
    // reset all window pictures
    _fgPicturesRef()[n+1] = Picture::getInvalid();
  }

  if (allQty > 0){ _fgPicturesRef()[1] = Picture::load( ResourceGroup::commerce, 142); }
  if( allQty > maxQty * 0.25) { _fgPicturesRef()[2] = Picture::load( ResourceGroup::commerce, 143); }
  if (allQty > maxQty * 0.5){ _fgPicturesRef()[3] = Picture::load( ResourceGroup::commerce, 144); }
  if (allQty > maxQty * 0.9){ _fgPicturesRef()[4] = Picture::load( ResourceGroup::commerce, 145); }
}

void Granary::save( VariantMap& stream) const
{
   WorkingBuilding::save( stream );

   stream[ "__debug_typeName" ] = Variant( std::string( CAESARIA_STR_EXT(B_GRANARY) ) );
   stream[ "goodStore" ] = _d->goodStore.save();
}

void Granary::load( const VariantMap& stream)
{
   WorkingBuilding::load(stream);

   _d->goodStore.load( stream.get( "goodStore" ).toMap() );

   computePictures();
}

void Granary::_resolveDeliverMode()
{
  if( getWalkers().size() > 0 )
  {
    return;
  }
  //if warehouse in devastation mode need try send cart pusher with goods to other granary/warehouse/factory
  for( int goodType=Good::wheat; goodType <= Good::vegetable; goodType++ )
  {
    Good::Type gType = (Good::Type)goodType;
    GoodOrders::Order order = _d->goodStore.getOrder( gType );
    int goodFreeQty = math::clamp( _d->goodStore.freeQty( gType ), 0, 400 );

    if( GoodOrders::deliver == order && goodFreeQty > 0 )
    {
      CartSupplierPtr walker = CartSupplier::create( _city() );
      walker->send2city( BuildingPtr( this ), gType, goodFreeQty );

      if( !walker->isDeleted() )
      {
        addWalker( walker.object() );
        return;
      }
    }
  }
}

void Granary::_tryDevastateGranary()
{
  //if granary in devastation mode need try send cart pusher with goods to other granary/warehouse/factory
  for( int goodType=Good::wheat; goodType <= Good::vegetable; goodType++ )
  {
    int goodQty = math::clamp( _d->goodStore.qty( (Good::Type)goodType ), 0, 400);

    if( goodQty > 0 )
    {
      GoodStock stock( (Good::Type)goodType, goodQty, goodQty);
      CartPusherPtr walker = CartPusher::create( _city() );
      walker->send2city( BuildingPtr( this ), stock );

      if( !walker->isDeleted() )
      {
        stock.setQty( 0 );
        _d->goodStore.retrieve( stock, goodQty );//setCurrentQty( (GoodType)goodType, goodQtyMax - goodQty );
        addWalker( walker.object() );
        break;
      }
    }
  }   
}
