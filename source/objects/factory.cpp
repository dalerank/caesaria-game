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
#include "good/helper.hpp"
#include "walker/cart_pusher.hpp"
#include "core/exception.hpp"
#include "gui/info_box.hpp"
#include "pathway/pathway_helper.hpp"
#include "core/gettext.hpp"
#include "game/resourcegroup.hpp"
#include "core/predefinitions.hpp"
#include "gfx/tilemap.hpp"
#include "core/variant_map.hpp"
#include "walker/cart_supplier.hpp"
#include "core/utils.hpp"
#include "good/storage.hpp"
#include "city/statistic.hpp"
#include "core/foreach.hpp"
#include "constants.hpp"
#include "game/gamedate.hpp"
#include "core/logger.hpp"
#include "objects_factory.hpp"

using namespace gfx;

class FactoryStorage : public good::Storage
{
public:
  FactoryStorage() : factory( NULL ) {}

  virtual int getMaxStore(const good::Product goodType)
  {
    if( !factory || factory->numberWorkers() == 0 )
    {
      return 0;
    }

    return good::Storage::getMaxStore( goodType );
  }

  virtual bool applyStorageReservation( good::Stock& stock, const int reservationID )
  {
    bool isOk = good::Storage::applyStorageReservation( stock, reservationID );
    emit onChangeState();
    return isOk;
  }

  virtual bool applyRetrieveReservation( good::Stock& stock, const int reservationID)
  {
    bool isOk = good::Storage::applyRetrieveReservation( stock, reservationID );
    stock.setInfo( game::Date::current(), factory->pos().hash() );
    emit onChangeState();
    return isOk;
  }

  virtual TilePos owner() const { return factory ? factory->pos() : TilePos::invalid(); }

  Factory* factory;

public signals:
  Signal0<> onChangeState;
};

class Factory::Impl
{
public:
  bool isActive;
  struct
  {
    float rate;  // max production / year
    float progress; // progress of the work, in percent (0-100).
  } production;

  struct {
    good::Info in;
    good::Info out;
  } goods;

  FactoryStorage goodStore;
  unsigned int lowWorkerWeeksNumber;
  unsigned int maxUnworkingWeeks;
  bool produceGood;
  unsigned int finishedQty;
  std::map<int,Picture> stockImages;

public:
  void productReady();
};

Factory::Factory(const good::Product inType, const good::Product outType,
                  const object::Type type, const Size& size )
: WorkingBuilding( type, size ), _d( new Impl )
{
  _d->production.rate = 2.f;
  _d->production.progress = 0.0f;
  _d->isActive = true;
  _d->produceGood = false;
  _d->goods.in = good::Info( inType );
  _d->goods.out = good::Info( outType );
  _d->finishedQty = 100;
  _d->maxUnworkingWeeks = 0;
  _d->lowWorkerWeeksNumber = 0;
  _d->goodStore.factory = this;
  _d->goodStore.setCapacity( 1000 );
  _d->goodStore.setCapacity(_d->goods.in.type(), 200);
  _d->goodStore.setCapacity(_d->goods.out.type(), 100);
  CONNECT( &_d->goodStore, onChangeState, this, Factory::_storeChanged );
}

good::Stock& Factory::inStock() { return _d->goodStore.getStock(_d->goods.in.type()); }
const good::Stock& Factory::inStock() const { return _d->goodStore.getStock(_d->goods.in.type());}
good::Stock& Factory::outStock() { return _d->goodStore.getStock(_d->goods.out.type());}
const good::Stock& Factory::outStock() const { return _d->goodStore.getStock(_d->goods.out.type()); }
const good::Info& Factory::consume() const{  return _d->goods.in; }
int Factory::progress(){ return math::clamp<int>( (int)_d->production.progress, 0, 100 );}
void Factory::updateProgress(float value){  _d->production.progress = math::clamp<float>( _d->production.progress += value, 0.f, 101.f );}

bool Factory::mayWork() const
{
  if( numberWorkers() == 0 || !isActive() )
    return false;

  bool mayContinue = false;
  if( inStock().type() == good::none )
  {
    mayContinue = true;
  }
  else
  {
    mayContinue = ( haveMaterial() || _d->produceGood );
  }

  mayContinue &= (outStock().freeQty() > 0);

  return mayContinue;
}

