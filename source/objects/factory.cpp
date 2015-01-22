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

#include "factory.hpp"

#include "gfx/tile.hpp"
#include "good/goodhelper.hpp"
#include "walker/cart_pusher.hpp"
#include "core/exception.hpp"
#include "gui/info_box.hpp"
#include "core/gettext.hpp"
#include "game/resourcegroup.hpp"
#include "core/predefinitions.hpp"
#include "gfx/tilemap.hpp"
#include "core/variant_map.hpp"
#include "walker/cart_supplier.hpp"
#include "core/utils.hpp"
#include "good/goodstore_simple.hpp"
#include "city/helper.hpp"
#include "core/foreach.hpp"
#include "constants.hpp"
#include "game/gamedate.hpp"
#include "core/logger.hpp"
#include "objects_factory.hpp"

using namespace constants;
using namespace gfx;

REGISTER_CLASS_IN_OVERLAYFACTORY(objects::wine_workshop, Winery)
REGISTER_CLASS_IN_OVERLAYFACTORY(objects::oil_workshop, Creamery)

class FactoryStore : public good::SimpleStore
{
public:
  FactoryStore() : factory( NULL ) {}

  virtual int getMaxStore(const good::Product goodType)
  {
    if( !factory || factory->numberWorkers() == 0 )
    {
      return 0;
    }

    return good::SimpleStore::getMaxStore( goodType );
  }

  virtual void applyStorageReservation( good::Stock &stock, const int reservationID )
  {
    good::SimpleStore::applyStorageReservation( stock, reservationID );
    emit onChangeState();
  }

  virtual void applyRetrieveReservation( good::Stock &stock, const int reservationID)
  {
    good::SimpleStore::applyRetrieveReservation( stock, reservationID );
    emit onChangeState();
  }

  Factory* factory;

public signals:
  Signal0<> onChangeState;
};

class Factory::Impl
{
public:
  bool isActive;
  float productionRate;  // max production / year
  float progress;  // progress of the work, in percent (0-100).
  Picture stockPicture; // stock of input good
  FactoryStore store;
  good::Product inGoodType;
  unsigned int lowWorkerWeeksNumber;
  unsigned int maxUnworkingWeeks;
  good::Product outGoodType;
  bool produceGood;
  unsigned int finishedQty;
};

Factory::Factory(const good::Product inType, const good::Product outType,
                  const TileOverlay::Type type, const Size& size )
: WorkingBuilding( type, size ), _d( new Impl )
{
  _d->productionRate = 2.f;
  _d->progress = 0.0f;
  _d->isActive = true;
  _d->produceGood = false;
  _d->inGoodType = inType;
  _d->outGoodType = outType;
  _d->finishedQty = 100;
  _d->maxUnworkingWeeks = 0;
  _d->lowWorkerWeeksNumber = 0;
  _d->store.factory = this;
  _d->store.setCapacity( 1000 );
  _d->store.setCapacity(_d->inGoodType, 200);
  _d->store.setCapacity(_d->outGoodType, 100);
  CONNECT( &_d->store, onChangeState, this, Factory::_storeChanged );
}

good::Stock& Factory::inStockRef(){   return _d->store.getStock(_d->inGoodType);}
const good::Stock& Factory::inStockRef() const { return _d->store.getStock(_d->inGoodType);}
good::Stock &Factory::outStockRef(){  return _d->store.getStock(_d->outGoodType);}
good::Product Factory::consumeGoodType() const{  return _d->inGoodType; }
int Factory::progress(){  return math::clamp<int>( (int)_d->progress, 0, 100 );}
void Factory::updateProgress(float value){  _d->progress = math::clamp<float>( _d->progress += value, 0.f, 101.f );}

bool Factory::mayWork() const
{
  if( numberWorkers() == 0 || !isActive() )
    return false;

  good::Stock& inStock = const_cast< Factory* >( this )->inStockRef();
  bool mayContinue = false;
  if( inStock.type() == good::none )
  {
    mayContinue = true;
  }
  else
  {
    mayContinue = ( haveMaterial() || _d->produceGood );
  }

  const good::Stock& outStock = const_cast< Factory* >( this )->outStockRef();
  mayContinue &= (outStock.freeQty() > 0);

  return mayContinue;
}

void Factory::_removeSpoiledGoods()
{
  store().removeExpired( game::Date::current() );
}

void Factory::_setUnworkingInterval(unsigned int weeks)
{
  _d->maxUnworkingWeeks = weeks;
}

void Factory::_reachUnworkingTreshold()
{
  collapse();
}

bool Factory::haveMaterial() const {  return (consumeGoodType() != good::none && !inStockRef().empty()); }

