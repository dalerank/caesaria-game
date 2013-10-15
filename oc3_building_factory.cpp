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
//
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com

#include "oc3_building_factory.hpp"

#include "oc3_tile.hpp"
#include "oc3_walker_cart_pusher.hpp"
#include "oc3_exception.hpp"
#include "oc3_gui_info_box.hpp"
#include "oc3_gettext.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_predefinitions.hpp"
#include "oc3_tilemap.hpp"
#include "oc3_variant.hpp"
#include "oc3_walker_cart_supplier.hpp"
#include "oc3_stringhelper.hpp"
#include "oc3_goodstore_simple.hpp"
#include "oc3_city.hpp"
#include "oc3_foreach.hpp"

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
  std::string errorStr;
};

Factory::Factory(const Good::Type inType, const Good::Type outType,
                  const BuildingType type, const Size& size )
: WorkingBuilding( type, size ), _d( new Impl )
{
   _d->productionRate = 2.f;
   _d->progress = 0.0f;
   _d->isActive = true;
   _d->produceGood = false;
   _d->inGoodType = inType;
   _d->outGoodType = outType;
   _d->goodStore.setMaxQty(1000);  // quite unlimited
   _d->goodStore.setMaxQty(_d->inGoodType, 200);
   _d->goodStore.setMaxQty(_d->outGoodType, 200);
}


GoodStock& Factory::getInGood()
{
   return _d->goodStore.getStock(_d->inGoodType);
}

GoodStock& Factory::getOutGood()
{
   return _d->goodStore.getStock(_d->outGoodType);
}

int Factory::getProgress()
{
  return math::clamp<int>( (int)_d->progress, 0, 100 );
}

void Factory::updateProgress(float value)
{
  _d->progress += value;
}

bool Factory::mayWork() const
{
  if( getWorkers() == 0 || !_d->isActive )
    return false;

  GoodStock& inStock = const_cast< Factory* >( this )->getInGood();
  if( inStock.type() == Good::none )
    return true;

  if( inStock._currentQty > 0 || _d->produceGood )
    return true;

  return false;
}

void Factory::timeStep(const unsigned long time)
{
   WorkingBuilding::timeStep(time);

   //try get good from storage building for us
   if( time % 22 == 1 && getWorkers() > 0 && getWalkerList().size() == 0 )
   {
     receiveGood(); 
     deliverGood();      
   }

   //start/stop animation when workers found
   bool mayAnimate = mayWork();

   if( mayAnimate && _getAnimation().isStopped() )
   {
     _getAnimation().start();
   }

   if( !mayAnimate && _getAnimation().isRunning() )
   {
     _getAnimation().stop();
   }

   //no workers or no good in stock... stop animate
   if( !mayAnimate )
   {
     return;
   }  
  
   if( _d->progress >= 100.0 )
   {
     _d->produceGood = false;
     
     if( _d->goodStore.getCurrentQty( _d->outGoodType ) < _d->goodStore.getMaxQty( _d->outGoodType )  )
     {
       _d->progress -= 100.f;
       //gcc fix for temporaly ref object
       GoodStock tmpStock( _d->outGoodType, 100, 100 );
       _d->goodStore.store( tmpStock, 100 );
     }
   }
   else
   {
     //ok... factory is work, produce goods
     float workersRatio = float(getWorkers()) / float(getMaxWorkers());  // work drops if not enough workers
     // 1080: number of seconds in a year, 0.67: number of timeSteps per second
     float work = 100.f / 1080.f / 0.67f * _d->productionRate * workersRatio * workersRatio;  // work is proportional to time and factory speed
     if( _d->produceGood )
     {
       _d->progress += work;

       _getAnimation().update( time );
       const Picture& pic = _getAnimation().getCurrentPicture();
       if( pic.isValid() )
       {
         // animation of the working factory
         int level = getForegroundPictures().size()-1;
         _getForegroundPictures().at(level) = _getAnimation().getCurrentPicture();
       }
     }
   }  

   if( !_d->produceGood )
   {
     if( _d->inGoodType == Good::none ) //raw material
     {
       _d->produceGood = true;
     }
     else if( _d->goodStore.getCurrentQty( _d->inGoodType ) >= 100 && _d->goodStore.getCurrentQty( _d->outGoodType ) < 100 )
     {
       _d->produceGood = true;
       //gcc fix temporaly ref object error
       GoodStock tmpStock( _d->inGoodType, 100, 0 );
       _d->goodStore.retrieve( tmpStock, 100  );
     }     
   }
}