void Factory::_removeSpoiledGoods()
{
  store().removeExpired( game::Date::current() );
}

void Factory::_weekUpdate(unsigned int time)
{
  if( numberWorkers() > 0 && walkers().size() == 0 )
  {
    receiveGood();
    deliverGood();
  }

  if( (int)game::Date::current().month() % 3 == 1 )
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

    if( math::random( (int)_d->lowWorkerWeeksNumber ) > (int)_d->maxUnworkingWeeks )
    {
      _reachUnworkingTreshold();
    }
  }
}

void Factory::_setConsumeGoodType(int, good::Product product)
{
   _d->goods.in = product;
}

void Factory::_setUnworkingInterval(unsigned int weeks)
{
  _d->maxUnworkingWeeks = weeks;
}

void Factory::_reachUnworkingTreshold() {}

bool Factory::haveMaterial() const {  return (consume().type() != good::none && !inStock().empty()); }

void Factory::timeStep(const unsigned long time)
{
  WorkingBuilding::timeStep(time);

  //try get good from storage building for us
  if( game::Date::isWeekChanged() )
  {
    _weekUpdate( time );
  }

  //no workers or no good in stock... stop animate
  if( !mayWork() )
    return;

  if( _d->production.progress >= 100.0 ) { _productReady();     }
  else                                   { _productProgress();  }

  if( !_d->produceGood )
  {
    int consumeQty = (int)getConsumeQty();
    if( _d->goods.in.type() == good::none ) //raw material
    {
      _d->produceGood = true;
    }
    else if( _d->goodStore.qty( _d->goods.in.type() ) >= consumeQty
            && _d->goodStore.qty( _d->goods.out.type() ) < 100 )
    {
      _d->produceGood = true;
      //gcc fix temporaly ref object error
      good::Stock tmpStock( _d->goods.in.type(), consumeQty, 0 );
      _d->goodStore.retrieve( tmpStock, consumeQty  );
    }
  }
}

void Factory::deliverGood()
{
  // make a cart pusher and send him away
  int qty = _d->goodStore.qty( _d->goods.out.type() );
  if( _mayDeliverGood() && qty >= CartPusher::simpleCart )
  {
    auto cartPusher = Walker::create<CartPusher>( _city() );

    good::Stock pusherStock( _d->goods.out.type(), qty, 0 );
    _d->goodStore.retrieve( pusherStock, math::clamp<int>( qty, 0, CartPusher::megaCart ) );

    cartPusher->send2city( this, pusherStock );

    //success to send cartpusher
    if( !cartPusher->isDeleted() )
    {
      addWalker( cartPusher.object() );
    }
    else
    {
      _d->goodStore.store( cartPusher->stock(), cartPusher->stock().qty() );
    }
  }
}

good::Store& Factory::store() { return _d->goodStore; }

std::string Factory::troubleDesc() const
{
  std::string ret = WorkingBuilding::troubleDesc();

  if( !isActive() )
  {
    ret = fmt::format( "##trade_advisor_blocked_{}_production##", consume().name() );
  }

  if( ret.empty() && !haveMaterial() && consume().type() != good::none )
  {
    ret = fmt::format( "##trouble_need_{}##", consume().name() );
  }

  return ret;
}

void Factory::save( VariantMap& stream ) const
{
  WorkingBuilding::save( stream );
  VARIANT_SAVE_ANY_D( stream, _d, production.rate )
  VARIANT_SAVE_ANY_D( stream, _d, production.progress )
  VARIANT_SAVE_ANY_D( stream, _d, lowWorkerWeeksNumber )
  VARIANT_SAVE_ANY_D( stream, _d, maxUnworkingWeeks )
  VARIANT_SAVE_CLASS_D( stream, _d, goodStore )
}

void Factory::load( const VariantMap& stream)
{
  WorkingBuilding::load( stream );
  VARIANT_LOAD_CLASS_D( _d, goodStore, stream )
  VARIANT_LOAD_ANYDEF_D( _d, production.progress, 0.f, stream )
  VARIANT_LOAD_ANYDEF_D( _d, production.rate, 9.6f, stream )
  VARIANT_LOAD_ANYDEF_D( _d, lowWorkerWeeksNumber, 0, stream )
  VARIANT_LOAD_ANYDEF_D( _d, maxUnworkingWeeks, 0, stream )

  _storeChanged();
}

