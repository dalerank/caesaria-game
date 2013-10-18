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
#include "oc3_astarpathfinding.hpp"
#include "oc3_pathway.hpp"
#include "oc3_walker.hpp"

class FishPlace::Impl
{
public:
  int fishCount;
  WalkerPtr walker;
  Picture const* savePicture;
  Point basicOffset;
};

FishPlace::FishPlace() : TileOverlay( wtrFishPlace ), _d( new Impl )
{
  _getAnimation().setFrameDelay( 3 );
  _getForegroundPictures().resize( 1 );

  _d->fishCount = rand() % 100;

  if( _d->fishCount > 1 )
  {
    _getAnimation().load( ResourceGroup::land3a, 19, 24); //big fish place
    _d->basicOffset = Point( -41, 122 );
    _getAnimation().setOffset( _d->basicOffset );
  }
  else
  {
    _getAnimation().load( ResourceGroup::land3a, 1, 18);
    _d->basicOffset =  Point( 0, 55 );
    _getAnimation().setOffset( _d->basicOffset );
  } //small fish place
}

FishPlace::~FishPlace()
{

}

void FishPlace::build(CityPtr city, const TilePos& pos)
{
  _d->savePicture = &city->getTilemap().at( pos ).getPicture();
  setPicture( *_d->savePicture );

  PathWay pathway;
  bool pathFound = Pathfinder::getInstance().getPath( pos, city->getBorderInfo().boatExit,
                                                      pathway, Pathfinder::waterOnly, Size( 1 ) );

  if( !pathFound )
  {
    deleteLater();
  }
  else
  {
    _d->walker = WalkerPtr( new Walker( city ) );
    _d->walker->drop();
    _d->walker->setSpeed( 0.1f );
    _d->walker->setPathWay( pathway );
    _d->walker->setIJ( pos );
    _d->walker->go();
  }

  TileOverlay::build( city, pos );
}

void FishPlace::initTerrain(Tile& terrain)
{

}

void FishPlace::timeStep(const unsigned long time)
{
  _getAnimation().update( time );

  _getForegroundPictures().at(0) = _getAnimation().getCurrentPicture();

  if( _d->walker != 0 )
  {
    TilePos lastPos = _d->walker->getIJ();
    _d->walker->timeStep( time );

    if( lastPos != _d->walker->getIJ() )
    {
      getTile().setOverlay( 0 );
      getTile().setPicture( _d->savePicture );

      TilePos pos =  _d->walker->getIJ();

      _d->savePicture = &_getCity()->getTilemap().at( pos ).getPicture();
      setPicture( *_d->savePicture );

      TileOverlay::build( _getCity(), pos );
    }
    else if( lastPos == _d->walker->getPathway().getDestination().getIJ() )
    {
      deleteLater();
    }
  }
}

void FishPlace::destroy()
{
  getTile().setOverlay( 0 );

  TileOverlay::destroy();
}
