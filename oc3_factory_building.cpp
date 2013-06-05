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
#include "oc3_stringhelper.hpp"

class Factory::Impl
{
public:
  Walkers pushers;
  float productionRate;  // max production / year
  float progress;  // progress of the work, in percent (0-100).
  Picture* stockPicture; // stock of input good
  SimpleGoodStore goodStore;

  void removeIdlePushers();
};

Factory::Factory( const GoodType inType, const GoodType outType,
                  const BuildingType type, const Size& size )
: WorkingBuilding( type, size ), _d( new Impl )
{
   setMaxWorkers(10);
   setWorkers(8);

   _d->productionRate = 4.8f;
   _d->progress = 0.0f;
   _inGoodType = inType;
   _outGoodType = outType;
   _d->goodStore.setMaxQty(1000);  // quite unlimited
   _d->goodStore.setMaxQty(_inGoodType, 200);
   _d->goodStore.setMaxQty(_outGoodType, 200);
}


GoodStock& Factory::getInGood()
{
   return _d->goodStore.getStock(_inGoodType);
}


GoodStock& Factory::getOutGood()
{
   return _d->goodStore.getStock(_outGoodType);
}


int Factory::getProgress()
{
   return (int) _d->progress;
}

void Factory::Impl::removeIdlePushers()
{
  // release walkers
  std::list<WalkerPtr>::iterator i = pushers.begin();
  while (i != pushers.end())
  {
    if( (*i)->isDeleted() )
      pushers.erase( i++ );
    else
      ++i;
  }
};

void Factory::timeStep(const unsigned long time)
{
   Building::timeStep(time);
  
   GoodStock &inStock = getInGood();

   float workersRatio = float(getWorkers()) / float(getMaxWorkers());  // work drops if not enough workers
   // 1080: number of seconds in a year, 0.67: number of timeSteps per second
   float work = 100.f / 1080.f / 0.67f * _d->productionRate * workersRatio * workersRatio;  // work is proportionnal to time and factory speed
   if (inStock._goodType != G_NONE && inStock._currentQty == 0)
   {
      // cannot work, no input material!
      work = 0.0;
   }

   if( _d->progress > 100.0 )
   {
      if (inStock._goodType != G_NONE)
      {
         // the input good is consumed
         inStock._currentQty -= 100;
      }

      _d->removeIdlePushers();
      deliverGood();      
   }
   else
   {
     _d->progress += work;

     _animation.update( time );
     Picture *pic = _animation.getCurrentPicture();
     if (pic != NULL)
     {
       // animation of the working factory
       int level = _fgPictures.size()-1;
       _fgPictures[level] = _animation.getCurrentPicture();
     }
   }  
}

void Factory::deliverGood()
{
  // make a cart pusher and send him away
  if( _mayDeliverGood() )
  {
    StringHelper::debug( 0xff, "Good is ready!!!" );
    
    GoodStock stock(_outGoodType, 100, 100);
    CartPusherPtr walker = CartPusher::create( Scenario::instance().getCity() );
    walker->send2City( BuildingPtr( this ), stock );
    _d->progress -= 100.f;

    if( !walker->isDeleted() )
      _addWalker( walker.as<Walker>() );
  }
}

void Factory::_addWalker( WalkerPtr walker )
{
  _d->pushers.push_back( walker );
}

SimpleGoodStore& Factory::getGoodStore()
{
   return _d->goodStore;
}

void Factory::save( VariantMap& stream ) const
{
  WorkingBuilding::save( stream );
  VariantMap vm_goodstore;
  _d->goodStore.save( vm_goodstore );

  stream[ "goodStore" ] = vm_goodstore;
  stream[ "progress" ] = _d->progress; 
}

void Factory::load( const VariantMap& stream)
{
//    WorkingBuilding::unserialize(stream);
//    _goodStore.unserialize(stream);
//    _progress = (float)stream.read_int(1, 0, 100); // approximation
}

Factory::~Factory()
{

}

bool Factory::_mayDeliverGood() const
{
  return ( getAccessRoads().size() > 0 ) && ( _d->pushers.size() == 0 );
}

void Factory::removeWalker( WalkerPtr w )
{
  for( Walkers::iterator it=_d->pushers.begin(); it != _d->pushers.end(); it++ )
  {
    if( *it == w )
    {
      _d->pushers.erase( it );  
      return;
    }
  }
}

