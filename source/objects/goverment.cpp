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

#include "goverment.hpp"
#include "game/resourcegroup.hpp"
#include "constants.hpp"

using namespace constants;

// housng1a 46 - governor's house    3 x 3
// housng1a 47 - governor's villa    4 x 4
// housng1a 48 - governor's palace   5 x 5

GovernorsHouse::GovernorsHouse() : WorkingBuilding( building::governorHouse, Size(3) )
{
  setMaximumWorkers(5);
  setPicture( ResourceGroup::housing, 46 );
}

GovernorsVilla::GovernorsVilla() : WorkingBuilding( building::governorVilla, Size(4) )
{
  setMaximumWorkers(10);
  setWorkers(0);    
  setPicture( ResourceGroup::housing, 47 );
}

GovernorsPalace::GovernorsPalace() : WorkingBuilding( building::governorPalace, Size( 5 ) )
{
  setMaximumWorkers(15);
  setWorkers(0);  
  setPicture( ResourceGroup::housing, 48 );
}

MissionaryPost::MissionaryPost() : WorkingBuilding(building::missionaryPost, Size(2) )
{
  setMaximumWorkers(20);
  setWorkers(0);  
  setPicture( ResourceGroup::transport, 93 );
}

