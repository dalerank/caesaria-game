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

#include "education.hpp"
#include "game/resourcegroup.hpp"
#include "building/constants.hpp"

using namespace constants;

School::School() : ServiceBuilding(Service::school, building::school, Size(2))
{
  setPicture( ResourceGroup::commerce, 83 );
}

int School::getVisitorsNumber() const
{
  return 75;
}

void School::deliverService()
{
  if( getWorkersCount() <= 0 )
    return;

  if( getWalkers().size() < 3 )
  {
    ServiceBuilding::deliverService();
  }
}

unsigned int School::getWalkerDistance() const
{
  return 26;
}

Library::Library() : ServiceBuilding(Service::library, building::library, Size(2))
{
  setPicture( ResourceGroup::commerce, 84 );
}

int Library::getVisitorsNumber() const
{
  return 800;
}

Academy::Academy() : ServiceBuilding(Service::college, building::academy, Size(3))
{
  setPicture( ResourceGroup::commerce, 85 );
}

int Academy::getVisitorsNumber() const
{
  return 100;
}
