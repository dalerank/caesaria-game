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

#include "oc3_factory_building.hpp"

#include "oc3_tile.hpp"
#include "oc3_scenario.hpp"
#include "oc3_walker_cart_pusher.hpp"
#include "oc3_exception.hpp"
#include "oc3_gui_info_box.hpp"
#include "oc3_gettext.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_predefinitions.hpp"
#include "oc3_variant.hpp"
#include "oc3_walker_cart_supplier.hpp"
#include "oc3_stringhelper.hpp"
#include "oc3_goodstore_simple.hpp"

class Factory::Impl
{
public:
  float productionRate;  // max production / year
  float progress;  // progress of the work, in percent (0-100).
  Picture* stockPicture; // stock of input good
  SimpleGoodStore goodStore;
  GoodType inGoodType;
  GoodType outGoodType;
  bool produceGood;
};

Factory::Factory( const GoodType inType, const GoodType outType,
                  const BuildingType type, const Size& size )
: WorkingBuilding( type, size ), _d( new Impl )
{
   setMaxWorkers(10);
   //setWorkers(8);

   _d->productionRate = 4.8f;
   _d->progress = 0.0f;
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

bool Factory::_mayWork() const
{
  if( getWorkers() == 0 )
    return false;

  GoodStock& inStock = const_cast< Factory* >( this )->getInGood();
  if( inStock._goodType == G_NONE ) 
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
   bool mayAnimate = _mayWork();

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
       _d->goodStore.store( GoodStock( _d->outGoodType, 100, 100 ), 100 );    
     }
   }
   else
   {
     //ok... factory is work, produce goods
     float workersRatio = float(getWorkers()) / float(getMaxWorkers());  // work drops if not enough workers
     // 1080: number of seconds in a year, 0.67: number of timeSteps per second
     float work = 100.f / 1080.f / 0.67f * _d->productionRate * workersRatio * workersRatio;  // work is proportional to time and factory speed
     if( _d->inGoodType != G_NONE && _d->goodStore.getCurrentQty( _d->inGoodType ) == 0 )
     {
       // cannot work, no input material!
       work = 0.0;
     }

     _d->progress += work;

     _getAnimation().update( time );
     Picture *pic = _getAnimation().getCurrentPicture();
     if (pic != NULL)
     {
       // animation of the working factory
       int level = _fgPictures.size()-1;
       _fgPictures[level] = _getAnimation().getCurrentPicture();
     }
   }  

   if( !_d->produceGood )
   {
     if( _d->inGoodType == G_NONE ) //raw material
     {
       _d->produceGood = true;
     }
     else if( _d->goodStore.getCurrentQty( _d->inGoodType ) >= 100 && _d->goodStore.getCurrentQty( _d->outGoodType ) < 100 )
     {
       _d->produceGood = true;
       _d->goodStore.retrieve( GoodStock( _d->inGoodType, 100, 0 ), 100  );
     }     
   }
}

