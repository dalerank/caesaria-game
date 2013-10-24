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

#include "oc3_building_health.hpp"
#include "oc3_resourcegroup.hpp"
#include "core/position.hpp"
#include "oc3_tile.hpp"

Doctor::Doctor() : ServiceBuilding(Service::doctor, B_DOCTOR, Size(1))
{
}

unsigned int Doctor::getWalkerDistance() const
{
  return 26;
}

void Doctor::deliverService()
{
  if( getWorkers() > 0 && getWalkerList().size() == 0 )
  {
    ServiceBuilding::deliverService();
  }
}

Hospital::Hospital() : ServiceBuilding(Service::hospital, B_HOSPITAL, Size(3 ) )
{
}

Baths::Baths() : ServiceBuilding(Service::baths, B_BATHS, Size(2) )
{
  _haveReservorWater = false;
  _getForegroundPictures().resize(2);

  _initAnimation();
}

unsigned int Baths::getWalkerDistance() const
{
  return 35;
}

void Baths::timeStep(const unsigned long time)
{
  if( time % 22 == 1 )
  {
    if( getTile().getWaterService( WTR_RESERVOIR ) > 0 && getWorkers() > 0 )
    {
      _getAnimation().start();
      _haveReservorWater = true;
    }
    else
    {
      _getAnimation().stop();
      _haveReservorWater = false;
      _getForegroundPictures().at(0) = Picture::getInvalid();
    }
  }

  ServiceBuilding::timeStep( time );
}

void Baths::deliverService()
{
  if( _haveReservorWater && getWalkerList().empty() )
  {
    ServiceBuilding::deliverService();
  }
}

void Baths::_initAnimation()
{
  _getAnimation().load( ResourceGroup::security, 22, 10);
  _getAnimation().setOffset( Point( 23, 25 ) );
  _getAnimation().setFrameDelay( 2 );
  _getAnimation().stop();
}

Barber::Barber() : ServiceBuilding(Service::barber, B_BARBER, Size(1))
{
}

void Barber::deliverService()
{

}

unsigned int Barber::getWalkerDistance() const
{
  return 35;
}
