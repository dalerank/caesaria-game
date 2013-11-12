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

#include "health.hpp"
#include "game/resourcegroup.hpp"
#include "core/position.hpp"
#include "gfx/tile.hpp"
#include "constants.hpp"

using namespace constants;

Doctor::Doctor() : ServiceBuilding(Service::doctor, building::B_DOCTOR, Size(1))
{
}

unsigned int Doctor::getWalkerDistance() const
{
  return 26;
}

void Doctor::deliverService()
{
  if( getWorkersCount() > 0 && getWalkers().size() == 0 )
  {
    ServiceBuilding::deliverService();
  }
}

Hospital::Hospital() : ServiceBuilding(Service::hospital, building::B_HOSPITAL, Size(3 ) )
{
}

Baths::Baths() : ServiceBuilding(Service::baths, building::B_BATHS, Size(2) )
{
  _haveReservorWater = false;
  _fgPicturesRef().resize(2);

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
    if( getTile().getWaterService( WTR_RESERVOIR ) > 0 && getWorkersCount() > 0 )
    {
      _animationRef().start();
      _haveReservorWater = true;
    }
    else
    {
      _animationRef().stop();
      _haveReservorWater = false;
      _fgPicturesRef().at(0) = Picture::getInvalid();
    }
  }

  ServiceBuilding::timeStep( time );
}

void Baths::deliverService()
{
  if( _haveReservorWater && getWalkers().empty() )
  {
    ServiceBuilding::deliverService();
  }
}

void Baths::_initAnimation()
{
  _animationRef().load( ResourceGroup::security, 22, 10);
  _animationRef().setOffset( Point( 23, 25 ) );
  _animationRef().setDelay( 2 );
  _animationRef().stop();
}

Barber::Barber() : ServiceBuilding(Service::barber, building::B_BARBER, Size(1))
{
}

void Barber::deliverService()
{

}

unsigned int Barber::getWalkerDistance() const
{
  return 35;
}
