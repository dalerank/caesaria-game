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
#include "core/variant.hpp"
#include "walker/cart_supplier.hpp"
#include "core/stringhelper.hpp"
#include "good/goodstore_simple.hpp"
#include "city/helper.hpp"
#include "core/foreach.hpp"
#include "constants.hpp"
#include "game/gamedate.hpp"

using namespace constants;

class Factory::Impl
{
public:
  bool isActive;
  float productionRate;  // max production / year
  float progress;  // progress of the work, in percent (0-100).
  Picture stockPicture; // stock of input good
  SimpleGoodStore goodStore;
  Good::Type inGoodType;
  Good::Type outGoodType;
  bool produceGood;
  unsigned int finishedQty;
};

Factory::Factory(const Good::Type inType, const Good::Type outType,
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
   _d->goodStore.setCapacity(1000);  // quite unlimited
   _d->goodStore.setCapacity(_d->inGoodType, 200);
   _d->goodStore.setCapacity(_d->outGoodType, 200);
}

GoodStock& Factory::inStockRef(){   return _d->goodStore.getStock(_d->inGoodType);}
const GoodStock& Factory::inStockRef() const { return _d->goodStore.getStock(_d->inGoodType);}
GoodStock& Factory::outStockRef(){  return _d->goodStore.getStock(_d->outGoodType);}
Good::Type Factory::getInGoodType() const{  return _d->inGoodType; }
int Factory::getProgress(){  return math::clamp<int>( (int)_d->progress, 0, 100 );}
void Factory::updateProgress(float value){  _d->progress = math::clamp<float>( _d->progress += value, 0.f, 101.f );}

bool Factory::mayWork() const
{
  if( getWorkersCount() == 0 || !_d->isActive )
    return false;

  GoodStock& inStock = const_cast< Factory* >( this )->inStockRef();
  if( inStock.type() == Good::none )
    return true;

  if( haveMaterial() || _d->produceGood )
    return true;

  return false;
}

bool Factory::haveMaterial() const {  return (getInGoodType() != Good::none && !inStockRef().empty()); }
void Factory::timeStep(const unsigned long time)
{
  WorkingBuilding::timeStep(time);

  //try get good from storage building for us
  if( time % 22 == 1 && getWorkersCount() > 0 && getWalkers().size() == 0 )
  {
    receiveGood();
    deliverGood();

    if( GameDate::current().month() % 3 == 1 )
    {
      getGoodStore().removeExpired( GameDate::current() );
    }
  }

  //start/stop animation when workers found
  bool mayAnimate = mayWork();

  if( mayAnimate && _animationRef().isStopped() )
  {
    _animationRef().start();
  }

  if( !mayAnimate && _animationRef().isRunning() )
  {
    _animationRef().stop();
  }

  //no workers or no good in stock... stop animate
  if( !mayAnimate )
  {
    return;
  }
  
  if( _d->progress >= 100.0 )
  {
    _d->produceGood = false;

    if( _d->goodStore.getQty( _d->outGoodType ) < _d->goodStore.capacity( _d->outGoodType )  )
    {
      _d->progress -= 100.f;
      unsigned int qty = getFinishedQty();
      //gcc fix for temporaly ref object
      GoodStock tmpStock( _d->outGoodType, qty, qty );
      _d->goodStore.store( tmpStock, qty );
    }
  }
  else
  {
    //ok... factory is work, produce goods
    float workersRatio = (float)getWorkersCount() / (float)getMaxWorkers();  // work drops if not enough workers
    float timeKoeff = DateTime::monthInYear / (float)GameDate::ticksInMonth();
    float work = timeKoeff * _d->productionRate * workersRatio;  // work is proportional to time and factory speed
    if( _d->produceGood )
    {
      _d->progress += work;

      _animationRef().update( time );
      const Picture& pic = _animationRef().getFrame();
      if( pic.isValid() )
      {
        // animation of the working factory
        int level = _fgPicturesRef().size()-1;
        _fgPicturesRef()[level] = _animationRef().getFrame();
      }
    }
  }

  if( !_d->produceGood )
  {
    unsigned int consumeQty = getConsumeQty();
    if( _d->inGoodType == Good::none ) //raw material
    {
      _d->produceGood = true;
    }
    else if( _d->goodStore.getQty( _d->inGoodType ) >= consumeQty && _d->goodStore.getQty( _d->outGoodType ) < 100 )
    {
      _d->produceGood = true;
      //gcc fix temporaly ref object error
      GoodStock tmpStock( _d->inGoodType, consumeQty, 0 );
      _d->goodStore.retrieve( tmpStock, consumeQty  );
    }
  }
}