void Factory::deliverGood()
{
  // make a cart pusher and send him away
  if( _mayDeliverGood() && _d->goodStore.getCurrentQty( _d->outGoodType ) >= 100 )
  {      
    GoodStock stock(_d->outGoodType, 100, 0);
    _d->goodStore.retrieve( stock, 100 );

    CartPusherPtr walker = CartPusher::create( Scenario::instance().getCity() );
    walker->send2City( BuildingPtr( this ), stock );

    if( !walker->isDeleted() )
    {
      addWalker( walker.as<Walker>() );
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
  _d->progress = (float)stream.get( "progress" ); // approximation
  _d->productionRate = (float)stream.get( "productionRate" );
}

Factory::~Factory()
{

}

bool Factory::_mayDeliverGood() const
{
  return ( getAccessRoads().size() > 0 ) && ( getWalkerList().size() == 0 );
}

void Factory::_setProductRate( const float rate )
{
  _d->productionRate = rate;
}

GoodType Factory::getOutGoodType() const
{
  return _d->outGoodType;
}

void Factory::receiveGood()
{
  GoodStock& stock = getInGood();

  //send cart supplier if stock not full
  if( _mayDeliverGood() && stock._currentQty < stock._maxQty )
  {
    CartSupplierPtr walker = CartSupplier::create( Scenario::instance().getCity() );
    walker->send2City( this, stock._goodType, stock._maxQty - stock._currentQty );

    if( !walker->isDeleted() )
    {
      addWalker( walker.as<Walker>() );
    }
  }
}

TimberLogger::TimberLogger() : Factory(G_NONE, G_TIMBER, B_TIMBER_YARD, Size(2) )
{
  _setProductRate( 9.6f );
  setPicture( Picture::load(ResourceGroup::commerce, 72) );

  _getAnimation().load( ResourceGroup::commerce, 73, 10);
  _fgPictures.resize(2);
  setWorkers( 0 );
}

bool TimberLogger::canBuild(const TilePos& pos ) const
{
   bool is_constructible = WorkingBuilding::canBuild( pos );
   bool near_forest = false;  // tells if the factory is next to a forest

   Tilemap& tilemap = Scenario::instance().getCity()->getTilemap();
   PtrTilesArea rect = tilemap.getRectangle( pos + TilePos( -1, -1 ), getSize() + Size( 2 ), Tilemap::checkCorners );
   for( PtrTilesArea::iterator itTiles = rect.begin(); itTiles != rect.end(); ++itTiles)
   {
      Tile &tile = **itTiles;
      near_forest |= tile.getTerrain().isTree();
   }

   return (is_constructible && near_forest);
}


IronMine::IronMine() : Factory(G_NONE, G_IRON, B_IRON_MINE, Size(2) )
{
  _setProductRate( 9.6f );
  setWorkers( 0 );

  setPicture( Picture::load(ResourceGroup::commerce, 54) );

  _getAnimation().load( ResourceGroup::commerce, 55, 6 );
  _getAnimation().setFrameDelay( 5 );
  _fgPictures.resize(2);
}

bool IronMine::canBuild(const TilePos& pos ) const
{
  bool is_constructible = WorkingBuilding::canBuild( pos );
  bool near_mountain = false;  // tells if the factory is next to a mountain

  Tilemap& tilemap = Scenario::instance().getCity()->getTilemap();
  PtrTilesArea rect = tilemap.getRectangle( pos + TilePos( -1, -1 ), getSize() + Size(2), Tilemap::checkCorners );
  for( PtrTilesArea::iterator itTiles = rect.begin(); itTiles != rect.end(); ++itTiles)
  {
     near_mountain |= (*itTiles)->getTerrain().isRock();
  }

  return (is_constructible && near_mountain);
}

WeaponsWorkshop::WeaponsWorkshop() : Factory(G_IRON, G_WEAPON, B_WEAPONS_WORKSHOP, Size(2) )
{
  setPicture( Picture::load(ResourceGroup::commerce, 108) );

  _getAnimation().load( ResourceGroup::commerce, 109, 6);
  _fgPictures.resize(2);
}

FactoryFurniture::FactoryFurniture() : Factory(G_TIMBER, G_FURNITURE, B_FURNITURE, Size(2) )
{
  setPicture( Picture::load(ResourceGroup::commerce, 117) );

  _getAnimation().load(ResourceGroup::commerce, 118, 14);
  _fgPictures.resize(2);
}

Winery::Winery() : Factory(G_GRAPE, G_WINE, B_WINE_WORKSHOP, Size(2) )
{
  setPicture( Picture::load(ResourceGroup::commerce, 86) );

  _getAnimation().load(ResourceGroup::commerce, 87, 12);
  _fgPictures.resize(2);
}

FactoryOil::FactoryOil() : Factory(G_OLIVE, G_OIL, B_OIL_WORKSHOP, Size(2) )
{
  setPicture( Picture::load(ResourceGroup::commerce, 99) );

  _getAnimation().load(ResourceGroup::commerce, 100, 8);
  _fgPictures.resize(2);
}

Wharf::Wharf() : Factory(G_NONE, G_FISH, B_WHARF, Size(2))
{
  // transport 52 53 54 55
  setPicture( Picture::load( ResourceGroup::wharf, 52));
}

/* INCORRECT! */
bool Wharf::canBuild(const TilePos& pos ) const
{
  bool is_constructible = Construction::canBuild( pos );

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
   
  Tilemap& tilemap = Scenario::instance().getCity()->getTilemap();
   
  PtrTilesArea rect = tilemap.getRectangle( pos + TilePos( -1, -1 ), getSize() + Size( 2 ), false);
  for( PtrTilesArea::iterator itTiles = rect.begin(); itTiles != rect.end(); ++itTiles)
  {
    Tile &tile = **itTiles;
    std::cout << tile.getI() << " " << tile.getJ() << "  " << pos.getI() << " " << pos.getJ() << std::endl;
      
     // if (tiles.get_terrain().isWater())
      
    int size = getSize().getWidth();
     if (tile.getJ() > (pos.getJ() + size -1) && !tile.getTerrain().isWater()) {  bNorth = false; }
     if (tile.getJ() < pos.getJ() && !tile.getTerrain().isWater())              {  bSouth = false; }
     if (tile.getI() > (pos.getI() + size -1) && !tile.getTerrain().isWater()) {  bEast = false;  }
     if (tile.getI() < pos.getI() && !tile.getTerrain().isWater())              {  bWest = false;  }      
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