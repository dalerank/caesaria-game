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



#include "factory_building.hpp"

#include <iostream>

#include "scenario.hpp"
#include "walker_cart_pusher.hpp"
#include "exception.hpp"
#include "gui_info_box.hpp"
#include "gettext.hpp"


Factory::Factory(const GoodType inType, const GoodType outType)
{
   setMaxWorkers(10);
   setWorkers(8);

   _productionRate = 4.8;
   _progress = 0.0;
   _inGoodType = inType;
   _outGoodType = outType;
   _goodStore.setMaxQty(1000);  // quite unlimited
   _goodStore.setMaxQty(_inGoodType, 200);
   _goodStore.setMaxQty(_outGoodType, 200);

   _animIndex = 0;
}


GoodStock& Factory::getInGood()
{
   return _goodStore.getStock(_inGoodType);
}


GoodStock& Factory::getOutGood()
{
   return _goodStore.getStock(_outGoodType);
}


int Factory::getProgress()
{
   return (int) _progress;
}


std::map<GoodType, Factory*> Factory::_specimen;

std::map<GoodType, Factory*>& Factory::getSpecimen()
{
   if (_specimen.empty())
   {
      _specimen[G_TIMBER]    = new FactoryTimber();
      _specimen[G_FURNITURE] = new FactoryFurniture();
      _specimen[G_IRON]      = new FactoryIron();
      _specimen[G_WEAPON]    = new FactoryWeapon();
      _specimen[G_WINE]      = new FactoryWine();
      _specimen[G_OIL]       = new FactoryOil();
      _specimen[G_CLAY]      = new FactoryClay();
      _specimen[G_POTTERY]   = new FactoryPottery();
      _specimen[G_MARBLE]    = new FactoryMarble();
      // ????
      _specimen[G_FISH]      = new Wharf();
   }

   return _specimen;
}


void Factory::timeStep(const unsigned long time)
{
   Building::timeStep(time);

   GoodStock &inStock = getInGood();

   float workersRatio = float(getWorkers()) / float(getMaxWorkers());  // work drops if not enough workers
   // 1080: number of seconds in a year, 0.67: number of timeSteps per second
   float work = 100. / 1080. / 0.67 * _productionRate * workersRatio * workersRatio;  // work is proportional to time and factory speed
   if (inStock._goodType != G_NONE && inStock._currentQty == 0)
   {
      // cannot work, no input material!
      work = 0.0;
   }

   _progress += work;

   if (_progress > 100.0)
   {
      if (inStock._goodType != G_NONE)
      {
         // the input good is consumed
         inStock._currentQty -= 100;
      }
      deliverGood();
      _progress -= 100.0;
   }

   if ((time&1) == 0)
   {
      _animation.nextFrame();
      Picture *pic = _animation.get_current_picture();
      if (pic != NULL)
      {
         // animation of the working factory
         int level = _fgPictures.size()-1;
         _fgPictures[level] = _animation.get_current_picture();
      }

      // if (inStock._goodType != G_NONE && inStock._currentQty >= 0)
      // {
      //    _fgPictures[0] = _stockPicture;
      // }
   }
}


void Factory::deliverGood()
{
   // std::cout << "Factory delivery" << std::endl;

   // make a cart pusher and send him away
   GoodStock stock(_outGoodType, 100, 100);
   CartPusher* walker = new CartPusher();
   walker->setStock(stock);
   walker->setProducerBuilding(*this);
   walker->start();
   Scenario::instance().getCity().getWalkerList().push_back(walker);
}


SimpleGoodStore& Factory::getGoodStore()
{
   return _goodStore;
}


GuiInfoBox* Factory::makeInfoBox()
{
   GuiInfoFactory* box = new GuiInfoFactory(*this);
   return box;
}

void Factory::serialize(OutputSerialStream &stream)
{
   WorkingBuilding::serialize(stream);
   _goodStore.serialize(stream);
   stream.write_int((int)_progress, 1, 0, 100); // approximation
}

void Factory::unserialize(InputSerialStream &stream)
{
   WorkingBuilding::unserialize(stream);
   _goodStore.unserialize(stream);
   _progress = stream.read_int(1, 0, 100); // approximation
}


