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

#include "oc3_farm.hpp"
#include "oc3_positioni.hpp"
#include "oc3_exception.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_scenario.hpp"
#include "oc3_tile.hpp"
#include "oc3_city.hpp"

class FarmTile
{
public:
  FarmTile(const GoodType outGood, const TilePos& pos );
  virtual ~FarmTile();
  void computePicture(const int percent);
  Picture& getPicture();

private:
  TilePos _pos;
  Picture _picture;
  Animation _animation;
};

FarmTile::FarmTile(const GoodType outGood, const TilePos& pos )
{
  _pos = pos;

  int picIdx = 0;
  switch (outGood)
  {
  case G_WHEAT: picIdx = 13; break;
  case G_VEGETABLE: picIdx = 18; break;
  case G_FRUIT: picIdx = 23; break;
  case G_OLIVE: picIdx = 28; break;
  case G_GRAPE: picIdx = 33; break;
  case G_MEAT: picIdx = 38; break;
  default:
    THROW("Unexpected farmType in farm:" << outGood);
  }

  _animation.load( ResourceGroup::commerce, picIdx, 5);
  computePicture(0);
}

void FarmTile::computePicture(const int percent)
{
  PicturesArray& pictures = _animation.getPictures();

  int picIdx = (percent * (pictures.size()-1)) / 100;
  _picture = *pictures[picIdx];
  _picture.addOffset(30*(_pos.getI()+_pos.getJ()), 15*(_pos.getJ()-_pos.getI() ));
}

Picture& FarmTile::getPicture()
{
  return _picture;
}

FarmTile::~FarmTile()
{

}
class Farm::Impl
{
public:
  std::vector<FarmTile> subTiles;
  Picture pictureBuilding;  // we need to change its offset
};

Farm::Farm(const GoodType outGood, const BuildingType type ) 
: Factory(G_NONE, outGood, type, Size(3) ), _d( new Impl )
{
  _d->pictureBuilding = Picture::load( ResourceGroup::commerce, 12);  // farm building
  _d->pictureBuilding.addOffset(30, 15);

  setPicture( _d->pictureBuilding );
  init();
  setWorkers( 0 );
}

bool Farm::canBuild(const TilePos& pos ) const
{
  bool is_constructible = Construction::canBuild( pos );
  bool on_meadow = false;

  Tilemap& tilemap = Scenario::instance().getCity()->getTilemap();
  PtrTilesArea rect = tilemap.getFilledRectangle( pos, getSize() );
  for( PtrTilesArea::iterator itTiles = rect.begin(); itTiles != rect.end(); ++itTiles)
  {
    on_meadow |= (*itTiles)->getTerrain().isMeadow();
  }

  return (is_constructible && on_meadow);  
}


void Farm::init()
{
  GoodType farmType = getOutGoodType();
  // add subTiles in draw order
  _d->subTiles.push_back(FarmTile(farmType, TilePos( 0, 0 ) ));
  _d->subTiles.push_back(FarmTile(farmType, TilePos( 2, 2 ) ));
  _d->subTiles.push_back(FarmTile(farmType, TilePos( 1, 0 ) ));
  _d->subTiles.push_back(FarmTile(farmType, TilePos( 2, 1 ) ));
  _d->subTiles.push_back(FarmTile(farmType, TilePos( 2, 0 ) ));

  _fgPictures.resize(5);
  for (int n = 0; n<5; ++n)
  {
    _fgPictures[n] = &_d->subTiles[n].getPicture();
  }
}

void Farm::computePictures()
{
  int amount = getProgress();
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
    _d->subTiles[n].computePicture(percentTile);
  }
}


void Farm::timeStep(const unsigned long time)
{
  Factory::timeStep(time);

  computePictures();
}

void Farm::save( VariantMap& stream ) const
{
  Factory::save( stream );
}

void Farm::load( const VariantMap& stream )
{
  Factory::load( stream );
}

Farm::~Farm()
{

}

FarmWheat::FarmWheat() : Farm(G_WHEAT, B_WHEAT_FARM)
{
}

FarmOlive::FarmOlive() : Farm(G_OLIVE, B_OLIVE_FARM)
{
}

FarmGrape::FarmGrape() : Farm(G_GRAPE, B_GRAPE_FARM)
{
}

FarmMeat::FarmMeat() : Farm(G_MEAT, B_PIG_FARM)
{
}

FarmFruit::FarmFruit() : Farm(G_FRUIT, B_FRUIT_FARM)
{
}

FarmVegetable::FarmVegetable() : Farm(G_VEGETABLE, B_VEGETABLE_FARM)
{
}