void Factory::deliverGood()
{
  // make a cart pusher and send him away
  int qty = _d->goodStore.getQty( _d->outGoodType );
  if( _mayDeliverGood() && qty >= 100 )
  {      
    CartPusherPtr walker = CartPusher::create( _getCity() );

    GoodStock pusherStock( _d->outGoodType, qty, 0 ); 
    _d->goodStore.retrieve( pusherStock, math::clamp( qty, 0, 400 ) );

    walker->send2city( BuildingPtr( this ), pusherStock );

    //success to send cartpusher
    if( !walker->isDeleted() )
    {
      addWalker( walker.object() );
    }
    else
    {
      _d->goodStore.store( walker->getStock(), walker->getStock().qty() );
    }
  }
}

GoodStore& Factory::getGoodStore() {   return _d->goodStore; }

std::string Factory::getTrouble() const
{
  std::string ret = WorkingBuilding::getTrouble();

  if( ret.empty() && !haveMaterial() )
  {
    std::string goodname = GoodHelper::getTypeName( getInGoodType() );
    ret = StringHelper::format( 0xff, "##trouble_need_%s##", goodname.c_str() );
  }

  return ret;
}

void Factory::save( VariantMap& stream ) const
{
  WorkingBuilding::save( stream );
  stream[ "productionRate" ] = _d->productionRate;
  stream[ "goodStore" ] = _d->goodStore.save();
  stream[ "progress" ] = _d->progress; 
}

void Factory::load( const VariantMap& stream)
{
  WorkingBuilding::load( stream );
  _d->goodStore.load( stream.get( "goodStore" ).toMap() );
  _d->progress = (float)stream.get( "progress", 0.f ); // approximation

  Variant value = stream.get( "productionRate", 9.6f );
  if( value.isValid() )
  {
    _d->productionRate = (float)value;
  }
}

Factory::~Factory(){}
bool Factory::_mayDeliverGood() const {  return ( getAccessRoads().size() > 0 ) && ( getWalkers().size() == 0 );}
void Factory::setProductRate( const float rate ){  _d->productionRate = rate;}
float Factory::getProductRate() const{  return _d->productionRate;}
unsigned int Factory::getFinishedQty() const{  return _d->finishedQty;}
unsigned int Factory::getConsumeQty() const{  return 100;}
Good::Type Factory::getOutGoodType() const{  return _d->outGoodType;}

void Factory::receiveGood()
{
  //send cart supplier if stock not full
  unsigned int qty = _d->goodStore.getMaxStore( getInGoodType() );
  qty = math::clamp<unsigned int>( qty, 0, 100 );
  if( _mayDeliverGood() && qty > 0 )
  {
    CartSupplierPtr walker = CartSupplier::create( _getCity() );
    walker->send2city( this, getInGoodType(), qty );

    if( !walker->isDeleted() )
    {
      addWalker( walker.object() );
    }
  }
}

bool Factory::isActive() const {  return _d->isActive; }
void Factory::setActive( bool active ) {   _d->isActive = active;}
bool Factory::standIdle() const{  return !mayWork(); }

TimberLogger::TimberLogger() : Factory(Good::none, Good::timber, building::timberLogger, Size(2) )
{
  setPicture( ResourceGroup::commerce, 72 );

  _animationRef().load( ResourceGroup::commerce, 73, 10);
  _fgPicturesRef().resize(2);
}

bool TimberLogger::canBuild( PlayerCityPtr city, TilePos pos, const TilesArray& aroundTiles ) const
{
   bool is_constructible = WorkingBuilding::canBuild( city, pos, aroundTiles );
   bool near_forest = false;  // tells if the factory is next to a forest

   Tilemap& tilemap = city->getTilemap();
   TilesArray area = tilemap.getRectangle( pos + TilePos( -1, -1 ), getSize() + Size( 2 ), Tilemap::checkCorners );
   foreach( tile, area )
   {
     near_forest |= (*tile)->getFlag( Tile::tlTree );
   }

   const_cast< TimberLogger* >( this )->_setError( near_forest ? "" : _("##lumber_mill_need_forest_near##"));

   return (is_constructible && near_forest);
}