void Factory::_setProductRate( const float rate )
{
  _d->productionRate = rate;
}

FactoryTimber::FactoryTimber() : Factory(G_NONE, G_TIMBER, B_TIMBER, Size(2) )
{
  _setProductRate( 9.6f );
  _picture = &Picture::load(ResourceGroup::commerce, 72);

  _animation.load( ResourceGroup::commerce, 73, 10);
  _fgPictures.resize(2);
}

bool FactoryTimber::canBuild(const TilePos& pos ) const
{
   bool is_constructible = Construction::canBuild( pos );
   bool near_forest = false;  // tells if the factory is next to a forest

   Tilemap& tilemap = Scenario::instance().getCity().getTilemap();
   PtrTilesArea rect = tilemap.getRectangle( pos + TilePos( -1, -1 ), Size( _size + 2 ), Tilemap::checkCorners );
   for( PtrTilesArea::iterator itTiles = rect.begin(); itTiles != rect.end(); ++itTiles)
   {
      Tile &tile = **itTiles;
      near_forest |= tile.getTerrain().isTree();
   }

   return (is_constructible && near_forest);
}


FactoryIron::FactoryIron() : Factory(G_NONE, G_IRON, B_IRON, Size(2) )
{
   _setProductRate( 9.6f );
   _picture = &Picture::load(ResourceGroup::commerce, 54);

   _animation.load( ResourceGroup::commerce, 55, 6);
   _fgPictures.resize(2);
}

bool FactoryIron::canBuild(const TilePos& pos ) const
{
   bool is_constructible = Construction::canBuild( pos );
   bool near_mountain = false;  // tells if the factory is next to a mountain

   Tilemap& tilemap = Scenario::instance().getCity().getTilemap();
   PtrTilesArea rect = tilemap.getRectangle( pos + TilePos( -1, -1 ), Size( _size + 2), Tilemap::checkCorners );
   for( PtrTilesArea::iterator itTiles = rect.begin(); itTiles != rect.end(); ++itTiles)
   {
      near_mountain |= (*itTiles)->getTerrain().isRock();
   }

   return (is_constructible && near_mountain);
}

FactoryWeapon::FactoryWeapon() : Factory(G_IRON, G_WEAPON, B_WEAPON, Size(2) )
{
  _picture = &Picture::load(ResourceGroup::commerce, 108);

  _animation.load( ResourceGroup::commerce, 109, 6);
  _fgPictures.resize(2);
}

FactoryFurniture::FactoryFurniture() : Factory(G_TIMBER, G_FURNITURE, B_FURNITURE, Size(2) )
{
  _picture = &Picture::load(ResourceGroup::commerce, 117);

  _animation.load(ResourceGroup::commerce, 118, 14);
  _fgPictures.resize(2);
}

FactoryWine::FactoryWine() : Factory(G_GRAPE, G_WINE, B_WINE, Size(2) )
{
  _picture = &Picture::load(ResourceGroup::commerce, 86);

  _animation.load(ResourceGroup::commerce, 87, 12);
  _fgPictures.resize(2);
}

FactoryOil::FactoryOil() : Factory(G_OLIVE, G_OIL, B_OIL, Size(2) )
{
  _picture = &Picture::load(ResourceGroup::commerce, 99);

  _animation.load(ResourceGroup::commerce, 100, 8);
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
   
  Tilemap& tilemap = Scenario::instance().getCity().getTilemap();
   
  PtrTilesArea rect = tilemap.getRectangle( pos + TilePos( -1, -1 ), Size( _size+2 ), false);
  for( PtrTilesArea::iterator itTiles = rect.begin(); itTiles != rect.end(); ++itTiles)
  {
    Tile &tile = **itTiles;
    std::cout << tile.getI() << " " << tile.getJ() << "  " << pos.getI() << " " << pos.getJ() << std::endl;
      
     // if (tiles.get_terrain().isWater())
      
     if (tile.getJ() > (pos.getJ() + _size -1) && !tile.getTerrain().isWater()) {  bNorth = false; }
     if (tile.getJ() < pos.getJ() && !tile.getTerrain().isWater())              {  bSouth = false; }
     if (tile.getI() > (pos.getI() + _size -1) && !tile.getTerrain().isWater()) {  bEast = false;  }
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