Factory::~Factory(){}
bool Factory::_mayDeliverGood() const {  return ( !roadside().empty() ) && ( walkers().size() == 0 );}

void Factory::setProductRate( const float rate ){  _d->production.rate = rate;}
float Factory::productRate() const{  return _d->production.rate;}

void Factory::_storeChanged()
{
  _fgPicture(1) = _getSctockImage( inStock().qty() );
}

math::Percent Factory::effciency()      const { return laborAccessPercent() * productivity() / 100; }
unsigned int Factory::getFinishedQty() const { return _d->finishedQty; }
unsigned int Factory::getConsumeQty()  const { return 100; }

std::string Factory::cartStateDesc() const
{
  auto cartPusher = walkers().valueOrEmpty(0).as<CartPusher>();
  if( cartPusher.isValid() )
  {
    if( cartPusher->pathway().isValid() )
    {
      return cartPusher->pathway().isReverse()
               ? "##factory_cart_returning_from_delivery##"
               : "##factory_cart_taking_goods##";
    }
    else
    {
      return "##factory_cart_wait##";
    }
  }

  return "";
}

void Factory::initialize(const object::Info& mdata)
{
  WorkingBuilding::initialize( mdata );

  setProductRate( (float)mdata.getOption( "productRate", 9.6f ) );
  Variant outputProduct = mdata.getOption( "output" );
  if( outputProduct.isValid() )
  {
    good::Product pr = good::Helper::type( outputProduct.toString() );
    if( pr != good::none )
      _d->goods.out = pr;
  }

  VariantMap vStockImages = mdata.getOption( "stock.image" ).toMap();
  for( const auto& item : vStockImages )
  {
    VariantMap stageVm = item.second.toMap();
    int index = stageVm.get( "qty" ).toInt();
    Picture image; VARIANT_LOAD_PICTURE(image, stageVm);
    image.addOffset( stageVm.get("offset"));
    _d->stockImages[ index ] = image;
  }
}

const good::Info& Factory::produce() const{  return _d->goods.out;}

void Factory::receiveGood()
{
  //send cart supplier if stock not full
  if( consume().type() == good::none )
    return;

  unsigned int mayStoreQty = _d->goodStore.getMaxStore( consume().type() );
  mayStoreQty = math::clamp<unsigned int>( mayStoreQty, 0, CartPusher::simpleCart );
  if( _mayDeliverGood() && mayStoreQty > 0 )
  {
    auto cartSupplier = Walker::create<CartSupplier>( _city() );
    cartSupplier->send2city( this, consume().type(), mayStoreQty );

    addWalker( cartSupplier.object() );
  }
}

bool Factory::isActive() const {  return _d->isActive; }
void Factory::setActive( bool active ) {   _d->isActive = active;}
bool Factory::standIdle() const{  return !mayWork(); }

void Factory::_productReady()
{
  _d->produceGood = false;

  if( _d->goodStore.qty( _d->goods.out.type() ) < _d->goodStore.capacity( _d->goods.out.type() )  )
  {
    _d->production.progress -= 100.f;
    unsigned int qty = getFinishedQty();
    //gcc fix for temporaly ref object
    good::Stock tmpStock( _d->goods.out.type(), qty, qty );
    _d->goodStore.store( tmpStock, qty );
  }
}

const Picture& Factory::_getSctockImage(int qty)
{
  if( qty == 0  )
    return Picture::getInvalid();

  qty = math::clamp( (qty / 100) * 100, 100, 1000);
  auto it = _d->stockImages.find(qty);
  if(it != _d->stockImages.end())
    return it->second;

  Logger::warning("WARNING !!! Cant find image for factory stock image {}", qty);
  return Picture::getInvalid();
}

void Factory::_productProgress()
{
  if( _d->produceGood && game::Date::isDayChanged() )
  {
    //ok... factory is work, produce goods
    float productionPerYear = _d->production.rate / 365.f;
    float laborAccessKoeff = laborAccessPercent() / 100.f;
    float dayProgress = productivity() * productionPerYear * laborAccessKoeff;  // work is proportional to time and factory speed

    _d->production.progress += dayProgress;
  }
}
