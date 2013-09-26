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

#include "oc3_building_engineer_post.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_positioni.hpp"


BuildingEngineer::BuildingEngineer() : ServiceBuilding( Service::engineer, B_ENGINEER_POST, Size(1) )
{
  setWorkers( 0 );
  setPicture( Picture::load( ResourceGroup::buildingEngineer, 56 ) );

  _getAnimation().load( ResourceGroup::buildingEngineer, 57, 10 );
  _getAnimation().setFrameDelay( 4 );
  _getAnimation().setOffset( Point( 10, 42 ) );
  _fgPictures.resize(1);
}

void BuildingEngineer::timeStep(const unsigned long time)
{
  bool mayAnimate = getWorkers() > 0;

  if( mayAnimate && _getAnimation().isStopped() )
  {
      _getAnimation().start();
  }

  if( !mayAnimate && _getAnimation().isRunning() )
  {
      _getAnimation().stop();
  }

  ServiceBuilding::timeStep( time );
}

void BuildingEngineer::deliverService()
{
  if( getWorkers() > 0 && getWalkerList().size() == 0 )
  {
      ServiceBuilding::deliverService();
  }
}

unsigned int BuildingEngineer::getWalkerDistance() const
{
  return 26;
}