IronMine::IronMine() : Factory(Good::none, Good::iron, building::ironMine, Size(2) )
{
  setPicture( ResourceGroup::commerce, 54 );

  _animationRef().load( ResourceGroup::commerce, 55, 6 );
  _animationRef().setDelay( 5 );
  _fgPicturesRef().resize(2);
}

bool IronMine::canBuild( PlayerCityPtr city, TilePos pos, const TilesArray& aroundTiles ) const
{
  bool is_constructible = WorkingBuilding::canBuild( city, pos, aroundTiles );
  bool near_mountain = false;  // tells if the factory is next to a mountain

  Tilemap& tilemap = city->getTilemap();
  TilesArray perimetr = tilemap.getRectangle( pos + TilePos( -1, -1 ), pos + TilePos(3, 3), Tilemap::checkCorners );

  foreach( it, perimetr ) { near_mountain |= (*it)->getFlag( Tile::tlRock ); }

  const_cast< IronMine* >( this )->_setError( near_mountain ? "" : _("##iron_mine_need_mountain_near##"));

  return (is_constructible && near_mountain);
}

WeaponsWorkshop::WeaponsWorkshop() : Factory(Good::iron, Good::weapon, building::weaponsWorkshop, Size(2) )
{
  setPicture( ResourceGroup::commerce, 108);

  _animationRef().load( ResourceGroup::commerce, 109, 6);
  _fgPicturesRef().resize(2);
}

bool WeaponsWorkshop::canBuild( PlayerCityPtr city, TilePos pos, const TilesArray& aroundTiles ) const
{
  return Factory::canBuild( city, pos, aroundTiles );
}

void WeaponsWorkshop::build(PlayerCityPtr city, const TilePos& pos)
{
  Factory::build( city, pos );

  CityHelper helper( city );
  bool haveIronMine = !helper.find<Building>( building::ironMine ).empty();

  _setError( haveIronMine ? "" : "##need_iron_for_work##" );
}

bool FurnitureWorkshop::canBuild(PlayerCityPtr city, TilePos pos, const TilesArray& aroundTiles) const
{
  return Factory::canBuild( city, pos, aroundTiles );
}

void FurnitureWorkshop::build(PlayerCityPtr city, const TilePos& pos)
{
  Factory::build( city, pos );

  CityHelper helper( city );
  bool haveTimberLogger = !helper.find<TimberLogger>( building::timberLogger ).empty();

  _setError( haveTimberLogger ? "" : _("##need_timber_for_work##") );
}

FurnitureWorkshop::FurnitureWorkshop() : Factory(Good::timber, Good::furniture, building::furnitureWorkshop, Size(2) )
{
  setPicture( ResourceGroup::commerce, 117 );

  _animationRef().load(ResourceGroup::commerce, 118, 14);
  _animationRef().setDelay( 4 );
  _fgPicturesRef().resize(2);
}

Winery::Winery() : Factory(Good::grape, Good::wine, building::winery, Size(2) )
{
  setPicture( ResourceGroup::commerce, 86 );

  _animationRef().load(ResourceGroup::commerce, 87, 12);
  _fgPicturesRef().resize(2);
}

bool Winery::canBuild(PlayerCityPtr city, TilePos pos, const TilesArray& aroundTiles) const
{
  return Factory::canBuild( city, pos, aroundTiles );
}

void Winery::build(PlayerCityPtr city, const TilePos& pos)
{
  Factory::build( city, pos );

  CityHelper helper( city );
  bool haveVinegrad = !helper.find<Building>( building::grapeFarm ).empty();

  const_cast< Winery* >( this )->_setError( haveVinegrad ? "" : "##need_grape##" );
}

Creamery::Creamery() : Factory(Good::olive, Good::oil, building::creamery, Size(2) )
{
  setPicture( ResourceGroup::commerce, 99 );

  _animationRef().load(ResourceGroup::commerce, 100, 8);
  _fgPicturesRef().resize(2);
}

bool Creamery::canBuild(PlayerCityPtr city, TilePos pos, const TilesArray& aroundTiles) const
{
  return Factory::canBuild( city, pos, aroundTiles );
}

void Creamery::build(PlayerCityPtr city, const TilePos& pos)
{
  Factory::build( city, pos );

  CityHelper helper( city );
  bool haveOliveFarm = !helper.find<Building>( building::oliveFarm ).empty();

  _setError( haveOliveFarm ? "" : _("##need_olive_for_work##") );
}