void Factory::deliverGood()
{
  // make a cart pusher and send him away
  int qty = _d->goodStore.getCurrentQty( _d->outGoodType );
  if( _mayDeliverGood() && qty >= 100 )
  {      
    CartPusherPtr walker = CartPusher::create( _getCity() );

    GoodStock pusherStock( _d->outGoodType, qty, 0 ); 
    _d->goodStore.retrieve( pusherStock, math::clamp( qty, 0, 400 ) );

    walker->send2City( BuildingPtr( this ), pusherStock );

    //success to send cartpusher
    if( !walker->isDeleted() )
    {
      addWalker( walker.as<Walker>() );
    }
    else
    {
      _d->goodStore.store( walker->getStock(), walker->getStock()._currentQty );
    }
  }
}

GoodStore& Factory::getGoodStore()
{
   return _d->goodStore;
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
  _d->productionRate = (float)stream.get( "productionRate", 9.6f );
}

Factory::~Factory()
{

}

bool Factory::_mayDeliverGood() const
{
  return ( getAccessRoads().size() > 0 ) && ( getWalkerList().size() == 0 );
}

void Factory::_setError(const std::string& err)
{
  _d->errorStr = err;
}

void Factory::setProductRate( const float rate )
{
  _d->productionRate = rate;
}

std::string Factory::getError() const
{
  return _d->errorStr;
}

Good::Type Factory::getOutGoodType() const
{
  return _d->outGoodType;
}

void Factory::receiveGood()
{
  GoodStock& stock = getInGood();

  //send cart supplier if stock not full
  if( _mayDeliverGood() && stock._currentQty < stock._maxQty )
  {
    CartSupplierPtr walker = CartSupplier::create( _getCity() );
    walker->send2City( this, stock.type(), stock._maxQty - stock._currentQty );

    if( !walker->isDeleted() )
    {
      addWalker( walker.as<Walker>() );
    }
  }
}

bool Factory::isActive() const
{
  return _d->isActive;
}

void Factory::setActive( bool active )
{
  _d->isActive = active;
}

bool Factory::standIdle() const
{
  return !mayWork();
}

TimberLogger::TimberLogger() : Factory(Good::none, Good::timber, B_TIMBER_YARD, Size(2) )
{
  setPicture( ResourceGroup::commerce, 72 );

  _getAnimation().load( ResourceGroup::commerce, 73, 10);
  _getForegroundPictures().resize(2);
}

bool TimberLogger::canBuild( CityPtr city, const TilePos& pos ) const
{
   bool is_constructible = WorkingBuilding::canBuild( city, pos );
   bool near_forest = false;  // tells if the factory is next to a forest

   Tilemap& tilemap = city->getTilemap();
   TilemapArea area = tilemap.getRectangle( pos + TilePos( -1, -1 ), getSize() + Size( 2 ), Tilemap::checkCorners );
   foreach( Tile* tile, area )
   {
      near_forest |= tile->getFlag( Tile::tlTree );
   }

   const_cast< TimberLogger* >( this )->_setError( near_forest ? "" : _("##lumber_mill_need_forest_near##"));

   return (is_constructible && near_forest);
}


IronMine::IronMine() : Factory(Good::none, Good::iron, B_IRON_MINE, Size(2) )
{
  setPicture( ResourceGroup::commerce, 54 );

  _getAnimation().load( ResourceGroup::commerce, 55, 6 );
  _getAnimation().setFrameDelay( 5 );
  _getForegroundPictures().resize(2);
}

bool IronMine::canBuild( CityPtr city, const TilePos& pos ) const
{
  bool is_constructible = WorkingBuilding::canBuild( city, pos );
  bool near_mountain = false;  // tells if the factory is next to a mountain

  Tilemap& tilemap = city->getTilemap();
  TilemapArea perimetr = tilemap.getRectangle( pos + TilePos( -1, -1 ), getSize() + Size(2), Tilemap::checkCorners );
  foreach( Tile* tile, perimetr )
  {
     near_mountain |= tile->getFlag( Tile::tlRock );
  }

  return (is_constructible && near_mountain);
}

WeaponsWorkshop::WeaponsWorkshop() : Factory(Good::iron, Good::weapon, B_WEAPONS_WORKSHOP, Size(2) )
{
  setPicture( ResourceGroup::commerce, 108);

  _getAnimation().load( ResourceGroup::commerce, 109, 6);
  _getForegroundPictures().resize(2);
}

bool WeaponsWorkshop::canBuild(CityPtr city, const TilePos& pos) const
{
  bool ret = Factory::canBuild( city, pos );

  CityHelper helper( city );
  bool haveIronMine = !helper.getBuildings<Building>( B_IRON_MINE ).empty();

  const_cast< WeaponsWorkshop* >( this )->_setError( haveIronMine ? "" : _("##need_iron_for_work##") );
  return ret;
}

