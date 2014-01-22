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

#include "prefecture.hpp"
#include "gfx/picture.hpp"
#include "game/resourcegroup.hpp"
#include "core/position.hpp"
#include "walker/prefect.hpp"
#include "pathway/astarpathfinding.hpp"
#include "gfx/tile.hpp"
#include "pathway/path_finding.hpp"
#include "gfx/tilemap.hpp"
#include "city/city.hpp"
#include "objects/constants.hpp"

class Prefecture::Impl
{
public:
  TilePos fireDetect;
};

Prefecture::Prefecture()
  : ServiceBuilding(Service::prefect, constants::building::prefecture, Size(1)),
    _d( new Impl )
{
  _d->fireDetect = TilePos( -1, -1 );
  _fireIncrement = 0;
  setPicture( ResourceGroup::security, 1 );
  
  _animationRef().load( ResourceGroup::security, 2, 10);
  _animationRef().setDelay( 4 );
  _animationRef().setOffset( Point( 20, 36 ) );
  _fgPicturesRef().resize(1);
}

Prefecture::~Prefecture()
{

}

void Prefecture::timeStep(const unsigned long time)
{
  bool mayAnimate = getWorkersCount() > 0;

  if( mayAnimate  )
  {
    if( _animationRef().isStopped() )
    {
      _animationRef().setIndex( 0 );
      _animationRef().start();
    }
  }
  else if( _animationRef().isRunning() )
  {
    _animationRef().setIndex( -1 );
    _animationRef().stop();
    _fgPicturesRef().back() = Picture::getInvalid();
  }

  ServiceBuilding::timeStep( time );
}

void Prefecture::deliverService()
{
  if( getWorkersCount() > 0 && getWalkers().size() == 0 )
  {
    bool fireDetect = _d->fireDetect.i() >= 0;
    PrefectPtr walker = Prefect::create( _getCity() );
    walker->setMaxDistance( 26 );

    //bool patrol = true;
    if( fireDetect )
    {
      TilePos startPos = getAccessRoads().front()->getIJ();

      Tilemap& tmap = _getCity()->getTilemap();
      TilesArray arrivedArea = tmap.getArea( _d->fireDetect - TilePos( 1, 1), _d->fireDetect + TilePos( 1, 1 ) );
      Pathway pathway = Pathfinder::getInstance().getPath( startPos, arrivedArea, Pathfinder::terrainOnly );
      //patrol = !pathFounded;

      if( pathway.isValid() )
      {
        pathway.setNextTile( tmap.at( _d->fireDetect ) );
        walker->setIJ( pathway.getStartPos() );
        walker->setPathway( pathway );
      }
      else
      {
        fireDetect = false;
      }

      _d->fireDetect = TilePos( -1, -1 );
    }
    
    walker->send2City( PrefecturePtr( this ), fireDetect ? 200 : 0 );

    addWalker( walker.as<Walker>() );
  }
}

void Prefecture::fireDetect( const TilePos& pos )
{
  _d->fireDetect = pos;
}