void Factory::timeStep(const unsigned long time)
{
  WorkingBuilding::timeStep(time);

  //try get good from storage building for us
  if( game::Date::isWeekChanged() )
  {
    if( numberWorkers() > 0 && walkers().size() == 0 )
    {
      receiveGood();
      deliverGood();      
    }

    if( game::Date::current().month() % 3 == 1 )
    {
      _removeSpoiledGoods();
    }

    if( _d->maxUnworkingWeeks > 0 )
    {
      if( numberWorkers() < maximumWorkers() / 3 )
      {
        _d->lowWorkerWeeksNumber++;
      }
      else
      {
        _d->lowWorkerWeeksNumber = std::max<int>( 0, _d->lowWorkerWeeksNumber-1 );
      }

      if( _d->lowWorkerWeeksNumber > 8 &&  _d->lowWorkerWeeksNumber > (unsigned int)math::random( 42 ) )
      {
        _reachUnworkingTreshold();
      }
    }
  }

  //no workers or no good in stock... stop animate
  if( !mayWork() )
  {
    return;
  }
  
  if( _d->progress >= 100.0 )
  {
    _d->produceGood = false;

    if( _d->store.qty( _d->outGoodType ) < _d->store.capacity( _d->outGoodType )  )
    {
      _d->progress -= 100.f;
      unsigned int qty = getFinishedQty();
      //gcc fix for temporaly ref object
      good::Stock tmpStock( _d->outGoodType, qty, qty );
      _d->store.store( tmpStock, qty );
    }
  }
  else
  {
    if( _d->produceGood && game::Date::isDayChanged() )
    {
      //ok... factory is work, produce goods
      float timeKoeff = _d->productionRate / 365.f;
      float laborAccessKoeff = laborAccessPercent() / 100.f;
      float dayProgress = productivity() * timeKoeff * laborAccessKoeff;  // work is proportional to time and factory speed

      _d->progress += dayProgress;
    }
  }

  if( !_d->produceGood )
  {
    int consumeQty = (int)getConsumeQty();
    if( _d->inGoodType == good::none ) //raw material
    {
      _d->produceGood = true;
    }
    else if( _d->store.qty( _d->inGoodType ) >= consumeQty && _d->store.qty( _d->outGoodType ) < 100 )
    {
      _d->produceGood = true;
      //gcc fix temporaly ref object error
      good::Stock tmpStock( _d->inGoodType, consumeQty, 0 );
      _d->store.retrieve( tmpStock, consumeQty  );
    }
  }
}

void Factory::deliverGood()
{
  // make a cart pusher and send him away
  int qty = _d->store.qty( _d->outGoodType );
  if( _mayDeliverGood() && qty >= 100 )
  {      
    CartPusherPtr walker = CartPusher::create( _city() );

    good::Stock pusherStock( _d->outGoodType, qty, 0 );
    _d->store.retrieve( pusherStock, math::clamp( qty, 0, 400 ) );

    walker->send2city( BuildingPtr( this ), pusherStock );

    //success to send cartpusher
    if( !walker->isDeleted() )
    {
      addWalker( walker.object() );
    }
    else
    {
      _d->store.store( walker->stock(), walker->stock().qty() );
    }
  }
}

good::Store& Factory::store() {   return _d->store; }

std::string Factory::troubleDesc() const
{
  std::string ret = WorkingBuilding::troubleDesc();

  if( !isActive() )
  {
    std::string goodname = good::Helper::getTypeName( consumeGoodType() );
    ret = utils::format( 0xff, "##trade_advisor_blocked_%s_production##", goodname.c_str() );
  }

  if( ret.empty() && !haveMaterial() && consumeGoodType() != good::none )
  {
    std::string goodname = good::Helper::getTypeName( consumeGoodType() );
    ret = utils::format( 0xff, "##trouble_need_%s##", goodname.c_str() );
  }

  return ret;
}

void Factory::save( VariantMap& stream ) const
{
  WorkingBuilding::save( stream );
  VARIANT_SAVE_ANY_D( stream, _d, productionRate )
  VARIANT_SAVE_ANY_D( stream, _d, progress )
  VARIANT_SAVE_ANY_D( stream, _d, lowWorkerWeeksNumber )
  stream[ "goodStore" ] = _d->store.save();
}

void Factory::load( const VariantMap& stream)
{
  WorkingBuilding::load( stream );
  _d->store.load( stream.get( "goodStore" ).toMap() );
  VARIANT_LOAD_ANYDEF_D( _d, progress, 0.f, stream )
  VARIANT_LOAD_ANYDEF_D( _d, productionRate, 9.6f, stream )
  VARIANT_LOAD_ANYDEF_D( _d, lowWorkerWeeksNumber, 0, stream )
  VARIANT_LOAD_ANYDEF_D( _d, lowWorkerWeeksNumber, 0, stream )

  _storeChanged();
}

