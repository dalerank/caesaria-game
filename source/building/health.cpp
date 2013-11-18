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
#include "game/tilemap.hpp"
#include "game/city.hpp"
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
  _fgPicturesRef().resize(1);
}

unsigned int Baths::getWalkerDistance() const
{
  return 35;
}

void Baths::timeStep(const unsigned long time)
{
  if( time % 22 == 1 )
  {
    CityHelper helper( _getCity() );

    bool haveWater = false;
    TilesArray tiles = helper.getArea( this );
    foreach( Tile* tile, tiles )
    {
      haveWater |= tile->getWaterService( WTR_RESERVOIR ) > 0;
    }

    if( haveWater && getWorkersCount() > 0 )
    {
      if( _animationRef().isStopped() )
      {
        _animationRef().start();
        _haveReservorWater = true;
      }
    }
    else
    {
      if( _animationRef().isRunning() )
      {
       _animationRef().stop();
        _haveReservorWater = false;
        _fgPicturesRef().back() = Picture::getInvalid();
      }
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
