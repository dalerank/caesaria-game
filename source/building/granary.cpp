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

#include "granary.hpp"
#include "game/resourcegroup.hpp"
#include "gfx/picture.hpp"
#include "core/variant.hpp"
#include "walker/cart_pusher.hpp"
#include "game/goodstore_simple.hpp"
#include "game/city.hpp"

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
    setMaxQty( GranaryGoodStore::maxCapacity );
  }

  // returns the reservationID if stock can be retrieved (else 0)
  virtual long reserveStorage(GoodStock &stock)
  {
    return granary->getWorkers() > 0 ? SimpleGoodStore::reserveStorage( stock ) : 0;
  }

  virtual void store( GoodStock &stock, const int amount)
  {
    if( granary->getWorkers() == 0 )
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
    setMaxQty( type, (order == GoodOrders::reject || order == GoodOrders::none ) ? 0 : GranaryGoodStore::maxCapacity );
  }

  Granary* granary;
};

class Granary::Impl
{
public:
  GranaryGoodStore goodStore;
  bool devastateThis;
};

Granary::Granary() : WorkingBuilding( B_GRANARY, Size(3) ), _d( new Impl )
{
  _d->goodStore.granary = this;

  setPicture( Picture::load( ResourceGroup::commerce, 140));
  _getForegroundPictures().resize(6);  // 1 upper level + 4 windows + animation

  _getAnimation().load(ResourceGroup::commerce, 146, 7, Animation::straight);
  // do the animation in reverse
  _getAnimation().load(ResourceGroup::commerce, 151, 6, Animation::reverse);
  _getAnimation().setFrameDelay( 4 );

  _getForegroundPictures().at(0) = Picture::load( ResourceGroup::commerce, 141);
  _getForegroundPictures().at(5) = _getAnimation().getCurrentPicture();
  computePictures();

  _d->devastateThis = false;  
}

void Granary::timeStep(const unsigned long time)
{
  WorkingBuilding::timeStep( time );
  if( getWorkers() > 0 )
  {
    _getAnimation().update( time );

    _getForegroundPictures().at(5) = _getAnimation().getCurrentPicture();

    if( time % 22 == 1 && _d->goodStore.isDevastation() 
        && (_d->goodStore.getCurrentQty() > 0) && getWalkerList().empty() )
    {
      _tryDevastateGranary();
    }
  }
}

GoodStore& Granary::getGoodStore()
{
  return _d->goodStore;
}

void Granary::computePictures()
{
  int allQty = _d->goodStore.getCurrentQty();
  int maxQty = _d->goodStore.getMaxQty();

  for (int n = 0; n < 4; ++n)
  {
    // reset all window pictures
    _getForegroundPictures().at(n+1) = Picture::getInvalid();
  }

  if (allQty > 0)
  {
    _getForegroundPictures().at(1) = Picture::load( ResourceGroup::commerce, 142);
  }
  if( allQty > maxQty * 0.25)
  {
    _getForegroundPictures().at(2) = Picture::load( ResourceGroup::commerce, 143);
  }
  if (allQty > maxQty * 0.5)
  {
    _getForegroundPictures().at(3) = Picture::load( ResourceGroup::commerce, 144);
  }
  if (allQty > maxQty * 0.9)
  {
    _getForegroundPictures().at(4) = Picture::load( ResourceGroup::commerce, 145);
  }
}

void Granary::save( VariantMap& stream) const
{
   WorkingBuilding::save( stream );

   stream[ "__debug_typeName" ] = Variant( std::string( OC3_STR_EXT(B_GRANARY) ) );
   stream[ "goodStore" ] = _d->goodStore.save();
}

void Granary::load( const VariantMap& stream)
{
   WorkingBuilding::load(stream);

   _d->goodStore.load( stream.get( "goodStore" ).toMap() );

   computePictures();
}

void Granary::_tryDevastateGranary()
{
  //if granary in devastation mode need try send cart pusher with goods to other granary/warehouse/factory
  for( int goodType=Good::wheat; goodType <= Good::vegetable; goodType++ )
  {
    int goodQty = math::clamp( goodQty, 0, 400);

    if( goodQty > 0 )
    {
      GoodStock stock( (Good::Type)goodType, goodQty, goodQty);
      CartPusherPtr walker = CartPusher::create( _getCity() );
      walker->send2City( BuildingPtr( this ), stock );

      if( !walker->isDeleted() )
      {
        stock._currentQty = 0;
        _d->goodStore.retrieve( stock, goodQty );//setCurrentQty( (GoodType)goodType, goodQtyMax - goodQty );
        addWalker( walker.as<Walker>() );
        break;
      }
    }
  }   
}
