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
#include "oc3_positioni.hpp"

Doctor::Doctor() : ServiceBuilding(S_DOCTOR, B_DOCTOR, Size(1))
{
  setPicture( Picture::load( ResourceGroup::security, 20));
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

Hospital::Hospital() : ServiceBuilding(S_HOSPITAL, B_HOSPITAL, Size(3 ) )
{
  setMaxWorkers( 30 );
  setPicture( Picture::load( ResourceGroup::security, 44));
}

Baths::Baths() : ServiceBuilding(S_BATHS, B_BATHS, Size(2) )
{
  setMaxWorkers( 10 );
  setPicture( Picture::load( ResourceGroup::security, 21));

  _getAnimation().load( ResourceGroup::security, 22, 10);
  _getAnimation().setOffset( Point( 23, 25 ) );
  _fgPictures.resize(2);
}

Barber::Barber() : ServiceBuilding(S_BARBER, B_BARBER, Size(1))
{
  setMaxWorkers( 2 );
  setPicture( Picture::load( ResourceGroup::security, 19));
}