FactoryMarble::FactoryMarble() : Factory(G_NONE, G_MARBLE)
{
   setType(B_MARBLE);
   _size = 2;
   _productionRate = 9.6;
   _picture = &PicLoader::instance().get_picture("commerce", 43);

   AnimLoader animLoader(PicLoader::instance());
   animLoader.fill_animation(_animation, "commerce", 44, 10);
   _fgPictures.resize(2);
}

FactoryMarble* FactoryMarble::clone() const
{
   return new FactoryMarble(*this);
}

bool FactoryMarble::canBuild(const int i, const int j) const
{
   bool is_constructible = Construction::canBuild(i, j);
   bool near_mountain = false;  // tells if the factory is next to a mountain

   Tilemap& tilemap = Scenario::instance().getCity().getTilemap();
   std::list<Tile*> rect = tilemap.getRectangle(i-1, j-1, i+_size, j+_size, false);
   for (std::list<Tile*>::iterator itTiles = rect.begin(); itTiles != rect.end(); ++itTiles)
   {
      Tile &tile = **itTiles;
      near_mountain |= tile.get_terrain().isRock();
   }

   return (is_constructible && near_mountain);
}


FactoryTimber::FactoryTimber() : Factory(G_NONE, G_TIMBER)
{
   setType(B_TIMBER);
   _size = 2;
   _productionRate = 9.6;
   _picture = &PicLoader::instance().get_picture("commerce", 72);

   AnimLoader animLoader(PicLoader::instance());
   animLoader.fill_animation(_animation, "commerce", 73, 10);
   _fgPictures.resize(2);
}

FactoryTimber* FactoryTimber::clone() const
{
   return new FactoryTimber(*this);
}

bool FactoryTimber::canBuild(const int i, const int j) const
{
   bool is_constructible = Construction::canBuild(i, j);
   bool near_forest = false;  // tells if the factory is next to a forest

   Tilemap& tilemap = Scenario::instance().getCity().getTilemap();
   std::list<Tile*> rect = tilemap.getRectangle(i-1, j-1, i+_size, j+_size, false);
   for (std::list<Tile*>::iterator itTiles = rect.begin(); itTiles != rect.end(); ++itTiles)
   {
      Tile &tile = **itTiles;
      near_forest |= tile.get_terrain().isTree();
   }

   return (is_constructible && near_forest);
}


FactoryIron::FactoryIron() : Factory(G_NONE, G_IRON)
{
   setType(B_IRON);
   _size = 2;
   _productionRate = 9.6;
   _picture = &PicLoader::instance().get_picture("commerce", 54);

   AnimLoader animLoader(PicLoader::instance());
   animLoader.fill_animation(_animation, "commerce", 55, 6);
   _fgPictures.resize(2);
}

FactoryIron* FactoryIron::clone() const
{
   return new FactoryIron(*this);
}

bool FactoryIron::canBuild(const int i, const int j) const
{
   bool is_constructible = Construction::canBuild(i, j);
   bool near_mountain = false;  // tells if the factory is next to a mountain

   Tilemap& tilemap = Scenario::instance().getCity().getTilemap();
   std::list<Tile*> rect = tilemap.getRectangle(i-1, j-1, i+_size, j+_size, false);
   for (std::list<Tile*>::iterator itTiles = rect.begin(); itTiles != rect.end(); ++itTiles)
   {
      Tile &tile = **itTiles;
      near_mountain |= tile.get_terrain().isRock();
   }

   return (is_constructible && near_mountain);
}


FactoryClay::FactoryClay() : Factory(G_NONE, G_CLAY)
{
   setType(B_CLAY);
   _size = 2;
   _productionRate = 9.6;
   _picture = &PicLoader::instance().get_picture("commerce", 61);

   AnimLoader animLoader(PicLoader::instance());
   animLoader.fill_animation(_animation, "commerce", 62, 10);
   _fgPictures.resize(2);
}

FactoryClay* FactoryClay::clone() const
{
   return new FactoryClay(*this);
}

