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

#include "oc3_granary.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_picture.hpp"
#include "oc3_variant.hpp"
#include "oc3_scenario.hpp"
#include "oc3_walker_cart_pusher.hpp"
#include "oc3_goodstore_simple.hpp"

class GranaryGoodStore : public SimpleGoodStore
{
public:
  static const int maxCapacity = 2400;

  GranaryGoodStore()
  {
    for( int type=G_WHEAT; type <= G_VEGETABLE; type++ )
    {
      setOrder( (GoodType)type, GoodOrders::accept );
    }

    setMaxQty( GranaryGoodStore::maxCapacity );
  }
  
  void setOrder( const GoodType type, const GoodOrders::Order order )
  {
    SimpleGoodStore::setOrder( type, order );
    setMaxQty( type, (order == GoodOrders::reject || order == GoodOrders::none ) ? 0 : GranaryGoodStore::maxCapacity );
  }
};

class Granary::Impl
{
public:
  SimpleGoodStore goodStore;
  bool devastateThis;
};

Granary::Granary() : WorkingBuilding( B_GRANARY, Size(3) ), _d( new Impl )
{
  setMaxWorkers(5);
  setWorkers(0);

  setPicture( Picture::load( ResourceGroup::commerce, 140));
  _fgPictures.resize(6);  // 1 upper level + 4 windows + animation

  _getAnimation().load(ResourceGroup::commerce, 146, 7, Animation::straight);
  // do the animation in reverse
  _getAnimation().load(ResourceGroup::commerce, 151, 6, Animation::reverse);
  _getAnimation().setFrameDelay( 4 );

  _fgPictures[0] = &Picture::load( ResourceGroup::commerce, 141);
  _fgPictures[5] = _getAnimation().getCurrentPicture();
  computePictures();

  _d->devastateThis = false;
}

void Granary::timeStep(const unsigned long time)
{
  WorkingBuilding::timeStep( time );
  if( getWorkers() > 0 )
  {
    _getAnimation().update( time );

    _fgPictures[5] = _getAnimation().getCurrentPicture();    

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
    _fgPictures[n+1] = NULL;
  }

  if (allQty > 0)
  {
    _fgPictures[1] = &Picture::load( ResourceGroup::commerce, 142);
  }
  if( allQty > maxQty * 0.25)
  {
    _fgPictures[2] = &Picture::load( ResourceGroup::commerce, 143);
  }
  if (allQty > maxQty * 0.5)
  {
    _fgPictures[3] = &Picture::load( ResourceGroup::commerce, 144);
  }
  if (allQty > maxQty * 0.9)
  {
    _fgPictures[4] = &Picture::load( ResourceGroup::commerce, 145);
  }
}

void Granary::save( VariantMap& stream) const
{
   WorkingBuilding::save( stream );

   VariantMap vm_goodstore;

   stream[ "__debug_typeName" ] = OC3_STR_EXT(B_GRANARY);
   _d->goodStore.save( vm_goodstore );
   stream[ "goodStore" ] = vm_goodstore;
}

void Granary::load( const VariantMap& stream)
{
   WorkingBuilding::load(stream);
   _d->goodStore.load( stream.get( "goodStore" ).toMap() );
}

void Granary::_tryDevastateGranary()
{
  //if granary in devastation mode need try send cart pusher with goods to other granary/warehouse/factory
  for( int goodType=G_WHEAT; goodType <= G_VEGETABLE; goodType++ )
  {
    int goodQtyMax = _d->goodStore.getCurrentQty( (GoodType)goodType );
    int goodQty = math::clamp( goodQty, 0, 400);

    if( goodQty > 0 )
    {
      GoodStock stock( (GoodType)goodType, goodQty, goodQty);
      CartPusherPtr walker = CartPusher::create( Scenario::instance().getCity() );
      walker->send2City( BuildingPtr( this ), stock );

      if( !walker->isDeleted() )
      {
        GoodStock tmpStock( (GoodType)goodType, goodQty );
        _d->goodStore.retrieve( tmpStock, goodQty );//setCurrentQty( (GoodType)goodType, goodQtyMax - goodQty );
        addWalker( walker.as<Walker>() );
        break;
      }
    }
  }   
}