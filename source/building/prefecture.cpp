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

#include "prefecture.hpp"
#include "gfx/picture.hpp"
#include "game/resourcegroup.hpp"
#include "core/position.hpp"
#include "walker/prefect.hpp"
#include "game/astarpathfinding.hpp"
#include "gfx/tile.hpp"
#include "game/path_finding.hpp"
#include "game/city.hpp"
#include "building/constants.hpp"

Prefecture::Prefecture() : ServiceBuilding(Service::prefect, constants::building::prefecture, Size(1))
{
  _fireDetect = TilePos( -1, -1 );
  setPicture( ResourceGroup::security, 1 );
  
  _animationRef().load( ResourceGroup::security, 2, 10);
  _animationRef().setDelay( 4 );
  _animationRef().setOffset( Point( 20, 36 ) );
  _fgPicturesRef().resize(1);
}

int Prefecture::getServiceDelay() const
{
  float koeff = ( getWorkersCount() > 0 ) ? (float)getMaxWorkers() / (float)getWorkersCount() : 1.f;
  return (int)(ServiceBuilding::getServiceDelay() * koeff);
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
    bool fireDetect = _fireDetect.getI() >= 0;
    PrefectPtr walker = Prefect::create( _getCity() );
    walker->setMaxDistance( 26 );

    //bool patrol = true;
    if( fireDetect )
    {
      Pathway pathway;
      TilePos startPos = getAccessRoads().front()->getIJ();
      bool pathFounded = Pathfinder::getInstance().getPath( startPos, _fireDetect, pathway, false );
      //patrol = !pathFounded;

      if( pathFounded )
      {
        walker->setIJ( pathway.getOrigin().getIJ() );
        walker->setPathway( pathway );
      }

      _fireDetect = TilePos( -1, -1 );
    }
    
    walker->send2City( PrefecturePtr( this ), fireDetect ? 200 : 0 );

    addWalker( walker.as<Walker>() );
  }
}

void Prefecture::fireDetect( const TilePos& pos )
{
  _fireDetect = pos;
}