bool FactoryClay::canBuild(const int i, const int j) const
{
   bool is_constructible = Construction::canBuild(i, j);
   bool near_water = true;

   // Tilemap& tilemap = Scenario::instance().getCity().getTilemap();
   // std::list<Tile*> rect = tilemap.getRectangle(i-1, j-1, i+_size, j+_size, false);
   // for (std::list<Tile*>::iterator itTiles = rect.begin(); itTiles != rect.end(); ++itTiles)
   // {
   //    Tile &tile = **itTiles;
   //    near_mountain |= tile.get_terrain().isRock();
   // }

   return (is_constructible && near_water);
}


FactoryWeapon::FactoryWeapon() : Factory(G_IRON, G_WEAPON)
{
   setType(B_WEAPON);
   _size = 2;
   _picture = &PicLoader::instance().get_picture("commerce", 108);

   AnimLoader animLoader(PicLoader::instance());
   animLoader.fill_animation(_animation, "commerce", 109, 6);
   _fgPictures.resize(2);
}

FactoryWeapon* FactoryWeapon::clone() const
{
   return new FactoryWeapon(*this);
}


FactoryFurniture::FactoryFurniture() : Factory(G_TIMBER, G_FURNITURE)
{
   setType(B_FURNITURE);
   _size = 2;
   _picture = &PicLoader::instance().get_picture("commerce", 117);

   AnimLoader animLoader(PicLoader::instance());
   animLoader.fill_animation(_animation, "commerce", 118, 14);
   _fgPictures.resize(2);
}

FactoryFurniture* FactoryFurniture::clone() const
{
   return new FactoryFurniture(*this);
}


FactoryWine::FactoryWine() : Factory(G_GRAPE, G_WINE)
{
   setType(B_WINE);
   _size = 2;
   _picture = &PicLoader::instance().get_picture("commerce", 86);

   AnimLoader animLoader(PicLoader::instance());
   animLoader.fill_animation(_animation, "commerce", 87, 12);
   _fgPictures.resize(2);
}

FactoryWine* FactoryWine::clone() const
{
   return new FactoryWine(*this);
}


FactoryOil::FactoryOil() : Factory(G_OLIVE, G_OIL)
{
   setType(B_OIL);
   _size = 2;
   _picture = &PicLoader::instance().get_picture("commerce", 99);

   AnimLoader animLoader(PicLoader::instance());
   animLoader.fill_animation(_animation, "commerce", 100, 8);
   _fgPictures.resize(2);
}

FactoryOil* FactoryOil::clone() const
{
   return new FactoryOil(*this);
}


FactoryPottery::FactoryPottery() : Factory(G_CLAY, G_POTTERY)
{
   setType(B_POTTERY);
   _size = 2;
   _picture = &PicLoader::instance().get_picture("commerce", 132);

   AnimLoader animLoader(PicLoader::instance());
   animLoader.fill_animation(_animation, "commerce", 133, 7);
   _fgPictures.resize(2);
}

FactoryPottery* FactoryPottery::clone() const
{
   return new FactoryPottery(*this);
}


FarmTile::FarmTile(const GoodType outGood, const int i, const int j)
{
   _i = i;
   _j = j;

   int picIdx = 0;
   switch (outGood)
   {
   case G_WHEAT:
      picIdx = 13;
      break;
   case G_VEGETABLE:
      picIdx = 18;
      break;
   case G_FRUIT:
      picIdx = 23;
      break;
   case G_OLIVE:
      picIdx = 28;
      break;
   case G_GRAPE:
      picIdx = 33;
      break;
   case G_MEAT:
      picIdx = 38;
      break;
   default:
      THROW("Unexpected farmType in farm:" << outGood);
   }

   AnimLoader animLoader(PicLoader::instance());
   animLoader.fill_animation(_animation, "commerce", picIdx, 5);
   computePicture(0);
}

void FarmTile::computePicture(const int percent)
{
   std::vector<Picture*> &pictures = _animation.get_pictures();

   int picIdx = (percent * (pictures.size()-1)) / 100;
   _picture = *pictures[picIdx];
   _picture.add_offset(30*(_i+_j), 15*(_j-_i));
}

Picture& FarmTile::getPicture()
{
   return _picture;
}


Farm::Farm(const GoodType outGood) : Factory(G_NONE, outGood)
{
   _size = 3;
   _picture = &_pictureBuilding;

   _pictureBuilding = PicLoader::instance().get_picture("commerce", 12);  // farm building
   _pictureBuilding.add_offset(30, 15);
   init();
}

