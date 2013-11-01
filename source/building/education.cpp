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

#include "education.hpp"
#include "game/resourcegroup.hpp"
#include "building/constants.hpp"

using namespace constants;

School::School() : ServiceBuilding(Service::school, building::B_SCHOOL, Size(2))
{
  setPicture( ResourceGroup::commerce, 83 );
}

int School::getVisitorsNumber() const
{
  return 75;
}

void School::deliverService()
{
  if( getWorkers() <= 0 )
    return;

  if( getWalkerList().size() < 3 )
  {
    ServiceBuilding::deliverService();
  }
}

unsigned int School::getWalkerDistance() const
{
  return 26;
}

Library::Library() : ServiceBuilding(Service::library, building::B_LIBRARY, Size(2))
{
  setPicture( ResourceGroup::commerce, 84 );
}

int Library::getVisitorsNumber() const
{
  return 800;
}

College::College() : ServiceBuilding(Service::college, building::B_COLLEGE, Size(3))
{
  setPicture( ResourceGroup::commerce, 85 );
}

int College::getVisitorsNumber() const
{
  return 100;
}
