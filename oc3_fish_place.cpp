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

#include "oc3_fish_place.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_city.hpp"
#include "oc3_tilemap.hpp"

class FishPlace::Impl
{
public:
  int fishCount;
};

FishPlace::FishPlace() : LandOverlay( wtrFishPlace ), _d( new Impl )
{
  _getAnimation().setFrameDelay( 3 );
  _getForegroundPictures().resize( 1 );

  _d->fishCount = rand() % 100;

  if( _d->fishCount > 50 ) { _getAnimation().load( ResourceGroup::land3a, 19, 24); } //big fish place
  else { _getAnimation().load( ResourceGroup::land3a, 1, 18); } //small fish place
}

FishPlace::~FishPlace()
{

}

void FishPlace::build(CityPtr city, const TilePos& pos)
{
  setPicture( city->getTilemap().at( pos ).getPicture() );

  LandOverlay::build( city, pos );
}

void FishPlace::initTerrain(Tile& terrain)
{

}

void FishPlace::timeStep(const unsigned long time)
{
  _getAnimation().update( time );

  _getForegroundPictures().at(0) = _getAnimation().getCurrentPicture();
}