void Farm::init()
{
   GoodType farmType = _outGoodType;
   // add subTiles in draw order
   _subTiles.push_back(FarmTile(farmType, 0, 0));
   _subTiles.push_back(FarmTile(farmType, 2, 2));
   _subTiles.push_back(FarmTile(farmType, 1, 0));
   _subTiles.push_back(FarmTile(farmType, 2, 1));
   _subTiles.push_back(FarmTile(farmType, 2, 0));

   _fgPictures.resize(5);
   for (int n = 0; n<5; ++n)
   {
      _fgPictures[n] = &_subTiles[n].getPicture();
   }
}

void Farm::computePictures()
{
   int amount = (int) _progress;
   int percentTile;

   for (int n = 0; n<5; ++n)
   {
      if (amount >= 20)   // 20 = 100 / nbSubTiles
      {
         // this subtile is at maximum
         percentTile = 100;  // 100%
         amount -= 20;  // for next subTiles
      }
      else
      {
         // this subtile is not at maximum
         percentTile = 5 * amount;
         amount = 0;  // for next subTiles
      }
      _subTiles[n].computePicture(percentTile);
   }
}


void Farm::timeStep(const unsigned long time)
{
   Factory::timeStep(time);

   computePictures();
}


FarmWheat::FarmWheat() : Farm(G_WHEAT)
{
   setType(B_WHEAT);
}

FarmWheat* FarmWheat::clone() const
{
   FarmWheat *res = new FarmWheat(*this);
   res->init();
   return res;
}


FarmOlive::FarmOlive() : Farm(G_OLIVE)
{
   setType(B_OLIVE);
}

FarmOlive* FarmOlive::clone() const
{
   FarmOlive *res = new FarmOlive(*this);
   res->init();
   return res;
}


FarmGrape::FarmGrape() : Farm(G_GRAPE)
{
   setType(B_GRAPE);
}

FarmGrape* FarmGrape::clone() const
{
   FarmGrape *res = new FarmGrape(*this);
   res->init();
   return res;
}


FarmMeat::FarmMeat() : Farm(G_MEAT)
{
   setType(B_MEAT);
}

FarmMeat* FarmMeat::clone() const
{
   FarmMeat *res = new FarmMeat(*this);
   res->init();
   return res;
}


FarmFruit::FarmFruit() : Farm(G_FRUIT)
{
   setType(B_FRUIT);
}

FarmFruit* FarmFruit::clone() const
{
   FarmFruit *res = new FarmFruit(*this);
   res->init();
   return res;
}


FarmVegetable::FarmVegetable() : Farm(G_VEGETABLE)
{
   setType(B_VEGETABLE);
}

FarmVegetable* FarmVegetable::clone() const
{
   FarmVegetable *res = new FarmVegetable(*this);
   res->init();
   return res;
}

Wharf::Wharf() : Factory(G_NONE, G_FISH)
{
  setType(B_WHARF);
  _size = 2;
  // transport 52 53 54 55
  setPicture(PicLoader::instance().get_picture("transport", 52));
}

Wharf* Wharf::clone() const
{
   return new Wharf(*this);
}

/* INCORRECT! */
bool Wharf::canBuild(const int i, const int j) const
{
  bool is_constructible = Construction::canBuild(i, j);

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
   
  std::list<Tile*> rect = tilemap.getRectangle(i - 1, j - 1, i + _size, j + _size, false);
  for (std::list<Tile*>::iterator itTiles = rect.begin(); itTiles != rect.end(); ++itTiles)
  {
    Tile &tile = **itTiles;
    std::cout << tile.getI() << " " << tile.getJ() << "  " << i << " " << j << std::endl;
      
     // if (tiles.get_terrain().isWater())
      
      if (tile.getJ() > (j + _size -1) && !tile.get_terrain().isWater()) {  bNorth = false; }
      if (tile.getJ() < j && !tile.get_terrain().isWater())              {  bSouth = false; }
      if (tile.getI() > (i + _size -1) && !tile.get_terrain().isWater()) {  bEast = false;  }
      if (tile.getI() < i && !tile.get_terrain().isWater())              {  bWest = false;  }      
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