Factory::~Factory(){}
bool Factory::_mayDeliverGood() const {  return ( getAccessRoads().size() > 0 ) && ( walkers().size() == 0 );}

void Factory::_storeChanged(){}
void Factory::setProductRate( const float rate ){  _d->productionRate = rate;}
float Factory::productRate() const{  return _d->productionRate;}

unsigned int Factory::effciency() const { return laborAccessPercent() * productivity() / 100; }
unsigned int Factory::getFinishedQty() const{  return _d->finishedQty;}
unsigned int Factory::getConsumeQty() const{  return 100;}

std::string Factory::cartStateDesc() const
{
  if( walkers().size() > 0 )
  {
    CartPusherPtr cart = ptr_cast<CartPusher>( walkers().front() );
    if( cart.isValid() )
    {
      if( cart->pathway().isValid() )
      {
        return cart->pathway().isReverse()
                 ? "##factory_cart_returning_from_delivery##"
                 : "##factory_cart_taking_goods##";
      }
      else
      {
        return "##factory_cart_wait##";
      }
    }
  }

  return "";
}

void Factory::initialize(const MetaData& mdata)
{
  WorkingBuilding::initialize( mdata );

  setProductRate( (float)mdata.getOption( "productRate", 9.6f ) );
  Variant outputProduct = mdata.getOption( "output" );
  if( outputProduct.isValid() )
  {
    good::Product pr = good::Helper::getType( outputProduct.toString() );
    if( pr != good::none )
      _d->outGoodType = pr;
  }
}

good::Product Factory::produceGoodType() const{  return _d->outGoodType;}

void Factory::receiveGood()
{
  //send cart supplier if stock not full
  if( consumeGoodType() == good::none )
    return;

  unsigned int qty = _d->store.getMaxStore( consumeGoodType() );
  qty = math::clamp<unsigned int>( qty, 0, 100 );
  if( _mayDeliverGood() && qty > 0 )
  {
    CartSupplierPtr walker = CartSupplier::create( _city() );
    walker->send2city( this, consumeGoodType(), qty );

    if( !walker->isDeleted() )
    {
      addWalker( walker.object() );
    }
  }
}

bool Factory::isActive() const {  return _d->isActive; }
void Factory::setActive( bool active ) {   _d->isActive = active;}
bool Factory::standIdle() const{  return !mayWork(); }

Winery::Winery() : Factory(good::grape, good::wine, objects::wine_workshop, Size(2) )
{
  setPicture( ResourceGroup::commerce, 86 );

  _animationRef().load(ResourceGroup::commerce, 87, 12);
  _animationRef().setDelay( 4 );
  _fgPicturesRef().resize(3);
}

bool Winery::canBuild( const CityAreaInfo& areaInfo ) const
{
  return Factory::canBuild( areaInfo );
}

bool Winery::build( const CityAreaInfo& info )
{
  Factory::build( info );

  city::Helper helper( info.city );
  bool haveVinegrad = !helper.find<Building>( objects::vinard ).empty();

  _setError( haveVinegrad ? "" : "##need_grape##" );

  return true;
}

void Winery::_storeChanged()
{
  _fgPicturesRef()[1] = inStockRef().empty() ? Picture() : Picture::load( ResourceGroup::commerce, 153 );
  _fgPicturesRef()[1].setOffset( 40, -10 );
}

Creamery::Creamery() : Factory(good::olive, good::oil, objects::oil_workshop, Size(2) )
{
  setPicture( ResourceGroup::commerce, 99 );

  _animationRef().load(ResourceGroup::commerce, 100, 8);
  _animationRef().setDelay( 4 );
  _fgPicturesRef().resize( 3 );
}

bool Creamery::canBuild( const CityAreaInfo& areaInof ) const
{
  return Factory::canBuild( areaInof );
}

bool Creamery::build( const CityAreaInfo& info )
{
  Factory::build( info );

  city::Helper helper( info.city );
  bool haveOliveFarm = !helper.find<Building>( objects::olive_farm ).empty();

  _setError( haveOliveFarm ? "" : _("##need_olive_for_work##") );

  return true;
}

void Creamery::_storeChanged()
{
  _fgPicturesRef()[1] = inStockRef().empty() ? Picture() : Picture::load( ResourceGroup::commerce, 154 );
  _fgPicturesRef()[1].setOffset( 40, -5 );
}