bool WorkshopFurniture::canBuild(CityPtr city, const TilePos& pos) const
{
  bool ret = Factory::canBuild( city, pos );

  CityHelper helper( city );
  bool haveTimberLogger = !helper.getBuildings<TimberLogger>( B_TIMBER_YARD ).empty();

  const_cast< WorkshopFurniture* >( this )->_setError( haveTimberLogger ? "" : _("##need_timber_for_work##") );

  return ret;
}

WorkshopFurniture::WorkshopFurniture() : Factory(Good::timber, Good::furniture, B_FURNITURE, Size(2) )
{
  setPicture( ResourceGroup::commerce, 117 );

  _getAnimation().load(ResourceGroup::commerce, 118, 14);
  _getForegroundPictures().resize(2);
}

Winery::Winery() : Factory(Good::grape, Good::wine, B_WINE_WORKSHOP, Size(2) )
{
  setPicture( ResourceGroup::commerce, 86 );

  _getAnimation().load(ResourceGroup::commerce, 87, 12);
  _getForegroundPictures().resize(2);
}

bool Winery::canBuild(CityPtr city, const TilePos& pos) const
{
  bool ret = Factory::canBuild( city, pos );

  CityHelper helper( city );
  bool haveVinegrad = !helper.getBuildings<Building>( B_GRAPE_FARM ).empty();

  const_cast< Winery* >( this )->_setError( haveVinegrad ? "" : _("##need_vinegrad_for_work##") );
  return ret;
}

Creamery::Creamery() : Factory(Good::olive, Good::oil, B_OIL_WORKSHOP, Size(2) )
{
  setPicture( ResourceGroup::commerce, 99 );

  _getAnimation().load(ResourceGroup::commerce, 100, 8);
  _getForegroundPictures().resize(2);
}

bool Creamery::canBuild(CityPtr city, const TilePos& pos) const
{
  bool ret = Factory::canBuild( city, pos );

  CityHelper helper( city );
  bool haveOliveFarm = !helper.getBuildings<Building>( B_OLIVE_FARM ).empty();

  const_cast< Creamery* >( this )->_setError( haveOliveFarm ? "" : _("##need_olive_for_work##") );

  return ret;
}

Wharf::Wharf() : Factory(Good::none, Good::fish, B_WHARF, Size(2))
{
  // transport 52 53 54 55
  setPicture( ResourceGroup::wharf, 52 );
}

/* INCORRECT! */
bool Wharf::canBuild( CityPtr city, const TilePos& pos ) const
{
  bool is_constructible = Construction::canBuild( city, pos );

  // We can build wharf only on straight border of water and land
  //
  //   ?WW? ???? ???? ????
  //   ?XX? WXX? ?XXW ?XX?
  //   ?XX? WXX? ?XXW ?XX?
  //   ???? ???? ???? ?WW?
  //

  bool bNorth = true;
  bool bSouth = true;
  bool bWest  = true;
  bool bEast  = true;
   
  Tilemap& tilemap = city->getTilemap();
   
  TilemapArea perimetr = tilemap.getRectangle( pos + TilePos( -1, -1 ), getSize() + Size( 2 ), false);
  foreach( Tile* tile, perimetr )
  {    
    int size = getSize().getWidth();
     if(tile->getJ() > (pos.getJ() + size -1) && !tile->getFlag( Tile::tlWater )) {  bNorth = false; }
     if(tile->getJ() < pos.getJ() && !tile->getFlag( Tile::tlWater ))              {  bSouth = false; }
     if(tile->getI() > (pos.getI() + size -1) && !tile->getFlag( Tile::tlWater )) {  bEast = false;  }
     if(tile->getI() < pos.getI() && !tile->getFlag( Tile::tlWater ))              {  bWest = false;  }
   }

   return (is_constructible && (bNorth || bSouth || bEast || bWest));
}

/*
bool Wharf::canBuild(const int i, const int j) const
{
  Tilemap& tilemap = Scenario::instance().getCity().getTilemap();
  
  int sum = 0;
  
  // 2 x 2 so sum will be max 1 + 2 + 4 + 8
  for (int k = 0; k < _size; k++)
    for (int l = 0; l < _size; l++)
      sum += tilemap.at(i + k, j + l).get_terrain().isWater() << (k * _size + l);
  
  std::cout << sum << std::endl;
    
  if (sum==3 or sum==5 or 
    //sum==9 or sum==6 or
    sum==10 or sum==12) return true;
  
  return false;
}*/
