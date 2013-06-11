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

#include "oc3_prefecture.hpp"
#include "oc3_picture.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_positioni.hpp"
#include "oc3_walker_prefect.hpp"
#include "oc3_astarpathfinding.hpp"
#include "oc3_scenario.hpp"
#include "oc3_tile.hpp"

BuildingPrefecture::BuildingPrefecture() : ServiceBuilding(S_PREFECT, B_PREFECT, Size(1))
{
  _fireDetect = TilePos( -1, -1 );
  setPicture( Picture::load( ResourceGroup::security, 1 ) );
  

  _animation.load( ResourceGroup::security, 2, 10);
  _animation.setFrameDelay( 4 );
  _animation.setOffset( Point( 20, 36 ) );
  _fgPictures.resize(1);
}

int BuildingPrefecture::getServiceDelay() const
{
  float koeff = ( getWorkers() > 0 ) ? (float)getMaxWorkers() / (float)getWorkers() : 1.f;
  return (int)(ServiceBuilding::getServiceDelay() * koeff);
}

void BuildingPrefecture::timeStep(const unsigned long time)
{
  bool mayAnimate = getWorkers() > 0;

  if( mayAnimate && _animation.isStopped() )
  {
    _animation.start();
  }

  if( !mayAnimate && _animation.isRunning() )
  {
    _animation.stop();
  }

  ServiceBuilding::timeStep( time );
}

void BuildingPrefecture::deliverService()
{
  if( getWorkers() > 0 && getWalkerList().size() == 0 )
  {
    bool fireDetect = _fireDetect.getI() >= 0;
    WalkerPrefectPtr walker = WalkerPrefect::create( Scenario::instance().getCity() );

    bool patrol = true;
    if( fireDetect )
    {
      PathWay pathway;
      TilePos startPos = _accessRoads.front()->getIJ();
      bool pathFounded = Pathfinder::getInstance().getPath( startPos, _fireDetect, pathway, false, Size( 0 ) );
      patrol = !pathFounded;

      if( pathFounded )
      {
        walker->setPathWay( pathway );
        walker->setIJ( pathway.getOrigin().getIJ() );
      }

      _fireDetect = TilePos( -1, -1 );
    }
    
    walker->send2City( BuildingPrefecturePtr( this ), fireDetect ? 200 : 0 );

    addWalker( walker.as<Walker>() );
  }
}

void BuildingPrefecture::fireDetect( const TilePos& pos )
{
  _fireDetect = pos;
}