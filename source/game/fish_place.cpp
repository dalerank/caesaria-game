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

#include "fish_place.hpp"
#include "resourcegroup.hpp"
#include "city.hpp"
#include "tilemap.hpp"
#include "astarpathfinding.hpp"
#include "pathway.hpp"
#include "walker/walker.hpp"
#include "constants.hpp"

class FishPlace::Impl
{
public:
  Renderer::PassQueue passQueue;
  int fishCount;
  WalkerPtr walker;
  Picture const* savePicture;
  Point basicOffset;
  PicturesArray animations;
};

FishPlace::FishPlace() : TileOverlay( constants::place::fishPlace ), _d( new Impl )
{
  _getAnimation().setFrameDelay( 3 );
  _d->animations.resize( 1 );
  _d->passQueue.push_back( Renderer::foreground );
  _d->passQueue.push_back( Renderer::animations );

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

  TileOverlay::build( city, pos );
}

void FishPlace::initTerrain(Tile& terrain)
{

}

void FishPlace::timeStep(const unsigned long time)
{
  _getAnimation().update( time );

  _d->animations[ 0 ] = _getAnimation().getCurrentPicture();

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
  else
  {
    PathWay pathway;
    bool pathFound = Pathfinder::getInstance().getPath( getTilePos(), _getCity()->getBorderInfo().boatExit,
                                                        pathway, Pathfinder::waterOnly, Size( 1 ) );

    if( !pathFound )
    {
      deleteLater();
    }
    else
    {
      _d->walker = WalkerPtr( new Walker( _getCity() ) );
      _d->walker->drop();
      _d->walker->setSpeed( 0.1f );
      _d->walker->setPathWay( pathway );
      _d->walker->setIJ( getTilePos() );
      _d->walker->go();
    }
  }
}

void FishPlace::destroy()
{
  getTile().setOverlay( 0 );

  TileOverlay::destroy();
}

const PicturesArray& FishPlace::getPictures(Renderer::Pass pass) const
{
  switch(pass)
  {
  case Renderer::animations: return _d->animations;
  default: break;
  }

  return TileOverlay::getPictures( pass );
}

Renderer::PassQueue FishPlace::getPassQueue() const
{
  return _d->passQueue;
}
